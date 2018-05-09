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

twoflower::Brochure::Requirements::Requirements(
	const Brochure& brochure, const Action& action) :
	brochure(&brochure),
	action(action)
{
	// Nothing.
}

twoflower::Brochure::Requirements::const_iterator
twoflower::Brochure::Requirements::begin() const
{
	auto statement = brochure->database->create_statement(
		"SELECT * FROM Requirement WHERE action_instance_id=?;");
	statement.bind(1, action.get_id());

	return const_iterator(*brochure, statement);
}

twoflower::Brochure::Requirements::const_iterator
twoflower::Brochure::Requirements::end() const
{
	return const_iterator();
}

twoflower::Brochure::Requirements::const_iterator::const_iterator(
	const Brochure& brochure, Statement& statement) :
	brochure(&brochure),
	statement(new Statement(statement)),
	end(false)
{
	next();
}

twoflower::Brochure::Requirements::const_iterator::~const_iterator()
{
	// Nothing.
}

twoflower::Brochure::Requirements::const_iterator&
twoflower::Brochure::Requirements::const_iterator::operator ++()
{
	next();
	return *this;
}

twoflower::Brochure::Requirements::const_iterator
twoflower::Brochure::Requirements::const_iterator::operator ++(int)
{
	next();
	return *this;
}

bool twoflower::Brochure::Requirements::const_iterator::operator ==(
	const const_iterator& other) const
{
	return statement.get() == other.statement.get() || (end && other.end);
}

bool twoflower::Brochure::Requirements::const_iterator::operator !=(
	const const_iterator& other) const
{
	return !(*this == other);
}

twoflower::Brochure::Requirements::const_iterator::pointer
twoflower::Brochure::Requirements::const_iterator::operator ->() const
{
	return &value;
}

twoflower::Brochure::Requirements::const_iterator::value_type
twoflower::Brochure::Requirements::const_iterator::operator *() const
{
	return value;
}

void twoflower::Brochure::Requirements::const_iterator::next()
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
		Requirement::Builder builder(value);

		int id;
		statement->get("id", id);
		builder.set_id(id);

		int resource_id;
		statement->get("resource_id", resource_id);
		builder.set_resource(brochure->resources().get(resource_id));

		int action_instance_id;
		statement->get("action_instance_id", action_instance_id);
		builder.set_action(brochure->actions().get(action_instance_id));

		float count;
		statement->get("count", count);
		builder.set_count(count);

		int input;
		statement->get("input", input);
		builder.set_is_input(input);

		int output;
		statement->get("output", output);
		builder.set_is_output(output);
	}
}
