// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/input/inputQueue.hpp"

eiznebahn::InputQueue::Group eiznebahn::InputQueue::start(int channel)
{
	assert(!this->is_in_group);
	assert(channel >= 0 && channel < channel_max);

	this->is_in_group = true;
	this->current_channel = channel;

	return ++this->current_group;
}

void eiznebahn::InputQueue::stop()
{
	std::lock_guard<std::mutex> lock(this->mutex);
	assert(this->is_in_group);
	this->is_in_group = false;
	this->pending_groups.emplace(this->current_group, this->current_channel);
}

void eiznebahn::InputQueue::cancel(Group group)
{
	std::lock_guard<std::mutex> lock(this->mutex);
	assert(this->pending_groups.count(group) != 0);

	auto& channel = this->channels[this->pending_groups[group]];
	auto& queue = channel[group];
	queue.front()->cancel();
	this->canceled.insert(group);
}

bool eiznebahn::InputQueue::pending() const
{
	return !this->pending_groups.empty();
}

bool eiznebahn::InputQueue::pending(Group group) const
{
	return this->pending_groups.count(group) != 0;
}

void eiznebahn::InputQueue::update(float delta, InputSpoofer& spoofer)
{
	std::lock_guard<std::mutex> lock(this->mutex);
	for (int i = 0; i < channel_max; ++i)
	{
		update_channel(this->channels[i], delta, spoofer);
	}
}

void eiznebahn::InputQueue::update_channel(Channel& channel, float delta, InputSpoofer& spoofer)
{
	bool pending = !channel.empty();
	while (pending)
	{
		auto current = channel.begin();
		auto group = current->first;
		auto& queue = current->second;
		bool safe_to_erase = update_queue(queue, delta, spoofer);
		if (queue.empty() || (safe_to_erase && this->canceled.count(group) != 0))
		{
			this->pending_groups.erase(group);
			channel.erase(current);
			pending = !channel.empty();
		}
		else
		{
			pending = false;
		}
	}
}

bool eiznebahn::InputQueue::update_queue(ProviderQueue& queue, float delta, InputSpoofer& spoofer)
{
	auto& provider = queue.front();
	if (!provider->done())
	{
		provider->update(delta, spoofer);
	}

	if (provider->done())
	{
		while (!queue.empty() && queue.front()->done())
		{
			queue.pop();
		}

		return true;
	}

	return false;
}
