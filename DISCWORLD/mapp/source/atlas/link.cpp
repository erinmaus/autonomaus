// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "mapp/atlas/link.hpp"

std::string mapp::get_travel_type_literal(TravelType type)
{
	switch (type)
	{
		case TravelType::walk:
			return "walk";
		case TravelType::teleport:
			return "teleport";
		case TravelType::shortcut:
			return "shortcut";
		case TravelType::ship:
			return "ship";
		case TravelType::object:
			return "object";
		case TravelType::npc:
			return "npc";
		default:
			// Don't provide garbage inputs, doofus!
			throw std::runtime_error("invalid travel type enumeration value");
	}
}

mapp::TravelType mapp::get_travel_type_from_literal(const std::string& literal)
{
	if (literal == "walk")
	{
		return TravelType::walk;
	}
	if (literal == "teleport")
	{
		return TravelType::teleport;
	}
	if (literal == "shortcut")
	{
		return TravelType::shortcut;
	}
	if (literal == "ship")
	{
		return TravelType::ship;
	}
	if (literal == "object")
	{
		return TravelType::object;
	}
	if (literal == "npc")
	{
		return TravelType::npc;
	}
	else
	{
		throw std::runtime_error("invalid travel type enumeration value");
	}
}
