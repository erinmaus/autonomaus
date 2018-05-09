// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_CORTEX_HPP

#include <list>
#include <mutex>
#include <boost/filesystem.hpp>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/memory/memory.hpp"
#include "autonomaus/state/memory/message.hpp"

// X11 bleeding through...
#ifdef Status
#undef Status
#endif

namespace autonomaus
{
	class MotionSequence;

	class Cortex
	{
	public:
		Cortex(const std::string& name, Autonomaus& autonomaus);
		virtual ~Cortex();

		const std::string& get_name() const;

		virtual void update();

		struct Result;
		virtual Result poke(const std::string& action, const Message& message);
		virtual Message pull(const std::string& action, const Message& message);

		Autonomaus& get_autonomaus() const;

	protected:
		int count(const std::string& fuzzy_key) const;
		bool has_memory(const std::string& key) const;
		void remember(const std::string& key, const Memory& value);
		void forget(const std::string& key);

		typedef std::map<std::string, Memory> Memories;
		typedef Memories::const_iterator const_memory_iterator;
		typedef Memories::iterator memory_iterator;

		const_memory_iterator begin() const;
		memory_iterator begin();
		const_memory_iterator end() const;
		memory_iterator end();

		Memory get(const std::string& key) const;
		Memories get_all(const std::string& fuzzy_key) const;

		typedef std::function<Result(const Message&)> PokeCallback;
		typedef std::function<Message(const Message&)> PullCallback;
		void on_poke(const std::string& action, const PokeCallback& callback);
		void on_pull(const std::string& action, const PullCallback& callback);
		void ignore_poke(const std::string& action);
		void ignore_pull(const std::string& action);

		struct Action
		{
			virtual ~Action() = default;
			virtual Cortex::Result get_result() = 0;
			virtual void update() = 0;
		};

		Cortex::Result perform(MotionSequence& motion);
		Cortex::Result perform(Action* action);

	private:
		std::string name;
		boost::filesystem::path path;
		Autonomaus* autonomaus;

		mutable std::recursive_mutex mutex;
		bool is_dirty = false;
		Memories memories;
		void load_memories();
		void save_memories();

		std::map<std::string, PokeCallback> poke_callbacks;
		std::map<std::string, PullCallback> pull_callbacks;

		std::list<std::unique_ptr<Action>> actions;

		struct MotionAction;
	};

	struct Cortex::Result
	{
	public:
		enum class Status
		{
			empty,
			working,
			success,
			failure
		};
		typedef std::shared_ptr<Status> StatusReference;
		typedef std::function<void()> CancelCallback;

		Result();
		Result(Status status);
		explicit Result(const StatusReference& reference);
		Result(const StatusReference& reference, const CancelCallback& callback);
		~Result() = default;

		Status get() const;
		bool pending() const;

		void cancel();

		void terminate();
		void terminate(Status status);

		bool message(const StatusReference& reference, const Message& message);
		const Message& message() const;

	private:
		struct Impl
		{
			Impl() = default;
			~Impl();

			std::shared_ptr<Status> status;
			CancelCallback cancel_callback;
			bool has_cancel_callback;
			Message message;
		};

		std::shared_ptr<Impl> data = std::make_shared<Impl>();
	};
}

#endif
