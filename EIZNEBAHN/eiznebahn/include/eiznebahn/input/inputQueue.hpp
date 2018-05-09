// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_INPUT_INPUT_QUEUE_HPP
#define EIZNEBAHN_INPUT_INPUT_QUEUE_HPP

#include <cassert>
#include <mutex>
#include <set>
#include <map>
#include <memory>
#include <queue>
#include <utility>
#include "eiznebahn/input/inputProvider.hpp"
#include "eiznebahn/input/inputSpoofer.hpp"

namespace eiznebahn
{
	class InputQueue
	{
	public:
		typedef int Group;

		InputQueue() = default;
		~InputQueue() = default;

		enum
		{
			channel_mouse = 0,
			channel_keyboard,
			channel_max
		};
		Group start(int channel);
		void stop();
		void cancel(Group group);

		bool pending() const;
		bool pending(Group group) const;

		template <typename Provider, typename... Args>
		void push(Args&&... arguments);

		void update(float delta, InputSpoofer& spoofer);

	private:
		mutable std::mutex mutex;

		typedef std::unique_ptr<BasicInputProvider> ProviderPointer;
		typedef std::queue<ProviderPointer> ProviderQueue;
		typedef std::map<Group, ProviderQueue> Channel;
		Channel channels[channel_max];

		std::set<Group> canceled;

		void update_channel(Channel& group, float delta, InputSpoofer& spoofer);
		bool update_queue(ProviderQueue& queue, float delta, InputSpoofer& spoofer);

		bool is_in_group = false;
		Group current_group = 0;
		int current_channel;

		std::map<Group, int /* channel */> pending_groups;
	};
}

template <typename Provider, typename... Args>
void eiznebahn::InputQueue::push(Args&&... arguments)
{
	assert(this->is_in_group);
	assert(this->current_channel == Provider::CHANNEL);
	assert(Provider::CHANNEL >= 0 && Provider::CHANNEL < channel_max);

	auto& group = this->channels[this->current_channel][this->current_group];
	group.emplace(new Provider(std::forward<Args>(arguments)...));
}

#endif
