// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_MEMORY_HPP
#define AUTONOMAUS_STATE_MEMORY_MEMORY_HPP

#include <cmath>
#include <map>
#include <string>
#include "autonomaus/state/memory/message.hpp"
#include "mantorok/io/datDeserializer.hpp"
#include "mantorok/io/datSerializer.hpp"

namespace autonomaus
{
	class Memory
	{
	public:
		Memory() = default;
		~Memory() = default;

		float get_expiration() const;
		void set_expiration(float value);
		void set_expiration_from_now(float seconds);
		bool get_is_expired() const;

		Message* operator ->();
		const Message* operator ->() const;

		Message& operator *();
		const Message& operator *() const;

		Message& get();
		const Message& get() const;

		bool empty() const;

		bool operator ==(const Memory& other) const;

	private:
		float expiration = HUGE_VALF;
		Message message;
	};
}

namespace mantorok
{
	template <>
	struct DATDeserializer<autonomaus::Memory>
	{
		static void deserialize(autonomaus::Memory& value, DATReader& reader);
	};

	template <>
	struct DATSerializer<autonomaus::Memory>
	{
		static void serialize(const autonomaus::Memory& value, DATWriter& writer);
	};
}

#endif
