// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MESSAGE_HPP
#define AUTONOMAUS_STATE_MESSAGE_HPP

#include <map>
#include <string>
#include <vector>

namespace autonomaus
{
	class Message
	{
	public:
		struct Field;

		Message() = default;
		~Message() = default;

		bool valid() const;
		operator bool() const;

		std::string get_string(const std::string& field, std::size_t index = 0) const;
		void set_string(const std::string& field, const std::string& value, std::size_t index = 0);

		float get_single(const std::string& field, std::size_t index = 0) const;
		void set_single(const std::string& field, float value, std::size_t index = 0);

		int get_integer(const std::string& field, std::size_t index = 0) const;
		void set_integer(const std::string& field, int value, std::size_t index = 0);

		bool get_boolean(const std::string& field, std::size_t index = 0) const;
		void set_boolean(const std::string& field, bool value, std::size_t index = 0);

		Field get(const std::string& field) const;
		std::size_t length(const std::string& field) const;
		void length(const std::string& field, std::size_t value);
		bool has(const std::string& field) const;
		void unset(const std::string& field);

		bool operator ==(const Message& other) const;
		bool operator !=(const Message& other) const;

	private:
		typedef  std::map<std::string, Field> Fields;
		Fields fields;

	public:
		typedef Fields::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};

	struct Message::Field
	{
	public:
		Field() = default;

		enum
		{
			type_none,
			type_single,
			type_integer,
			type_string,
			type_boolean
		};

		int get_type() const;

		int get_integer(std::size_t index = 0) const;
		void set_integer(int value, std::size_t index = 0);

		float get_single(std::size_t index = 0) const;
		void set_single(float value, std::size_t index = 0);

		std::string get_string(std::size_t index = 0) const;
		void set_string(const std::string& value, std::size_t index = 0);

		bool get_boolean(std::size_t index = 0) const;
		void set_boolean(bool value, std::size_t index = 0);

		std::size_t length() const;
		void length(std::size_t value);

		bool operator ==(const Field& other) const;
		bool operator !=(const Field& other) const;

	private:
		int type = type_none;
		struct Value
		{
			int integer = 0;
			float single = 0.0f;
			std::string string;
			bool boolean = false;
		};
		std::vector<Value> values;
	};
}

#endif
