// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/regex.hpp>
#include <boost/filesystem/convenience.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "mantorok/io/datReader.hpp"
#include "mantorok/io/datWriter.hpp"

struct autonomaus::Cortex::MotionAction : public Action
{
	MotionAction(MotionSequence& sequence);
	~MotionAction();

	Cortex::Result get_result() override;
	void update() override;

	MotionSequence sequence;
	Cortex::Result::StatusReference status;
	Cortex::Result result;
};

autonomaus::Cortex::Cortex(const std::string& name, Autonomaus& autonomaus)
{
	this->name = name;
	this->autonomaus = &autonomaus;

	this->path = Autonomaus::get_config_path("memories");
	{
		auto p = 0;
		auto n = name.find("::");
		while (n != std::string::npos)
		{
			this->path.append(name.substr(p, n - p));
			p = n + 2;
			n = name.find("::", n + 2);
		}

		if (!boost::filesystem::exists(this->path))
		{
			boost::filesystem::create_directories(this->path);
		}

		this->path.append(name.substr(p, n - p) + ".dat");
	}

	load_memories();
}

autonomaus::Cortex::~Cortex()
{
	save_memories();
}

const std::string& autonomaus::Cortex::get_name() const
{
	return this->name;
}

void autonomaus::Cortex::update()
{
	for (auto i = this->actions.begin(); i != this->actions.end(); /* nop. */)
	{
		auto& action = *i;

		action->update();
		auto result = action->get_result();
		if (result.get() == Cortex::Result::Status::failure)
		{
			result.cancel();
		}
		
		if (result.get() != Cortex::Result::Status::working)
		{
			i = this->actions.erase(i);
		}
		else
		{
			++i;
		}
	}

	if (this->is_dirty)
	{
		save_memories();
		this->is_dirty = false;
	}
}

autonomaus::Cortex::Result autonomaus::Cortex::poke(
	const std::string& action,
	const Message& message)
{
	auto iter = this->poke_callbacks.find(action);
	if (iter != this->poke_callbacks.end())
	{
		return iter->second(message);
	}

	return Result(Result::Status::failure);
}

autonomaus::Message autonomaus::Cortex::pull(
	const std::string& action,
	const Message& message)
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	auto iter = this->pull_callbacks.find(action);
	if (iter != this->pull_callbacks.end())
	{
		return iter->second(message);
	}

	return Message();
}

autonomaus::Autonomaus& autonomaus::Cortex::get_autonomaus() const
{
	return *this->autonomaus;
}

int autonomaus::Cortex::count(const std::string& fuzzy_key) const
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);;


	boost::regex e(fuzzy_key);

	int count = 0;
	for (auto& i: this->memories)
	{
		if (boost::regex_match(i.first, e))
		{
			++count;
		}
	}

	return count;
}

bool autonomaus::Cortex::has_memory(const std::string& key) const
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	return this->memories.count(key) != 0;
}

void autonomaus::Cortex::remember(const std::string& key, const Memory& value)
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	auto iter = this->memories.find(key);
	if (iter != this->memories.end())
	{
		if (iter->second == value)
		{
			return;
		}
	}

	this->memories[key] = value;
	this->is_dirty = true;
}

void autonomaus::Cortex::forget(const std::string& key)
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	auto iter = this->memories.find(key);
	if (iter != this->memories.end())
	{
		this->memories.erase(iter);
		this->is_dirty = true;
	}
}

autonomaus::Memory autonomaus::Cortex::get(const std::string& key) const
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	if (has_memory(key))
	{
		return this->memories.find(key)->second;
	}

	return Memory();
}

autonomaus::Cortex::Memories autonomaus::Cortex::get_all(const std::string& fuzzy_key) const
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	Memories result;

	boost::regex e(fuzzy_key);
	for (auto& i: this->memories)
	{
		if (boost::regex_match(i.first, e))
		{
			result.insert(i);
		}
	}

	return result;
}

void autonomaus::Cortex::on_poke(
	const std::string& action,
	const PokeCallback& callback)
{
	this->poke_callbacks[action] = callback;
}

void autonomaus::Cortex::on_pull(
	const std::string& action,
	const PullCallback& callback)
{
	this->pull_callbacks[action] = callback;
}

void autonomaus::Cortex::ignore_poke(const std::string& action)
{
	this->poke_callbacks.erase(action);
}

void autonomaus::Cortex::ignore_pull(const std::string& action)
{
	this->pull_callbacks.erase(action);
}

autonomaus::Cortex::Result autonomaus::Cortex::perform(MotionSequence& sequence)
{
	return perform(new MotionAction(sequence));
}

autonomaus::Cortex::Result autonomaus::Cortex::perform(Action* action)
{
	this->actions.emplace_back(std::unique_ptr<Action>(action));

	return action->get_result();
}

void autonomaus::Cortex::load_memories()
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	std::ifstream stream(this->path.string());
	mantorok::DATReader reader(stream);

	if (reader.match_section("autonomaus.Cortex::root"))
	{
		std::string n;
		if (!reader.read<std::string>(n))
		{
			Log::warn(
				"(Cortex) Couldn't read Cortex name from '{}'."
				" Corrupt file?",
				this->path.string());
			return;
		}

		if (n != this->name)
		{
			Log::warn(
				"(Cortex) Name mis-match from '{}'. Expected '{}', got '{}'."
				" Memories may be invalid. Trying anyway.",
				this->path.string(),
				this->name,
				n);
		}

		mantorok::DATReader::Section section;
		while (reader.try_peek_section(section))
		{
			reader.enter_section();

			Memory memory;
			mantorok::DATDeserializer<Memory>::deserialize(memory, reader);

			this->memories[section.name] = memory;

			reader.leave_section();
		}

		reader.leave_section();
	}
}

void autonomaus::Cortex::save_memories()
{
	std::lock_guard<std::recursive_mutex> lock(this->mutex);

	std::ofstream stream(this->path.string());
	mantorok::DATWriter writer(stream);

	writer.begin_section("autonomaus.Cortex::root", mantorok::DATWriter::flag_clean);
	writer.write(this->name, "name");

	for (auto& i: this->memories)
	{
		writer.begin_section(i.first, mantorok::DATWriter::flag_clean);
		mantorok::DATSerializer<Memory>::serialize(i.second, writer);
		writer.end_section();
	}

	writer.end_section();
}

autonomaus::Cortex::const_memory_iterator autonomaus::Cortex::begin() const
{
	return this->memories.begin();
}

autonomaus::Cortex::memory_iterator autonomaus::Cortex::begin()
{
	return this->memories.begin();
}

autonomaus::Cortex::const_memory_iterator autonomaus::Cortex::end() const
{
	return this->memories.end();
}

autonomaus::Cortex::memory_iterator autonomaus::Cortex::end()
{
	return this->memories.end();
}

autonomaus::Cortex::Result::Result()
{
	this->data->status = std::make_unique<Status>(Status::empty);
	this->data->has_cancel_callback = false;
}

autonomaus::Cortex::Result::Result(Status status)
{
	this->data->status = std::make_unique<Status>(status);
	this->data->has_cancel_callback = false;
}

autonomaus::Cortex::Result::Result(const StatusReference& reference)
{
	this->data->status = reference;
	this->data->has_cancel_callback = false;
}

autonomaus::Cortex::Result::Result(const StatusReference& reference, const CancelCallback& callback)
{
	this->data->status = reference;
	this->data->cancel_callback = callback;
	this->data->has_cancel_callback = true;
}

autonomaus::Cortex::Result::Status autonomaus::Cortex::Result::get() const
{
	return *this->data->status;
}

bool autonomaus::Cortex::Result::pending() const
{
	return get() == Status::working;
}

void autonomaus::Cortex::Result::cancel()
{
	if (this->data->has_cancel_callback)
	{
		this->data->cancel_callback();
	}
}

void autonomaus::Cortex::Result::terminate()
{
	this->data->has_cancel_callback = false;
}

void autonomaus::Cortex::Result::terminate(Status status)
{
	terminate();
	*this->data->status = status;
}

bool autonomaus::Cortex::Result::message(const StatusReference& reference, const Message& message)
{
	if (this->data->status == reference)
	{
		this->data->message = message;
		return true;
	}

	return false;
}

const autonomaus::Message& autonomaus::Cortex::Result::message() const
{
	return this->data->message;
}

autonomaus::Cortex::Result::Impl::~Impl()
{
	if (this->has_cancel_callback)
	{
		this->cancel_callback();
	}
}

autonomaus::Cortex::MotionAction::MotionAction(MotionSequence& sequence) :
	sequence(std::move(sequence)),
	status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
	result(status, [this] { this->sequence.cancel(); this->result.terminate(); })
{
	// Nothing.
}

autonomaus::Cortex::MotionAction::~MotionAction()
{
	this->sequence.cancel();
	this->result.terminate();
}

autonomaus::Cortex::Result autonomaus::Cortex::MotionAction::get_result()
{
	return this->result;
}

void autonomaus::Cortex::MotionAction::update()
{
	*this->status = this->sequence.update();
}
