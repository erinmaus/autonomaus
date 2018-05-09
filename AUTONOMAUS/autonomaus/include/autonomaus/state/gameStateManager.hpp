// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_GAME_STATE_MANAGER_HPP
#define AUTONOMAUS_STATE_GAME_STATE_MANAGER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <map>
#include <thread>
#include <queue>
#include "kommon/deps/ctpl/ctpl.h"
#include "autonomaus/state/gameStateProvider.hpp"
#include "autonomaus/state/gameStateSlot.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "rosalind/common/updater.hpp"

namespace autonomaus
{
	class GameStateManager
	{
	public:
		GameStateManager();
		~GameStateManager();

		template <typename Provider, typename... Arguments>
		GameStateSlotView<Provider> add(Arguments&&... arguments);
		void remove(const BasicGameStateSlot& slot);

		template <typename Provider>
		void execute(
			const GameStateSlotView<Provider>& view,
			const std::function<void(Provider&)>& callback);
		void execute(
			const std::function<void(GameStateManager&)>& callback);

		void reload();
		void update(const touchstone::CallStream& call_stream);

		float get_last_update_timestamp() const;
		int get_last_update_frame() const;

		glooper::ResourceManager& get_resource_manager();
		rosalind::Updater& get_event_processors();

	private:
		void push_frame(const touchstone::CallStream& call_stream);

		std::atomic<bool> is_running;
		std::atomic<bool> pending_update;
		float pending_update_timestamp = 0.0f;
		float last_update_timestamp = 0.0f;
		int pending_update_frame = 0;
		int last_update_frame = 0;

		int num_frames = 0;
		float last_frame_average = 0.0f;
		float average_frame_time = 0.0f;

		typedef std::unique_ptr<BasicGameStateSlot> GameStateSlotPointer;
		std::map<BasicGameStateSlot::ID, GameStateSlotPointer> slots;
		BasicGameStateSlot::ID next_slot_id = BasicGameStateSlot::ID_BASE;

		typedef std::unique_ptr<BasicGameStateProvider> GameStateProviderPointer;
		std::map<BasicGameStateSlot::ID, GameStateProviderPointer> providers;

		glooper::ResourceManager resource_manager;
		rosalind::Updater updater;

		class Event
		{
		public:
			virtual ~Event() = default;
			virtual void execute(GameStateManager* manager) = 0;
		};

		class ReloadEvent : public Event
		{
		public:
			void execute(GameStateManager* manager) override;
		};

		class AttachProviderEvent : public Event
		{
		public:
			AttachProviderEvent(
				BasicGameStateProvider* provider,
				BasicGameStateSlot* slot);
			void execute(GameStateManager* manager) override;
		private:
			BasicGameStateProvider* provider;
			BasicGameStateSlot* slot;
		};

		class DetachProviderEvent : public Event
		{
		public:
			DetachProviderEvent(const BasicGameStateSlot* slot);
			void execute(GameStateManager* manager) override;
		private:
			GameStateManager* manager;
			BasicGameStateSlot::ID slot_id;
		};

		class ExecuteEvent : public Event
		{
		public:
			ExecuteEvent(const std::function<void()>& callback);
			void execute(GameStateManager* manager) override;
		private:
			std::function<void()> callback;
		};

		struct Frame
		{
			Frame();
			Frame(const touchstone::CallStream& call_stream);
			std::unique_ptr<std::uint8_t[]> data;
			touchstone::CallStream call_stream;
			float timestamp;
			int frame_num;
		};
		std::queue<Frame> pending_frames;
		int total_frames = 0;

		ctpl::thread_pool work_thread_pool;
		std::queue<std::unique_ptr<Event>> work_queue;
		std::condition_variable work_condition;
		std::mutex work_mutex;
		std::thread work_thread;
		void work();
		void process_work_queue();
		bool process_pending_frames(touchstone::GLTrace& trace);
		void synchronize_slots();
	};
}

template <typename Provider, typename... Arguments>
autonomaus::GameStateSlotView<Provider>
autonomaus::GameStateManager::add(Arguments&&... arguments)
{
	auto id = this->next_slot_id++;
	auto provider = new Provider(std::forward<Arguments>(arguments)...);
	auto slot = new GameStateSlot<Provider>(*this, id, *provider);
	provider->attach(*this, *slot);

	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->work_queue.emplace(new AttachProviderEvent(provider, slot));

	return GameStateSlotView<Provider>(*slot);
}

template <typename Provider>
void autonomaus::GameStateManager::execute(
	const GameStateSlotView<Provider>& view,
	const std::function<void(Provider&)>& callback)
{
	if (&view.get_slot().get_manager() != this)
	{
		return;
	}

	auto wrapped_callback = [this, callback, view]()
	{
		auto& slot = view.get_slot();
		callback(slot.get_provider());
	};
	std::lock_guard<std::mutex> lock(this->work_mutex);
	this->work_queue.emplace(new ExecuteEvent(wrapped_callback));
}

#endif
