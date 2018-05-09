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

twoflower::Brochure::Actions::Actions(const Brochure& brochure) :
	brochure(&brochure),
	is_limited(false)
{
	// Nothing.
}

twoflower::Brochure::Actions::Actions(
	const Brochure& brochure, const Resource& resource) :
	brochure(&brochure),
	resource(resource),
	is_limited(true)
{
	// Nothing.
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::by_type(
	const std::string& type,
	const std::string& name) const
{
	std::string query = "SELECT * FROM ActionInstance WHERE action_type=:type";
	if (!name.empty())
	{
		query += " AND action_name=:name";
	}
	if (is_limited)
	{
		query += " AND resource_id=:id";
	}

	query += ";";

	auto statement = brochure->database->create_statement(query);
	statement.bind(":type", type);
	statement.bind(":name", name);
	statement.bind(":id", resource.get_id());
	return const_iterator(*brochure->database, statement);
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::getters() const
{
	std::string query = "SELECT * FROM ActionInstance";
	query += " INNER JOIN ActionDefinition"
		" ON ActionDefinition.type=ActionInstance.action_type"
		" AND ActionDefinition.name=ActionInstance.action_name"
		" AND ActionDefinition.getter=1";
	if (is_limited)
	{
		query += " WHERE resource_id=:id";
	}
	query += ";";

	auto statement = brochure->database->create_statement(query);
	statement.bind(":id", resource.get_id());
	return const_iterator(*brochure->database, statement);
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::definitions() const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM ActionDefinition");
	return const_iterator(*brochure->database, statement, false);
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::begin() const
{
	if (!is_limited)
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM ActionInstance;");
		return const_iterator(*brochure->database, statement);
	}
	else
	{
		auto statement = brochure->database->create_statement(
			"SELECT * FROM ActionInstance"
			" WHERE resource_id=?;");
		statement.bind(1, resource.get_id());
		return const_iterator(*brochure->database, statement);
	}
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::end() const
{
	return const_iterator();
}

bool twoflower::Brochure::Actions::has(const Action& action) const
{
	if (action.get_id())
	{
		return has(action.get_id());
	}
	else
	{
		return brochure->has_action_definition(action.get_type(), action.get_name());
	}
}

bool twoflower::Brochure::Actions::has(int id) const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM ActionInstance WHERE id=?;");
	statement.bind(1, id);

	return statement.execute() == 1;
}

twoflower::Action twoflower::Brochure::Actions::get(int id) const
{
	if (!has(id))
	{
		throw std::runtime_error("action instance not in brochure; cannot get");
	}

	auto statement = brochure->database->create_statement(
		"SELECT * FROM ActionInstance WHERE id=?;");
	statement.bind(1, id);
	auto iter = const_iterator(*brochure->database, statement);
	return *iter;
}

twoflower::Brochure::Actions::const_iterator::const_iterator(
	Database& database, Statement& statement, bool is_instance) :
	database(&database),
	statement(new Statement(statement)),
	is_instance(is_instance),
	end(false)
{
	next();
}

twoflower::Brochure::Actions::const_iterator::~const_iterator()
{
	// Nothing.
}

twoflower::Brochure::Actions::const_iterator&
twoflower::Brochure::Actions::const_iterator::operator ++()
{
	next();
	return *this;
}

twoflower::Brochure::Actions::const_iterator
twoflower::Brochure::Actions::const_iterator::operator ++(int)
{
	next();
	return *this;
}

bool twoflower::Brochure::Actions::const_iterator::operator ==(
	const const_iterator& other) const
{
	return statement.get() == other.statement.get() || (end && other.end);
}

bool twoflower::Brochure::Actions::const_iterator::operator !=(
	const const_iterator& other) const
{
	return !(*this == other);
}

twoflower::Brochure::Actions::const_iterator::pointer
twoflower::Brochure::Actions::const_iterator::operator ->() const
{
	return &value;
}

twoflower::Brochure::Actions::const_iterator::value_type
twoflower::Brochure::Actions::const_iterator::operator *() const
{
	return value;
}

void twoflower::Brochure::Actions::const_iterator::next()
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
		Action::Builder builder(value);

		int id;
		statement->get("id", id);
		builder.set_id(id);

		std::string type;
		if (is_instance)
		{
			statement->get("action_type", type);
		}
		else
		{
			statement->get("type", type);
		}
		builder.set_type(type);

		std::string name;
		if (is_instance)
		{
			statement->get("action_name", name);
		}
		else
		{
			statement->get("name", name);
		}
		builder.set_name(name);

		int getter;
		if (is_instance)
		{
			auto getter_statement = database->create_statement(
				"SELECT getter FROM ActionDefinition WHERE type=? AND name=?");
			getter_statement.bind(1, type);
			getter_statement.bind(2, name);
			getter_statement.next();

			getter_statement.get(0, getter);
		}
		else
		{
			statement->get("getter", getter);
		}
		builder.set_is_getter(getter);

		std::string task;
		if (statement->empty("task"))
		{
			auto task_statement = database->create_statement(
				"SELECT task FROM ActionDefinition WHERE type=? AND name=?");
			task_statement.bind(1, type);
			task_statement.bind(2, name);
			task_statement.next();

			task_statement.get(0, task);
		}
		else
		{
			statement->get("task", task);
		}
		builder.set_task(task);

		float cost;
		if (statement->empty("cost"))
		{
			auto cost_statement = database->create_statement(
				"SELECT cost FROM ActionDefinition WHERE type=? AND name=?");
			cost_statement.bind(1, type);
			cost_statement.bind(2, name);
			cost_statement.next();

			cost_statement.get(0, cost);
		}
		else
		{
			statement->get("cost", cost);
		}
		builder.set_cost_multiplier(cost);
	}
}
