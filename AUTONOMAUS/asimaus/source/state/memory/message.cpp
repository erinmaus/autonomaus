// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include "autonomaus/state/memory/message.hpp"

bool autonomaus::Message::valid() const
{
	return !this->fields.empty();
}

autonomaus::Message::operator bool() const
{
	return valid();
}

std::string autonomaus::Message::get_string(const std::string& field, std::size_t index) const
{
	auto iter = this->fields.find(field);
	if (iter == this->fields.end())
	{
		return std::string();
	}

	return iter->second.get_string(index);
}

void autonomaus::Message::set_string(
	const std::string& field,
	const std::string& value,
	std::size_t index)
{
	this->fields[field].set_string(value, index);
}

float autonomaus::Message::get_single(const std::string& field, std::size_t index) const
{
	auto iter = this->fields.find(field);
	if (iter == this->fields.end())
	{
		return 0.0f;
	}

	return iter->second.get_single(index);
}

void autonomaus::Message::set_single(
	const std::string& field,
	float value,
	std::size_t index)
{
	this->fields[field].set_single(value, index);
}

int autonomaus::Message::get_integer(const std::string& field, std::size_t index) const
{
	auto iter = this->fields.find(field);
	if (iter == this->fields.end())
	{
		return 0;
	}

	return iter->second.get_integer(index);
}

void autonomaus::Message::set_integer(
	const std::string& field,
	int value,
	std::size_t index)
{
	this->fields[field].set_integer(value, index);
}

bool autonomaus::Message::get_boolean(const std::string& field, std::size_t index) const
{
	auto iter = this->fields.find(field);
	if (iter == this->fields.end())
	{
		return 0;
	}

	return iter->second.get_boolean(index);
}

void autonomaus::Message::set_boolean(
	const std::string& field,
	bool value,
	std::size_t index)
{
	this->fields[field].set_boolean(value, index);
}

autonomaus::Message::Field autonomaus::Message::get(const std::string& field) const
{
	auto iter = this->fields.find(field);
	if (iter != this->fields.end())
	{
		return iter->second;
	}

	return Field();
}

bool autonomaus::Message::has(const std::string& field) const
{
	return this->fields.count(field) != 0;
}

std::size_t autonomaus::Message::length(const std::string& field) const
{
	auto iter = this->fields.find(field);
	if (iter != this->fields.end())
	{
		return iter->second.length();
	}

	return 0;
}

void autonomaus::Message::length(const std::string& field, std::size_t value)
{
	auto iter = this->fields.find(field);
	if (iter != this->fields.end())
	{
		return iter->second.length(value);
	}
}

void autonomaus::Message::unset(const std::string& field)
{
	this->fields.erase(field);
}

bool autonomaus::Message::operator ==(const Message& other) const
{
	if (this->fields.size() != other.fields.size())
	{
		return false;
	}

	for (auto& i: this->fields)
	{
		if (!other.has(i.first))
		{
			return false;
		}
	}

	for (auto& i: other.fields)
	{
		if (!has(i.first))
		{
			return false;
		}
	}

	for (auto& i: this->fields)
	{
		auto iter = other.fields.find(i.first);
		if (iter == other.fields.end())
		{
			// ... This shouldn't happen.
			return false;
		}

		if (i.second != iter->second)
		{
			return false;
		}
	}

	return true;
}

bool autonomaus::Message::operator !=(const Message& other) const
{
	return !(*this == other);
}

autonomaus::Message::const_iterator autonomaus::Message::begin() const
{
	return this->fields.begin();
}

autonomaus::Message::const_iterator autonomaus::Message::end() const
{
	return this->fields.end();
}

int autonomaus::Message::Field::get_type() const
{
	return this->type;
}

int autonomaus::Message::Field::get_integer(std::size_t index) const
{
	if (index < this->values.size())
	{
		return this->values[index].integer;
	}

	return 0;
}

void autonomaus::Message::Field::set_integer(int value, std::size_t index)
{
	this->type = type_integer;
	this->values.resize(std::max(this->values.size(), index + 1));
	this->values[index].integer = value;
	this->values[index].single = value;
	this->values[index].string = std::to_string(value);
	this->values[index].boolean = (value != 0);
}

float autonomaus::Message::Field::get_single(std::size_t index) const
{
	if (index < this->values.size())
	{
		return this->values[index].single;
	}

	return 0.0f;
}

void autonomaus::Message::Field::set_single(float value, std::size_t index)
{
	this->type = type_single;
	this->values.resize(std::max(this->values.size(), index + 1));
	this->values[index].integer = value;
	this->values[index].single = value;
	this->values[index].string = std::to_string(value);
	this->values[index].boolean = (value != 0.0f);
}

std::string autonomaus::Message::Field::get_string(std::size_t index) const
{
	if (index < this->values.size())
	{
		return this->values[index].string;
	}

	return std::string();
}

void autonomaus::Message::Field::set_string(const std::string& value, std::size_t index)
{
	this->type = type_string;
	this->values.resize(std::max(this->values.size(), index + 1));
	this->values[index].integer = std::atoi(value.c_str());
	this->values[index].single = std::atof(value.c_str());
	this->values[index].string = value;

	if (value == "true" || this->values[index].integer != 0)
	{
		this->values[index].boolean = true;
	}
	else if (value == "false" || this->values[index].integer == 0 || value.empty())
	{
		this->values[index].boolean = false;
	}
}

std::size_t autonomaus::Message::Field::length() const
{
	return values.size();
}

void autonomaus::Message::Field::length(std::size_t value)
{
	return this->values.resize(value);
}

bool autonomaus::Message::Field::get_boolean(std::size_t index) const
{
	if (index < this->values.size())
	{
		return this->values[index].boolean;
	}

	return 0;
}

void autonomaus::Message::Field::set_boolean(bool value, std::size_t index)
{
	this->type = type_boolean;
	this->values.resize(std::max(this->values.size(), index + 1));
	this->values[index].integer = value;
	this->values[index].single = value;
	this->values[index].string = value ? "true" : "false";
	this->values[index].boolean = value;
}

bool autonomaus::Message::Field::operator ==(const Field& other) const
{
	if (this->values.size() != other.values.size())
	{
		return false;
	}

	if (this->type != other.type)
	{
		return false;
	}

	for (std::size_t i = 0; i < this->values.size(); ++i)
	{
		switch (this->type)
		{
			case type_integer:
			case type_single:
				if (this->values[i].integer != other.values[i].integer)
				{
					return false;
				}
				break;
			case type_string:
				if (this->values[i].string != other.values[i].string)
				{
					return false;
				}
			case type_boolean:
				if (this->values[i].boolean != other.values[i].boolean)
				{
					return false;
				}
				break;
			default:
				// Nothing.
				break;
		}
	}

	return true;
}

bool autonomaus::Message::Field::operator !=(const Field& other) const
{
	return !(*this == other);
}
