// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_GAME_STATE_SLOT_HPP
#define AUTONOMAUS_STATE_GAME_STATE_SLOT_HPP

#include <cassert>
#include <type_traits>

namespace autonomaus
{
	class GameStateManager;

	class BasicGameStateSlot
	{
	public:
		typedef int ID;
		static const ID ID_NONE;
		static const ID ID_BASE;

		BasicGameStateSlot(GameStateManager& manager, ID id);
		virtual ~BasicGameStateSlot() = default;

		GameStateManager& get_manager() const;
		ID get_id() const;

		virtual void pull() = 0;

	private:
		GameStateManager* manager = nullptr;
		ID id = ID_NONE;
	};

	template <typename Provider>
	class GameStateSlot : public BasicGameStateSlot
	{
	public:
		typedef typename Provider::State State;

		static_assert(
			std::is_copy_constructible<State>::value,
			"state must be copy-constructible");

		GameStateSlot(
			GameStateManager& manager, BasicGameStateSlot::ID id,
			Provider& provider);
		~GameStateSlot() = default;

		const State& get_current() const;
		State& get_working();

		Provider& get_provider() const;

		void pull() override;

	private:
		Provider* provider;
		State current;
		State working;
	};

	template <typename Provider>
	class GameStateSlotView
	{
	public:
		typedef typename Provider::State State;

		GameStateSlotView() = default;
		GameStateSlotView(const GameStateSlot<Provider>& slot);
		~GameStateSlotView() = default;

		const State& get() const;
		const GameStateSlot<Provider>& get_slot() const;

		const State* operator ->() const;
		const State& operator *() const;

		operator const BasicGameStateSlot&() const;
		operator const State&() const;

		bool valid() const;

	private:
		const GameStateSlot<Provider>* slot = nullptr;
	};
}

template <typename Provider>
autonomaus::GameStateSlot<Provider>::GameStateSlot(
	GameStateManager& manager,
	BasicGameStateSlot::ID id,
	Provider& provider) :
		BasicGameStateSlot(manager, id)
{
	this->provider = &provider;
}

template <typename Provider>
const typename autonomaus::GameStateSlot<Provider>::State&
autonomaus::GameStateSlot<Provider>::get_current() const
{
	return this->current;
}

template <typename Provider>
typename autonomaus::GameStateSlot<Provider>::State&
autonomaus::GameStateSlot<Provider>::get_working()
{
	return this->working;
}

template <typename Provider>
Provider& autonomaus::GameStateSlot<Provider>::get_provider() const
{
	return *this->provider;
}

template <typename Provider>
void autonomaus::GameStateSlot<Provider>::pull()
{
	this->current = this->working;
}

template <typename Provider>
autonomaus::GameStateSlotView<Provider>::GameStateSlotView(
	const GameStateSlot<Provider>& slot)
{
	this->slot = &slot;
}

template <typename Provider>
const typename autonomaus::GameStateSlotView<Provider>::State&
autonomaus::GameStateSlotView<Provider>::get() const
{
	assert(valid());
	return this->slot->get_current();
}

template <typename Provider>
const autonomaus::GameStateSlot<Provider>&
autonomaus::GameStateSlotView<Provider>::get_slot() const
{
	assert(valid());
	return *this->slot;
}

template <typename Provider>
const typename autonomaus::GameStateSlotView<Provider>::State*
autonomaus::GameStateSlotView<Provider>::operator ->() const
{
	return &get();
}

template <typename Provider>
const typename autonomaus::GameStateSlotView<Provider>::State&
autonomaus::GameStateSlotView<Provider>::operator *() const
{
	return get();
}

template <typename Provider>
autonomaus::GameStateSlotView<Provider>::operator const autonomaus::BasicGameStateSlot&() const
{
	return *get_slot();
}

template <typename Provider>
autonomaus::GameStateSlotView<Provider>::operator const State&() const
{
	return get();
}

template <typename Provider>
bool autonomaus::GameStateSlotView<Provider>::valid() const
{
	return this->slot != nullptr;
}

#endif
