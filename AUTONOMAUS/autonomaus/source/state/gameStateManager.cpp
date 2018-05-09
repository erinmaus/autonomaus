// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include <algorithm>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/gameStateChannel.hpp"

autonomaus::GameStateManager::GameStateManager() :
	is_running(true),
	pending_update(false),
	updater(resource_manager.get_buffer_manager(), resource_manager.get_texture_manager()),
	work_thread([this] { work(); })
{
	// Nothing.
}

autonomaus::GameStateManager::~GameStateManager()
{
	this->is_running = false;
	this->work_condition.notify_one();
	this->work_thread.join();

	Log::info("Terminated game state work thread.");

	this->resource_manager.save_config(Autonomaus::get_config_path("autonomaus.ini"));
}

void autonomaus::GameStateManager::remove(const BasicGameStateSlot& slot)
{
	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->work_queue.emplace(new DetachProviderEvent(&slot));
}

void autonomaus::GameStateManager::execute(
	const std::function<void(GameStateManager&)>& callback)
{
	auto wrapped_callback = [this, callback]()
	{
		callback(*this);
	};

	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->work_queue.emplace(new ExecuteEvent(wrapped_callback));
}

void autonomaus::GameStateManager::reload()
{
	Log::info("Reloading resources...");

	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->work_queue.emplace(new ReloadEvent());
}

void autonomaus::GameStateManager::update(
	const touchstone::CallStream& call_stream)
{
	if (this->pending_update)
	{
		for (auto& i: this->slots)
		{
			i.second->pull();
		}

		this->pending_update = false;
		this->last_update_timestamp = this->pending_update_timestamp;
		this->last_update_frame = this->pending_update_frame;
		this->work_condition.notify_one();
	}

	push_frame(call_stream);
	this->work_condition.notify_one();
}

float autonomaus::GameStateManager::get_last_update_timestamp() const
{
	return this->last_update_timestamp;
}

int autonomaus::GameStateManager::get_last_update_frame() const
{
	return this->last_update_frame;
}

glooper::ResourceManager& autonomaus::GameStateManager::get_resource_manager()
{
	return this->resource_manager;
}

rosalind::Updater& autonomaus::GameStateManager::get_event_processors()
{
	return this->updater;
}

void autonomaus::GameStateManager::push_frame(
	const touchstone::CallStream& call_stream)
{
	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->pending_frames.emplace(call_stream);
	this->pending_frames.back().frame_num = this->total_frames++;

	++this->num_frames;

	float current_time = get_current_time();
	float difference = current_time - this->last_frame_average;
	if (difference >= 1.0f)
	{
		this->average_frame_time = difference / this->num_frames;
		this->last_frame_average = current_time;
		this->num_frames = 0;
	}
}

void autonomaus::GameStateManager::work()
{
	Log::info("Preparing game state work thread...");
	this->resource_manager.load_config(Autonomaus::get_config_path("autonomaus.ini"));
	this->updater.get_texture_processor().add(
		&this->resource_manager.get_texture_fingerprint_manager());
	this->updater.get_texture_processor().add(
		&this->resource_manager.get_texture_key_manager());
	this->updater.get_texture_processor().add(
		&this->resource_manager.get_item_icon_manager());
	this->updater.get_item_icon_processor().add(
		&this->resource_manager.get_item_icon_manager());

	int num_threads = this->resource_manager.get_config_int("autonomaus", "num_threads", 2);
	this->work_thread_pool.resize(std::max(num_threads, 1));

	Log::info("Entering game state work thread loop...");

	touchstone::GLTrace trace;
	while (this->is_running)
	{
		process_work_queue();

		if (this->is_running)
		{
			bool updated = process_pending_frames(trace);
			if (updated)
			{
				synchronize_slots();
			}
		}
	}

	Log::info("Terminating game state work thread...");

	this->slots.clear();
	this->providers.clear();
}

void autonomaus::GameStateManager::process_work_queue()
{
	std::unique_lock<std::mutex> lock(this->work_mutex);
	this->work_condition.wait(
		lock,
		[this]
		{
			return !this->is_running ||
				!this->pending_frames.empty() ||
				!this->work_queue.empty();
		});

	auto queue = std::move(this->work_queue);
	lock.unlock();

	while (!queue.empty())
	{
		queue.front()->execute(this);
		queue.pop();
	}
}

bool autonomaus::GameStateManager::process_pending_frames(touchstone::GLTrace& trace)
{
	float frame_delta;
	bool process_frames;
	{
		std::lock_guard<std::mutex> lock(this->work_mutex);
		frame_delta = this->average_frame_time;
		process_frames = !this->pending_frames.empty();
	}

	bool performed_update = false;
	while (process_frames)
	{
		Frame frame;
		{
			std::lock_guard<std::mutex> lock(this->work_mutex);

			frame = std::move(this->pending_frames.front());
			this->pending_frames.pop();

			process_frames = !this->pending_frames.empty();
		}

		float current_time = get_current_time();
		float difference = current_time - frame.timestamp;

		trace.extract(frame.call_stream);
		if (difference < frame_delta)
		{
			this->updater.enable_draw(get_current_time());
			this->updater.update(trace, get_current_time());
			this->updater.disable_draw(get_current_time());

			performed_update = true;
			process_frames = false;
			this->pending_update_timestamp = frame.timestamp;
			this->pending_update_frame = frame.frame_num;
		}
		else
		{
			this->updater.update(trace, get_current_time());
		}
		trace.clear();
	}

	return performed_update;
}

void autonomaus::GameStateManager::synchronize_slots()
{
	std::vector<std::future<void>> pending;
	for (auto& i: providers)
	{
		auto& provider = i.second;
		if (provider->get_channel() == GameStateIndependentChannel::TAG)
		{
			auto result = this->work_thread_pool.push([&provider](auto) { provider->update(); });
			pending.push_back(std::move(result));
		}
	}

	for (auto& p: pending)
	{
		p.wait();
	}

	for (auto& i: providers)
	{
		auto& provider = i.second;
		if (provider->get_channel() != GameStateIndependentChannel::TAG)
		{
			provider->update();
		}
	}

	std::unique_lock<std::mutex> lock(this->work_mutex);
	this->pending_update = true;
	this->work_condition.wait(
		lock,
		[this]
		{
			return !this->pending_update || !this->is_running;
		});
}

void autonomaus::GameStateManager::ReloadEvent::execute(
	GameStateManager* manager)
{
	auto path = Autonomaus::get_config_path("autonomaus.ini");
	manager->resource_manager.load_config(path);

	Log::info("Reloaded resources (path: '{0}')", path);
}

autonomaus::GameStateManager::AttachProviderEvent::AttachProviderEvent(
	BasicGameStateProvider* provider,
	BasicGameStateSlot* slot)
{
	this->provider = provider;
	this->slot = slot;
}

void autonomaus::GameStateManager::AttachProviderEvent::execute(
	GameStateManager* manager)
{
	manager->slots.emplace(
		this->slot->get_id(),
		GameStateSlotPointer(this->slot));
	manager->providers.emplace(
		this->slot->get_id(),
		GameStateProviderPointer(this->provider));
}

autonomaus::GameStateManager::DetachProviderEvent::DetachProviderEvent(
	const BasicGameStateSlot* slot)
{
	this->manager = &slot->get_manager();
	this->slot_id = slot->get_id();
}

void autonomaus::GameStateManager::DetachProviderEvent::execute(
	GameStateManager* manager)
{
	if (this->manager == manager)
	{
		manager->slots.erase(this->slot_id);
		manager->providers.erase(this->slot_id);
	}
}

autonomaus::GameStateManager::ExecuteEvent::ExecuteEvent(
	const std::function<void()>& callback)
{
	this->callback = callback;
}

void autonomaus::GameStateManager::ExecuteEvent::execute(
	GameStateManager* manager)
{
	callback();
}

autonomaus::GameStateManager::Frame::Frame() :
	call_stream(nullptr, 0)
{
	// Nothing.
}

autonomaus::GameStateManager::Frame::Frame(
	const touchstone::CallStream& call_stream) :
		data(new std::uint8_t[call_stream.get_frame_data_size()]),
		call_stream(data.get(), call_stream.get_frame_data_size())
{
	std::memcpy(
		this->data.get(), call_stream.get_frame_data(),
		call_stream.get_frame_data_size());
	this->timestamp = get_current_time();
}
