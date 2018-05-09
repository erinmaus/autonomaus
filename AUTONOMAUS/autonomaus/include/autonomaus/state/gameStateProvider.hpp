// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_GAME_STATE_PROVIDER_HPP
#define AUTONOMAUS_STATE_GAME_STATE_PROVIDER_HPP

#include <cassert>
#include "autonomaus/state/gameStateSlot.hpp"
#include "autonomaus/state/gameStateChannel.hpp"

namespace autonomaus
{
	class GameStateManager;

	class BasicGameStateProvider
	{
	public:
		virtual ~BasicGameStateProvider() = default;

		virtual void update() = 0;
		virtual int get_channel() const = 0;
	};

	template <typename S, typename P, typename C>
	class GameStateProvider : public BasicGameStateProvider
	{
	public:
		typedef S State;
		typedef GameStateSlot<P> Slot;
		typedef GameStateSlotView<P> View;

		GameStateProvider() = default;
		virtual ~GameStateProvider() = default;

		virtual void attach(GameStateManager& manager, Slot& slot);
		virtual void detach();
		bool get_is_attached() const;

		GameStateManager* get_manager() const;
		Slot* get_slot() const;

		int get_channel() const override final;

	private:
		GameStateManager* manager = nullptr;
		Slot* slot = nullptr;
	};
}

template <typename S, typename P, typename C>
void autonomaus::GameStateProvider<S, P, C>::attach(GameStateManager& manager, Slot& slot)
{
	assert(!get_is_attached());
	this->manager = &manager;
	this->slot = &slot;
}

template <typename S, typename P, typename C>
void autonomaus::GameStateProvider<S, P, C>::detach()
{
	assert(get_is_attached());
	this->manager = nullptr;
	this->slot = nullptr;
}

template <typename S, typename P, typename C>
bool autonomaus::GameStateProvider<S, P, C>::get_is_attached() const
{
	return this->manager != nullptr && this->slot != nullptr;
}

template <typename S, typename P, typename C>
autonomaus::GameStateManager*
autonomaus::GameStateProvider<S, P, C>::get_manager() const
{
	return this->manager;
}

template <typename S, typename P, typename C>
typename autonomaus::GameStateProvider<S, P, C>::Slot*
autonomaus::GameStateProvider<S, P, C>::get_slot() const
{
	return this->slot;
}

template <typename S, typename P, typename C>
int autonomaus::GameStateProvider<S, P, C>::get_channel() const
{
	return C::TAG;
}

#endif
