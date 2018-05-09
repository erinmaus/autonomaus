// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include "autonomaus/core/time.hpp"
#include "autonomaus/state/memory/memory.hpp"

float autonomaus::Memory::get_expiration() const
{
	return this->expiration;
}

void autonomaus::Memory::set_expiration(float value)
{
	this->expiration = value;
}

void autonomaus::Memory::set_expiration_from_now(float seconds)
{
	this->expiration = get_current_time() + seconds;
}

bool autonomaus::Memory::get_is_expired() const
{
	return this->expiration >= get_current_time();
}

autonomaus::Message* autonomaus::Memory::operator ->()
{
	return &this->message;
}

const autonomaus::Message* autonomaus::Memory::operator ->() const
{
	return &this->message;
}

autonomaus::Message& autonomaus::Memory::operator *()
{
	return this->message;
}

const autonomaus::Message& autonomaus::Memory::operator *() const
{
	return this->message;
}

autonomaus::Message& autonomaus::Memory::get()
{
	return this->message;
}

const autonomaus::Message& autonomaus::Memory::get() const
{
	return this->message;
}

bool autonomaus::Memory::empty() const
{
	return !this->message.valid();
}

bool autonomaus::Memory::operator ==(const Memory& other) const
{
	return this->expiration == other.expiration && this->message == other.message;
}

void mantorok::DATDeserializer<autonomaus::Memory>::deserialize(
	autonomaus::Memory& value,
	DATReader& reader)
{
	float expiration;
	reader.read(expiration);

	autonomaus::Message message;
	DATReader::Section section;
	while (reader.try_peek_section(section))
	{
		reader.enter_section();

		int type;
		reader.read(type);

		std::size_t length;
		reader.read(length);

		for (std::size_t i = 0; i < length; ++i)
		{
			switch (type)
			{
				case autonomaus::Message::Field::type_integer:
					{
						int value = 0;
						reader.read(value);
						message.set_integer(section.name, value, i);
					}
					break;
				case autonomaus::Message::Field::type_single:
					{
						float value = 0.0f;
						reader.read(value);
						message.set_single(section.name, value, i);
					}
					break;
				case autonomaus::Message::Field::type_string:
					{
						std::string value;
						reader.read(value);
						message.set_string(section.name, value, i);
					}
					break;
				case autonomaus::Message::Field::type_boolean:
					{
						int value = 0;
						reader.read(value);
						message.set_boolean(section.name, value, i);
					}
					break;
				default:
					reader.next();
					break;
			}
		}

		reader.leave_section();
	}

	value.set_expiration(expiration);
	value.get() = message;
}

void mantorok::DATSerializer<autonomaus::Memory>::serialize(
	const autonomaus::Memory& value,
	DATWriter& writer)
{
	writer.write(value.get_expiration());

	for (auto& i: *value)
	{
		writer.begin_section(i.first, DATWriter::flag_clean);

		writer.write<int>(i.second.get_type(), "type");
		writer.write<std::size_t>(i.second.length(), "length");

		for (std::size_t j = 0; j < i.second.length(); ++j)
		{
			switch (i.second.get_type())
			{
				case autonomaus::Message::Field::type_integer:
					{
						int value = i.second.get_integer(j);
						writer.write(value);
					}
					break;
				case autonomaus::Message::Field::type_single:
					{
						float value = i.second.get_single(j);
						writer.write(value);
					}
					break;
				case autonomaus::Message::Field::type_string:
					{
						std::string value = i.second.get_string(j);
						writer.write(value);
					}
					break;
				case autonomaus::Message::Field::type_boolean:
					{
						int value = i.second.get_boolean(j);
						writer.write(value);
					}
					break;
				default:
					writer.write<int>(0);
					break;
			}
		}

		writer.end_section();
	}
}
