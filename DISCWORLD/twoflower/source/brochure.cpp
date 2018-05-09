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
#include "brochure/detail/database.hpp"
#include "brochure/detail/statement.hpp"
#include "brochure/detail/table.hpp"

twoflower::Brochure::Brochure(const std::string& filename)
{
	database = std::make_shared<Database>(filename);

	make_tables();
}

twoflower::Brochure::~Brochure()
{
	// Nothing.
}

twoflower::Brochure::Builder twoflower::Brochure::builder()
{
	return Builder(*this);
}

bool twoflower::Brochure::has_resource_type(const std::string& name) const
{
	auto statement = database->create_statement(
		"SELECT * FROM ResourceType WHERE name=?;");
	statement.bind(1, name);

	return statement.execute() != 0;
}

bool twoflower::Brochure::has_action_definition(
	const std::string& type,
	const std::string& name) const
{
	auto statement = database->create_statement(
		"SELECT * FROM ActionDefinition WHERE type=? AND name=?;");
	statement.bind(1, type);
	statement.bind(2, name);

	return statement.execute() != 0;
}

bool twoflower::Brochure::get_userdata(
	const Resource& resource,
	const std::string& field,
	int& value) const
{
	auto statement = database->create_statement(
		"SELECT integer FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);

	if (statement.next())
	{
		statement.get(0, value);
		return true;
	}

	return false;
}

bool twoflower::Brochure::get_userdata(
	const Resource& resource,
	const std::string& field,
	float& value) const
{
	auto statement = database->create_statement(
		"SELECT single FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);

	if (statement.next())
	{
		statement.get(0, value);
		return true;
	}

	return false;
}

bool twoflower::Brochure::get_userdata(
	const Resource& resource,
	const std::string& field,
	std::string& value) const
{
	auto statement = database->create_statement(
		"SELECT string FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);

	if (statement.next())
	{
		statement.get(0, value);
		return true;
	}

	return false;
}

bool twoflower::Brochure::get_userdata(
	const Resource& resource,
	const std::string& field,
	std::vector<std::uint8_t>& value) const
{
	auto statement = database->create_statement(
		"SELECT data FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);

	if (statement.next())
	{
		statement.get(0, value);
		return true;
	}

	return false;
}

bool twoflower::Brochure::has_userdata(
	const Resource& resource,
	const std::string& field) const
{
	auto statement = database->create_statement(
		"SELECT * FROM ResourceUserdata WHERE resource_id=? AND field=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, field);

	return statement.execute() != 0;
}

bool twoflower::Brochure::has_tag(const Resource& resource, const std::string& value) const
{
	auto statement = database->create_statement(
		"SELECT value FROM ResourceTag WHERE resource_id=? and value=?;");
	statement.bind(1, resource.get_id());
	statement.bind(2, value);

	return statement.execute() != 0;
}

twoflower::Brochure::Tags twoflower::Brochure::tags(const Resource& resource) const
{
	auto statement = database->create_statement(
		"SELECT value FROM ResourceTag WHERE resource_id=?;");
	statement.bind(1, resource.get_id());

	Tags results;
	while (statement.next())
	{
		std::string result;
		statement.get(0, result);

		results.insert(result);
	}

	return results;
}

twoflower::Resource twoflower::Brochure::resource(const Action& action) const
{
	auto statement = database->create_statement(
		"SELECT * FROM Resource INNER JOIN ActionInstance"
		" ON Resource.id=ActionInstance.resource_id"
		" AND ActionInstance.id=?;");
	statement.bind(1, action.get_id());
	statement.next();

	int id;
	statement.get("id", id);

	return resources().get(id);
}

twoflower::Brochure::Resources twoflower::Brochure::resources() const
{
	return Resources(*this);
}

twoflower::Brochure::Actions twoflower::Brochure::actions() const
{
	return Actions(*this);
}

twoflower::Brochure::Actions twoflower::Brochure::actions(const Resource& resource) const
{
	if (!resources().has(resource))
	{
		throw std::runtime_error("resource not in brochure; cannot get actions");
	}

	return Actions(*this, resource);
}

twoflower::Brochure::Requirements twoflower::Brochure::requirements(const Action& action) const
{
	if (!actions().has(action))
	{
		throw std::runtime_error("action instance not in brochure; cannot get requirements");
	}

	return Requirements(*this, action);
}

void twoflower::Brochure::make_tables()
{
	Table action_definition("ActionDefinition");
	action_definition.add_primary_key("type", Table::Type::text);
	action_definition.add_primary_key("name", Table::Type::text);
	action_definition.add_column("getter", Table::Type::integer, false);
	action_definition.add_column("task", Table::Type::text, true);
	action_definition.add_column("cost", Table::Type::real, true);
	create_table(action_definition);

	Table resource_type("ResourceType");
	resource_type.add_primary_key("name", Table::Type::text);
	create_table(resource_type);

	Table resource("Resource");
	resource.add_primary_key("id", Table::Type::integer);
	resource.add_column("resource_type", Table::Type::text, false);
	resource.add_column("name", Table::Type::text, true);
	resource.bind_foreign_key("ResourceType", "resource_type", "name");
	create_table(resource);

	Table tag("ResourceTag");
	tag.add_primary_key("resource_id", Table::Type::integer);
	tag.add_primary_key("value", Table::Type::text);
	tag.bind_foreign_key("Resource", "resource_id", "id");
	create_table(tag);

	Table userdata("ResourceUserdata");
	userdata.add_primary_key("field", Table::Type::text);
	userdata.add_primary_key("resource_id", Table::Type::integer);
	userdata.add_column("integer", Table::Type::integer, true);
	userdata.add_column("single", Table::Type::real, true);
	userdata.add_column("string", Table::Type::text, true);
	userdata.add_column("data", Table::Type::blob, true);
	userdata.bind_foreign_key("Resource", "resource_id", "id");
	create_table(userdata);

	Table action_instance("ActionInstance");
	action_instance.add_primary_key("id", Table::Type::integer);
	action_instance.add_column("action_type", Table::Type::text, false);
	action_instance.add_column("action_name", Table::Type::text, false);
	action_instance.add_column("resource_id", Table::Type::integer, false);
	action_instance.add_column("task", Table::Type::text, true);
	action_instance.add_column("cost", Table::Type::real, true);
	action_instance.bind_foreign_key("ActionDefinition", "action_type", "type");
	action_instance.bind_foreign_key("ActionDefinition", "action_name", "name");
	action_instance.bind_foreign_key("Resource", "resource_id", "id");
	create_table(action_instance);

	Table requirement("Requirement");
	requirement.add_primary_key("id", Table::Type::integer);
	requirement.add_column("resource_id", Table::Type::integer, false);
	requirement.add_column("action_instance_id", Table::Type::integer, false);
	requirement.add_column("count", Table::Type::real, true);
	requirement.add_column("input", Table::Type::integer, false);
	requirement.add_column("output", Table::Type::integer, false);
	action_instance.bind_foreign_key("Resource", "resource_id", "id");
	action_instance.bind_foreign_key("ActionInstance", "action_instance_id", "id");
	create_table(requirement);
}

void twoflower::Brochure::create_table(Table& table)
{
	if (!table.exists(*database))
	{
		table.create(*database);
	}

	if (!table.exists(*database))
	{
		std::string message = "couldn't create table: ";
		message += table.get_name();

		throw std::runtime_error(message);
	}
}
