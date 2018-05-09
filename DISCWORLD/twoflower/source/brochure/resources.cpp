// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <stdexcept>
#include "twoflower/brochure.hpp"
#include "detail/database.hpp"
#include "detail/statement.hpp"

twoflower::Brochure::Resources::Resources(const Brochure& brochure) :
	brochure(&brochure)
{
	// Nothing.
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::by_name(const std::string& name, const std::string& type) const
{
	if (!type.empty())
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM Resource WHERE name=? AND resource_type=?;");
		statement.bind(1, name);
		statement.bind(2, type);

		return const_iterator(statement);
	}
	else
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM Resource WHERE name=?;");
		statement.bind(1, name);

		return const_iterator(statement);
	}
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::by_fuzzy_name(const std::string& name, const std::string& type) const
{
	std::string fuzzy_name;
	fuzzy_name.resize(name.length() * 2 + 1, '%');
	for (std::size_t i = 0; i < name.length(); ++i)
	{
		fuzzy_name[i * 2 + 1] = name[i];
	}

	if (!type.empty())
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM Resource WHERE name LIKE ? COLLATE NOCASE AND resource_type=?;");
		statement.bind(1, fuzzy_name);
		statement.bind(2, type);

		return const_iterator(statement);
	}
	else
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM Resource WHERE name LIKE ? COLLATE NOCASE;");
			statement.bind(1, fuzzy_name);

			return const_iterator(statement);
	}
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::by_type(const std::string& type) const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM Resource WHERE resource_type=?;");
	statement.bind(1, type);

	return const_iterator(statement);
}

bool twoflower::Brochure::Resources::has(const Resource& resource) const
{
	return has(resource.get_id());
}

bool twoflower::Brochure::Resources::has(int id) const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM Resource WHERE id=?;");
	statement.bind(1, id);

	return statement.execute() == 1;
}

twoflower::Resource twoflower::Brochure::Resources::get(int id) const
{
	if (!has(id))
	{
		throw std::runtime_error("resource not in brochure; cannot get");
	}

	auto statement = brochure->database->create_statement(
		"SELECT * FROM Resource WHERE id=?;");
	statement.bind(1, id);
	auto iter = const_iterator(statement);
	return *iter;
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::begin() const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM Resource;");
	return const_iterator(statement);
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::end() const
{
	return const_iterator();
}

twoflower::Brochure::Resources::const_iterator::const_iterator(Statement& statement) :
	statement(new Statement(statement)),
	end(false)
{
	next();
}

twoflower::Brochure::Resources::const_iterator::~const_iterator()
{
	// Nothing.
}

twoflower::Brochure::Resources::const_iterator&
twoflower::Brochure::Resources::const_iterator::operator ++()
{
	next();
	return *this;
}

twoflower::Brochure::Resources::const_iterator
twoflower::Brochure::Resources::const_iterator::operator ++(int)
{
	next();
	return *this;
}

bool twoflower::Brochure::Resources::const_iterator::operator ==(
	const const_iterator& other) const
{
	return statement.get() == other.statement.get() || (end && other.end);
}

bool twoflower::Brochure::Resources::const_iterator::operator !=(
	const const_iterator& other) const
{
	return !(*this == other);
}

twoflower::Brochure::Resources::const_iterator::pointer
twoflower::Brochure::Resources::const_iterator::operator ->() const
{
	return &value;
}

twoflower::Brochure::Resources::const_iterator::value_type
twoflower::Brochure::Resources::const_iterator::operator *() const
{
	return value;
}

void twoflower::Brochure::Resources::const_iterator::next()
{
	if (end)
	{
		return;
	}

	if (!statement->next())
	{
		end = true;
	}
	else
	{
		Resource::Builder builder(value);

		int id;
		statement->get("id", id);
		builder.set_id(id);

		std::string name;
		statement->get("name", name);
		builder.set_name(name);

		std::string type;
		statement->get("resource_type", type);
		builder.set_type(type);
	}
}
