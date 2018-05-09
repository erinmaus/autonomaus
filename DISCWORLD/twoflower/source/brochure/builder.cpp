// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <cassert>
#include <stdexcept>
#include "twoflower/brochure.hpp"
#include "detail/database.hpp"
#include "detail/statement.hpp"
#include "detail/table.hpp"

twoflower::Brochure::Builder::Builder(Brochure& brochure) :
	brochure(&brochure)
{
	// Nothing.
}

void twoflower::Brochure::Builder::add_resource_type(const std::string& name)
{
	if (brochure->has_resource_type(name))
	{
		throw std::runtime_error("already has resource type; cannot add");
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ResourceType(name) VALUES(?);");
	statement.bind(1, name);
	statement.execute();
}

void twoflower::Brochure::Builder::remove_resource_type(const std::string& name)
{
	auto statement = brochure->database->create_statement(
		"DELETE FROM ResourceType WHERE name=?;");
	statement.bind(1, name);
	statement.execute();
}

twoflower::Resource twoflower::Brochure::Builder::add_resource(const Resource& resource)
{
	if (!brochure->has_resource_type(resource.get_type()))
	{
		std::string message = "missing resource type: ";
		message += resource.get_type();
		throw std::runtime_error(message);
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO Resource(name, resource_type)"
		" VALUES (?, ?);");
	if (!resource.get_name().empty())
	{
		statement.bind(1, resource.get_name());
	}
	statement.bind(2, resource.get_type());
	statement.execute();

	Resource result = resource;
	{
		auto row_statement = brochure->database->create_statement("SELECT last_insert_rowid();");
		row_statement.next();

		int id;
		row_statement.get(0, id);

		Resource::Builder builder(result);
		builder.set_id(id);
	}

	return result;
}

void twoflower::Brochure::Builder::update_resource(const Resource& resource)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot update");
	}

	auto statement = brochure->database->create_statement(
		"UPDATE Resource SET\n"
		" name=:name,\n"
		" resource_type=:type\n"
		"WHERE id=:id;\n");
	if (!resource.get_name().empty())
	{
		statement.bind(":name", resource.get_name());
	}
	statement.bind(":type", resource.get_type());
	statement.bind(":id", resource.get_id());
	statement.execute();
}

void twoflower::Brochure::Builder::add_tag(const Resource& resource, const std::string& value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add tag");
	}

	if (value.empty())
	{
		throw std::runtime_error("cannot insert empty tag");
	}

	if (!brochure->has_tag(resource, value))
	{
		auto statement = brochure->database->create_statement(
			"INSERT INTO ResourceTag(resource_id, value) VALUES(?, ?);");
		statement.bind(1, resource.get_id());
		statement.bind(2, value);

		statement.execute();
	}
}

void twoflower::Brochure::Builder::remove_tag(const Resource& resource, const std::string& value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add tag");
	}

	if (value.empty())
	{
		throw std::runtime_error("cannot remove empty tag");
	}

	if (brochure->has_tag(resource, value))
	{
		auto statement = brochure->database->create_statement(
			"DELETE FROM ResourceTag WHERE resource_id=? AND value=?;");
		statement.bind(1, resource.get_id());
		statement.bind(2, value);

		statement.execute();
	}
}

void twoflower::Brochure::Builder::remove_resource(const Resource& resource)
{
	auto requirements = brochure->database->create_statement(
		"SELECT action_instance_id FROM Requirement WHERE resource_id=?;");
	requirements.bind(1, resource.get_id());
	while (requirements.next())
	{
		int action_id;
		requirements.get(0, action_id);

		auto action_requirements_statement = brochure->database->create_statement(
			"SELECT id FROM Requirement WHERE action_instance_id=?;");
		action_requirements_statement.bind(1, action_id);
		while (action_requirements_statement.next())
		{
			int requirement_id;
			requirements.get(0, requirement_id);

			auto delete_requirement = brochure->database->create_statement(
				"DELETE FROM Requirement WHERE id=?;");

			delete_requirement.bind(1, requirement_id);
			delete_requirement.execute();
		}

		auto delete_action = brochure->database->create_statement(
			"DELETE FROM ActionInstance WHERE id=?;");
		delete_action.bind(1, action_id);
		delete_action.execute();
	}

	auto statement = brochure->database->create_statement(
		"DELETE FROM Resource WHERE id=?;");
	statement.bind(1, resource.get_id());
	statement.execute();
}

void twoflower::Brochure::Builder::update_requirement(const Requirement& requirement)
{
	if (!brochure->actions().has(requirement.get_action()))
	{
		throw std::runtime_error("action not in brochure; cannot update requirement");
	}

	if (!brochure->resources().has(requirement.get_resource()))
	{
		throw std::runtime_error("resource not in brochure; cannot update requirement");
	}

	auto statement = brochure->database->create_statement(
		"UPDATE Requirement SET\n"
		" resource_id=:resource_id,\n"
		" action_instance_id=:action_instance_id,\n"
		" count=:count,\n"
		" input=:input,\n"
		" output=:output\n"
		"WHERE id=:id;\n");
	statement.bind(":id", requirement.get_id());
	statement.bind(":resource_id", requirement.get_resource().get_id());
	statement.bind(":action_instance_id", requirement.get_action().get_id());
	statement.bind(":count", requirement.get_count());
	statement.bind(":input", requirement.is_input());
	statement.bind(":output", requirement.is_output());
	statement.execute();
}

void twoflower::Brochure::Builder::remove_requirement(const Requirement& requirement)
{
	auto statement = brochure->database->create_statement(
		"DELETE FROM Requirement WHERE id=?;");
	statement.bind(1, requirement.get_id());
	statement.execute();
}

void twoflower::Brochure::Builder::set_userdata(
	const Resource& resource,
	const std::string& field,
	int value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add userdata");
	}

	if (brochure->has_userdata(resource, field))
	{
		unset_userdata(resource, field);
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ResourceUserdata(resource_id, field, integer)"
		" VALUES(?, ?, ?);");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);
	statement.bind(3, value);
	statement.execute();
}

void twoflower::Brochure::Builder::set_userdata(
	const Resource& resource,
	const std::string& field,
	float value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add userdata");
	}

	if (brochure->has_userdata(resource, field))
	{
		unset_userdata(resource, field);
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ResourceUserdata(resource_id, field, single)"
		" VALUES(?, ?, ?);");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);
	statement.bind(3, value);
	statement.execute();
}

void twoflower::Brochure::Builder::set_userdata(
	const Resource& resource,
	const std::string& field,
	const std::string& value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add userdata");
	}

	if (brochure->has_userdata(resource, field))
	{
		unset_userdata(resource, field);
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ResourceUserdata(resource_id, field, string)"
		" VALUES(?, ?, ?);");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);
	statement.bind(3, value);
	statement.execute();
}

void twoflower::Brochure::Builder::set_userdata(
	const Resource& resource,
	const std::string& field,
	const std::vector<std::uint8_t>& value)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot add userdata");
	}

	if (brochure->has_userdata(resource, field))
	{
		unset_userdata(resource, field);
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ResourceUserdata(resource_id, field, data)"
		" VALUES(?, ?, ?);");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);
	statement.bind(3, value);
	statement.execute();
}

void twoflower::Brochure::Builder::unset_userdata(
	const Resource& resource,
	const std::string& field)
{
	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot remove userdata");
	}

	auto statement = brochure->database->create_statement(
		"DELETE FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);
	statement.execute();
}

void twoflower::Brochure::Builder::add_action_definition(const Action& action)
{
	if (brochure->has_action_definition(action.get_type(), action.get_name()))
	{
		throw std::runtime_error("action already in brochure; cannot add");
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ActionDefinition(type, name, getter, task, cost)"
		" VALUES (?, ?, ?, ?, ?);");
	statement.bind(1, action.get_type());
	statement.bind(2, action.get_name());
	statement.bind(3, action.is_getter());
	statement.bind(4, action.get_task());
	statement.bind(5, action.get_cost_multiplier());
	statement.execute();
}

void twoflower::Brochure::Builder::update_action(const Action& action)
{
	if (action.get_id())
	{
		auto statement = brochure->database->create_statement(
			"UPDATE ActionInstance SET\n"
			" action_type=:action_type,\n"
			" action_name=:action_name,\n"
			" task=:task,\n"
			" cost=:cost\n"
			"WHERE id=:id;\n");
		statement.bind(":id", action.get_id());
		statement.bind(":action_type", action.get_type());
		statement.bind(":action_name", action.get_name());
		if (!action.get_task().empty())
		{
			statement.bind(":task", action.get_task());
		}
		if (action.has_cost_multiplier())
		{
			statement.bind(":cost", action.get_cost_multiplier());
		}

		statement.execute();
	}
	else
	{
		if (!brochure->has_action_definition(action.get_type(), action.get_name()))
		{
			throw std::runtime_error("action not in brochure; cannot update");
		}

		auto statement = brochure->database->create_statement(
			"UPDATE ActionDefinition SET\n"
			" getter=:getter,\n"
			" task=:task,\n"
			" cost=:cost\n"
			"WHERE type = :type and name = :name;\n");
		statement.bind(":type", action.get_type());
		statement.bind(":name", action.get_name());
		statement.bind(":getter", action.is_getter());
		statement.bind(":task", action.get_task());
		statement.bind(":cost", action.get_cost_multiplier());

		statement.execute();
	}
}

void twoflower::Brochure::Builder::remove_action(const Action& action)
{
	if (action.get_id())
	{
		auto statement = brochure->database->create_statement(
			"DELETE FROM ActionInstance WHERE id=?");
		statement.bind(1, action.get_id());
		statement.execute();

		auto requirements = brochure->database->create_statement(
			"DELETE FROM Requirement WHERE action_instance_id=?;");
		requirements.bind(1, action.get_id());
		requirements.execute();
	}
	else
	{
		auto statement = brochure->database->create_statement(
			"DELETE FROM ActionDefinition WHERE type=? AND name=?;");
		statement.bind(1, action.get_type());
		statement.bind(2, action.get_name());
		statement.execute();
	}
}

twoflower::Action twoflower::Brochure::Builder::connect(
	const Action& action, const Resource& resource)
{
	if (!brochure->has_action_definition(action.get_type(), action.get_name()))
	{
		throw std::runtime_error("action definition not in brochure; cannot connect to resource");
	}

	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource instance not in brochure; cannot connect to action");
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO ActionInstance(action_type, action_name, resource_id) VALUES(?, ?, ?);");
	statement.bind(1, action.get_type());
	statement.bind(2, action.get_name());
	statement.bind(3, resource.get_id());
	statement.execute();

	int id;
	{
		auto row_statement = brochure->database->create_statement(
			"SELECT last_insert_rowid();");
		row_statement.next();
		row_statement.get(0, id);
	}

	if (!action.get_task().empty())
	{
		auto update_statement = brochure->database->create_statement(
			"UPDATE ActionInstance SET task=? WHERE id=?;");
		update_statement.bind(1, action.get_task());
		update_statement.bind(2, id);

		update_statement.execute();
	}

	if (action.has_cost_multiplier())
	{
		auto update_statement = brochure->database->create_statement(
			"UPDATE ActionInstance SET cost=? WHERE id=?;");
		update_statement.bind(1, action.get_cost_multiplier());
		update_statement.bind(2, id);

		update_statement.execute();
	}

	Action result = action;
	{
		Action::Builder builder(result);
		builder.set_id(id);
	}

	return result;
}

twoflower::Requirement twoflower::Brochure::Builder::connect(
	const Requirement& requirement, const Action& action, const Resource& resource)
{
	if (!brochure->actions().has(action.get_id()))
	{
		throw std::runtime_error("action instance not in brochure; cannot connect to requirement");
	}

	if (!brochure->resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot connect to requirement");
	}

	auto statement = brochure->database->create_statement(
		"INSERT INTO Requirement(resource_id, action_instance_id, count, input, output)"
		" VALUES(?, ?, ?, ?, ?);");
	statement.bind(1, resource.get_id());
	statement.bind(2, action.get_id());
	statement.bind(3, requirement.get_count());
	statement.bind(4, requirement.is_input());
	statement.bind(5, requirement.is_output());
	statement.next();

	int id;
	{
		auto row_statement = brochure->database->create_statement(
			"SELECT last_insert_rowid();");
		row_statement.next();
		row_statement.get(0, id);
	}

	Requirement result = requirement;
	{
		Requirement::Builder builder(result);
		builder.set_action(action);
		builder.set_resource(resource);
		builder.set_id(id);
	}

	return result;
}
