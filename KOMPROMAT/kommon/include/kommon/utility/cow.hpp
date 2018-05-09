// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMMON_UTILITY_COW_HPP
#define KOMMON_UTILITY_COW_HPP

#include "kommon/utility/lightSharedPointer.hpp"

namespace kommon
{
	template <typename State>
	class Cow // moo
	{
	public:
		typedef State value_type;

		Cow();
		Cow(const Cow& other);
		explicit Cow(const State& value);
		~Cow() = default;

		class Guard;
		Guard writable();

		const value_type* get() const;

		const value_type* operator ->() const;
		const value_type& operator *() const;

		Cow& operator =(const Cow& other);

		class Guard
		{
		public:
			friend Cow;
			~Guard();

			value_type* operator ->();
			value_type& operator *();

		private:
			explicit Guard(const Cow* cow);

			const Cow* cow;
		};

	private:
		struct Data
		{
		public:
			Data() = default;
			explicit Data(const State& state);

			State state;
			bool is_locked = false;
		};

		typedef LightSharedPointer<Data> DataPointer;
		DataPointer data = DataPointer(new Data());
	};
}

template <typename State>
kommon::Cow<State>::Cow()
{
	this->data = DataPointer(new Data());
}

template <typename State>
kommon::Cow<State>::Cow(const State& value)
{
	this->data = DataPointer(new Data(value));
}

template <typename State>
kommon::Cow<State>::Cow(const Cow<State>& other)
{
	*this = other;
}

template <typename State>
typename kommon::Cow<State>::Guard kommon::Cow<State>::writable()
{
	if (this->data.use_count() > 1)
	{
		this->data = DataPointer(new Data(this->data->state));
	}

	return Guard(this);
}

template <typename State>
const State* kommon::Cow<State>::get() const
{
	return &this->data->state;
}

template <typename State>
const State* kommon::Cow<State>::operator ->() const
{
	return get();
}

template <typename State>
const State& kommon::Cow<State>::operator *() const
{
	return this->data->state;
}

template <typename State>
kommon::Cow<State>& kommon::Cow<State>::operator =(
	const Cow<State>& other)
{
	if (other.data->is_locked)
	{
		this->data = DataPointer(new Data(other.data->state));
	}
	else
	{
		this->data = other.data;
	}

	return *this;
}

template <typename State>
kommon::Cow<State>::Guard::Guard(const Cow<State>* cow)
{
	this->cow = cow;
	this->cow->data->is_locked = true;
}

template <typename State>
kommon::Cow<State>::Guard::~Guard()
{
	this->cow->data->is_locked = false;
}

template <typename State>
State* kommon::Cow<State>::Guard::operator ->()
{
	return &this->cow->data->state;
}

template <typename State>
State& kommon::Cow<State>::Guard::operator *()
{
	return this->cow->data->state;
}

template <typename State>
kommon::Cow<State>::Data::Data(const State& state) :
	state(state)
{
	// Nothing.
}

#endif
