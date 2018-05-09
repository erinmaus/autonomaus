// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <boost/regex.hpp>
#include "mapp/atlas/atlas.hpp"

mapp::Atlas::Atlas(twoflower::Brochure& brochure) :
	brochure(&brochure)
{
	ensure_action_definition("parent", false);
	ensure_action_definition("child", false);
	ensure_action_definition("amenity", false);
	ensure_action_definition("walk", true);
	ensure_action_definition("teleport", true);
	ensure_action_definition("shortcut", true);
	ensure_action_definition("ship", true);
	ensure_action_definition("object", true);
	ensure_action_definition("npc", true);

	ensure_resource_type("location");
	ensure_resource_type("amenity");

	ensure_root_location();
}

// https://stackoverflow.com/a/217605
static inline void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

static inline std::string trim(std::string s) {
	ltrim(s);
	rtrim(s);

	return s;
}

static std::vector<std::string> split(const std::string& value, const std::string& delim)
{
	std::vector<std::string> substrings;
	std::size_t current_index = 0;
	std::size_t next_index = value.find(",");
	while (next_index != std::string::npos)
	{
		auto substring = value.substr(current_index, next_index - current_index);
		substrings.push_back(trim(substring));

		current_index = next_index + 1;
		next_index = value.find(",", current_index);
	}
	substrings.push_back(trim(value.substr(current_index, next_index - current_index)));

	return substrings;
}

bool mapp::Atlas::find(const std::string& address, Location& result) const
{
	auto matches = find(address);
	if (matches.size() != 1)
	{
		return false;
	}

	result = matches[0];
	return true;
}

mapp::Locations mapp::Atlas::find(const std::string& address) const
{
	auto components = split(address, ",");

	std::vector<twoflower::Resource> candidates;
	auto current = brochure->resources().by_name(components[0], "location");
	auto end = brochure->resources().end();
	while (current != end)
	{
		candidates.push_back(*current);
		++current;
	}

	std::vector<twoflower::Resource> matches;
	if (components.size() == 1)
	{
		matches = candidates;
	}
	else
	{
		for (auto& candidate: candidates)
		{
			bool match = true;

			Location current;
			if (!location(candidate, current))
			{
				continue;
			}

			for (std::size_t i = 1; i < components.size(); ++i)
			{
				auto p = parent(current);
				if (p.resource.get_id())
				{
					auto name = p.resource.get_name();
					if (name != components[i])
					{
						match = false;
						break;
					}
					else
					{
						current = p;
					}
				}
			}

			if (match)
			{
				matches.push_back(candidate);
			}
		}
	}

	Locations results;
	for (auto& match: matches)
	{
		Location result;
		if (location(match, result))
		{
			results.push_back(result);
		}
	}

	return results;
}

bool mapp::Atlas::fuzzy_find(const std::string& address, Location& result) const
{
	auto matches = fuzzy_find(address);
	if (matches.size() != 1)
	{
		return false;
	}

	result = matches[0];
	return true;
}

mapp::Locations mapp::Atlas::fuzzy_find(const std::string& address) const
{
	auto components = split(address, ",");

	std::vector<twoflower::Resource> candidates;
	auto current = brochure->resources().by_fuzzy_name(components[0], "location");
	auto end = brochure->resources().end();
	while (current != end)
	{
		candidates.push_back(*current);
		++current;
	}

	std::vector<twoflower::Resource> matches;
	if (components.size() == 1)
	{
		matches = candidates;
	}
	else
	{
		for (auto& candidate: candidates)
		{
			bool match = true;

			Location current;
			if (!location(candidate, current))
			{
				continue;
			}

			for (std::size_t i = 1; i < components.size(); ++i)
			{
				std::string fuzzy_name;
				fuzzy_name.resize(components[i].length() * 3 + 2);
				fuzzy_name[0] = '.';
				fuzzy_name[1] = '*';
				for (std::size_t j = 0; j < components[i].length(); ++j)
				{
					fuzzy_name[j * 2 + 0 + 2] = std::tolower(components[i].at(j));
					fuzzy_name[j * 2 + 1 + 2] = '.';
					fuzzy_name[j * 2 + 2 + 2] = '*';
				}
				boost::regex e(fuzzy_name);

				auto p = parent(current);
				if (p.resource.get_id())
				{
					auto name = p.resource.get_name();
					for (std::size_t i = 0; i < name.size(); ++i)
					{
						name[i] = std::tolower(name[i]);
					}

					if (!boost::regex_match(name, e))
					{
						match = false;
						break;
					}
					else
					{
						current = p;
					}
				}
			}

			if (match)
			{
				matches.push_back(candidate);
			}
		}
	}

	Locations results;
	for (auto& match: matches)
	{
		Location result;
		if (location(match, result))
		{
			results.push_back(result);
		}
	}

	return results;
}

bool mapp::Atlas::find(const thoreau::TileCoordinate& tile_coordinate, Location& result) const
{
	Location r;
	if (do_find_child_location_by_tile(tile_coordinate, root_location, r) &&
		r.resource.get_id() != root_location.resource.get_id())
	{
		result = r;
		return true;
	}

	return false;
}

std::string mapp::Atlas::address(const Location& location) const
{
	std::string name = location.resource.get_name();
	auto p = parent(location);
	while (p.resource.get_id())
	{
		name += ", ";
		name += p.resource.get_name();
		p = parent(p);
	}

	return name;
}

mapp::Location mapp::Atlas::add(const LocationDefinition& definition)
{
	return add(definition, root_location);
}

mapp::Location mapp::Atlas::add(const LocationDefinition& definition, const Location& parent)
{
	Location result;
	result.resource.builder().set_type("location");
	result.resource.builder().set_name(definition.name);

	result.resource = brochure->builder().add_resource(result.resource);

	result.shape = definition.shape;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-location-shape",
		definition.shape.serialize());

	if (definition.has_anchor)
	{
		brochure->builder().set_userdata(
			result.resource,
			"mapp-location-anchor-x",
			definition.anchor.x);
		brochure->builder().set_userdata(
			result.resource,
			"mapp-location-anchor-y",
			definition.anchor.y);
		brochure->builder().set_userdata(
			result.resource,
			"mapp-location-anchor-layer",
			definition.anchor.layer);
		result.has_anchor = true;
	}
	else
	{
		result.has_anchor = false;
	}

	result.min.x = result.shape.get_min().x;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-location-min-x",
		result.min.x);

	result.min.y = result.shape.get_min().y;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-location-min-y",
		result.min.y);

	result.max.x = result.shape.get_max().x;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-location-max-x",
		result.max.x);

	result.max.y = result.shape.get_max().y;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-location-max-y",
		result.max.y);

	twoflower::Action child_link;
	child_link.builder().set_type("travel");
	child_link.builder().set_name("child");
	child_link = brochure->builder().connect(child_link, parent.resource);
	brochure->builder().connect(twoflower::Requirement(), child_link, result.resource);

	twoflower::Action parent_link;
	parent_link.builder().set_type("travel");
	parent_link.builder().set_name("parent");
	parent_link = brochure->builder().connect(parent_link, result.resource);
	brochure->builder().connect(twoflower::Requirement(), parent_link, parent.resource);

	recompute_bounds(parent);

	this->cache_locations[result.resource.get_id()] = result;
	this->cache_children.erase(parent.resource.get_id());

	return result;
}

void mapp::Atlas::remove(const Location& location)
{
	if (location.resource.get_id() != root_location.resource.get_id())
	{
		auto p = parent(location);

		auto c = children(location);
		for (auto& i: c)
		{
			remove(i);
		}

		brochure->builder().remove_resource(location.resource);
		recompute_bounds(p);

		this->cache_locations.erase(location.resource.get_id());
		this->cache_parents.erase(location.resource.get_id());
		this->cache_children.erase(location.resource.get_id());
		this->cache_children.erase(p.resource.get_id());
	}
}

mapp::Location mapp::Atlas::update(const Location& location, const LocationDefinition& definition)
{
	auto resource = location.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);
	brochure->builder().set_userdata(
		resource,
		"mapp-location-shape",
		definition.shape.serialize());

	if (definition.has_anchor)
	{
		brochure->builder().set_userdata(
			resource,
			"mapp-location-anchor-x",
			definition.anchor.x);
		brochure->builder().set_userdata(
			resource,
			"mapp-location-anchor-y",
			definition.anchor.y);
		brochure->builder().set_userdata(
			resource,
			"mapp-location-anchor-layer",
			definition.anchor.layer);
	}
	else
	{
		brochure->builder().unset_userdata(
			resource,
			"mapp-location-anchor-x");
		brochure->builder().unset_userdata(
			resource,
			"mapp-location-anchor-y");
		brochure->builder().unset_userdata(
			resource,
			"mapp-location-anchor-layer");
	}

	recompute_bounds(location);
	{
		auto p = parent(location);
		recompute_bounds(p);
	}

	auto p = parent(location);
	this->cache_locations.erase(resource.get_id());
	this->cache_children.erase(p.resource.get_id());

	Location result;
	this->location(resource, result);
	return result;
}

mapp::Amenity mapp::Atlas::add(const AmenityDefinition& amenity, const Location& parent)
{
	Amenity result;
	result.resource.builder().set_type("amenity");
	result.resource.builder().set_name(amenity.name);
	result.resource = brochure->builder().add_resource(result.resource);

	twoflower::Action child_link;
	child_link.builder().set_type("travel");
	child_link.builder().set_name("amenity");
	child_link = brochure->builder().connect(child_link, parent.resource);
	brochure->builder().connect(twoflower::Requirement(), child_link, result.resource);

	twoflower::Action parent_link;
	parent_link.builder().set_type("travel");
	parent_link.builder().set_name("parent");
	parent_link = brochure->builder().connect(parent_link, result.resource);
	brochure->builder().connect(twoflower::Requirement(), parent_link, parent.resource);

	return result;
}

void mapp::Atlas::remove(const Amenity& amenity)
{
	brochure->builder().remove_resource(amenity.resource);
}

mapp::Amenity mapp::Atlas::update(const Amenity& amenity, const AmenityDefinition& definition)
{
	auto resource = amenity.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	Amenity result = amenity;
	result.resource = resource;
	return result;
}

mapp::Location mapp::Atlas::parent(const Location& location) const
{
	auto iter = this->cache_parents.find(location.resource.get_id());
	if (iter != this->cache_parents.end())
	{
		auto p = this->cache_locations.find(iter->second);
		if (p != this->cache_locations.end())
		{
			return p->second;
		}
	}

	mapp::Location result;
	if (parent(location.resource, result))
	{
		this->cache_parents[location.resource.get_id()] = result.resource.get_id();
		return result;
	}

	return Location();
}

bool mapp::Atlas::parent(
	const twoflower::Resource& resource,
	Location& result) const
{
	auto actions = brochure->actions(resource);
	auto iter = actions.by_type("travel", "parent");
	auto end = actions.end();

	while (iter != end)
	{
		auto requirements = brochure->requirements(*iter);
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "location")
			{
				if (location(requirement.get_resource(), result))
				{
					return true;
				}
			}
		}

		++iter;
	}

	return false;
}

mapp::Locations mapp::Atlas::children(const Location& location) const
{
	{
		auto iter = this->cache_children.find(location.resource.get_id());
		if (iter != this->cache_children.end())
		{
			return iter->second;
		}
	}

	auto actions = brochure->actions(location.resource);
	auto iter = actions.by_type("travel", "child");
	auto end = actions.end();

	Locations results;
	while (iter != end)
	{
		auto requirements = brochure->requirements(*iter);
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "location")
			{
				Location result;
				if (!this->location(requirement.get_resource(), result))
				{
					throw std::runtime_error("child location node invalid");
				}

				results.push_back(result);
			}
		}

		++iter;
	}

	this->cache_children[location.resource.get_id()] = results;

	return results;
}

bool mapp::Atlas::location(const twoflower::Resource& resource, Location& result) const
{
	auto iter = this->cache_locations.find(resource.get_id());
	if (iter != this->cache_locations.end())
	{
		result = Location(iter->second);
		return true;
	}

	if (!brochure->has_userdata(resource, "mapp-location-shape"))
	{
		return false;
	}

	std::vector<std::uint8_t> shape_data;
	if (!brochure->get_userdata(resource, "mapp-location-shape", shape_data))
	{
		return false;
	}

	int min_x, min_y, max_x, max_y;
	if (!brochure->get_userdata(resource, "mapp-location-min-x", min_x) ||
		!brochure->get_userdata(resource, "mapp-location-min-y", min_y) ||
		!brochure->get_userdata(resource, "mapp-location-max-x", max_x) ||
		!brochure->get_userdata(resource, "mapp-location-max-y", max_y))
	{
		return false;
	}

	int anchor_x, anchor_y, anchor_layer;
	bool has_anchor = true;
	if (brochure->get_userdata(resource, "mapp-location-anchor-x", anchor_x) &&
		brochure->get_userdata(resource, "mapp-location-anchor-y", anchor_y) &&
		brochure->get_userdata(resource, "mapp-location-anchor-layer", anchor_layer))
	{
		if (anchor_x < thoreau::TileCoordinate::MIN_COORDINATE ||
			anchor_x > thoreau::TileCoordinate::MAX_COORDINATE)
		{
			has_anchor = false;
		}
		
		if (anchor_y < thoreau::TileCoordinate::MIN_COORDINATE ||
			anchor_y > thoreau::TileCoordinate::MAX_COORDINATE)
		{
			has_anchor = false;
		}

		if (anchor_layer < thoreau::TileCoordinate::MIN_LAYER ||
			anchor_layer > thoreau::TileCoordinate::MAX_LAYER)
		{
			has_anchor = false;
		}
	}
	else
	{
		has_anchor = false;
	}

	thoreau::Shape shape;
	if (!shape.deserialize(shape_data))
	{
		return false;
	}

	result.resource = resource;
	result.shape = shape;
	result.min.x = min_x;
	result.min.y = min_y;
	result.max.x = max_x;
	result.max.y = max_y;

	result.has_anchor = has_anchor;
	if (has_anchor)
	{
		result.anchor.x = anchor_x;
		result.anchor.y = anchor_y;
		result.anchor.layer = anchor_layer;
	}
	else
	{
		result.anchor.x = min_x + (max_x - min_x) / 2;
		result.anchor.y = min_y + (max_y - min_y) / 2;
		result.anchor.layer = shape.get_layer();
	}

	this->cache_locations[resource.get_id()] = result;

	return true;
}

bool mapp::Atlas::amenity(const twoflower::Resource& resource, Amenity& result) const
{
	if (resource.get_type() == "amenity")
	{
		result.resource = resource;
		return true;
	}

	return false;
}

mapp::Amenities mapp::Atlas::amenities(const Location& location) const
{
	auto actions = brochure->actions(location.resource);
	auto current = actions.by_type("travel", "amenity");
	auto end = actions.end();

	Amenities results;
	while (current != end)
	{
		Amenity result;
		auto requirements = brochure->requirements(*current);
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "amenity" &&
				amenity(requirement.get_resource(), result))
			{
				break;
			}
		}

		if (result.resource.get_id())
		{
			results.push_back(result);
		}

		++current;
	}

	return results;
}

mapp::Link mapp::Atlas::connect(TravelType type, const Location& destination)
{
	twoflower::Action action;
	action.builder().set_type("travel");
	action.builder().set_name(get_travel_type_literal(type));
	action = brochure->builder().connect(action, destination.resource);

	Link result;
	result.action = action;
	result.type = type;

	return result;
}

mapp::Link mapp::Atlas::connect(
	TravelType type,
	const Location& from,
	const Location& to,
	float cost)
{
	twoflower::Action action;
	action.builder().set_type("travel");
	action.builder().set_name(get_travel_type_literal(type));
	if (cost != HUGE_VALF)
	{
		action.builder().set_cost_multiplier(cost);
	}
	action = brochure->builder().connect(action, from.resource);

	brochure->builder().connect(twoflower::Requirement(), action, to.resource);

	Link result;
	result.action = action;
	result.type = type;

	return result;
}

void mapp::Atlas::disconnect(
	const Location& from,
	const Location& to,
	TravelType type)
{
	auto actions = brochure->actions(from.resource);
	auto l = links(from);
	for (auto& i: l)
	{
		if (type != TravelType::none && i.type != type)
		{
			continue;
		}

		Location d;
		if (!destination(i, d))
		{
			continue;
		}

		if (d.resource.get_id() != to.resource.get_id())
		{
			continue;
		}

		remove(i);
	}
}

void mapp::Atlas::remove(const Link& link)
{
	brochure->builder().remove_action(link.action);
}

mapp::Link mapp::Atlas::link(const twoflower::Action& action) const
{
	Link result;
	result.type = get_travel_type_from_literal(action.get_name());
	result.action = action;

	auto destination_resource = brochure->resource(action);
	if (!location(destination_resource, result.destination))
	{
		throw std::runtime_error("action resource is not location");
	}

	return result;
}

bool mapp::Atlas::destination(const Link& link, Location& result) const
{
	return destination(link.action, result);
}

bool mapp::Atlas::destination(const twoflower::Action& action, Location& result) const
{
	auto requirements = brochure->requirements(action);

	for (auto requirement: requirements)
	{
		if (requirement.get_resource().get_type() == "location" &&
			location(requirement.get_resource(), result))
		{
			return true;
		}
	}

	return false;
}

mapp::Links mapp::Atlas::links(const Location& location) const
{
	auto actions = brochure->actions(location.resource);
	auto current = actions.by_type("travel");
	auto end = actions.end();

	Links results;
	while (current != end)
	{
		if (current->is_getter())
		{
			results.push_back(link(*current));
		}

		++current;
	}

	return results;
}

const mapp::Location& mapp::Atlas::root() const
{
	return root_location;
}

void mapp::Atlas::ensure_action_definition(const std::string& type, bool getter)
{
	if (!brochure->has_action_definition("travel", type))
	{
		twoflower::Action action;
		action.builder().set_type("travel");
		action.builder().set_name(type);
		action.builder().set_is_getter(getter);

		brochure->builder().add_action_definition(action);
	}
}

void mapp::Atlas::ensure_resource_type(const std::string& type)
{
	if (!brochure->has_resource_type(type))
	{
		brochure->builder().add_resource_type(type);
	}
}

void mapp::Atlas::ensure_root_location()
{
	twoflower::Resource root;
	auto iter = brochure->resources().by_name("RS", "location");
	if (iter == brochure->resources().end())
	{
		twoflower::Resource root_definition;
		root_definition.builder().set_name("RS");
		root_definition.builder().set_type("location");

		root = brochure->builder().add_resource(root_definition);

		thoreau::Shape shape;
		brochure->builder().set_userdata(root, "mapp-location-shape", shape.serialize());
		brochure->builder().set_userdata(root, "mapp-location-min-x", 0);
		brochure->builder().set_userdata(root, "mapp-location-min-y", 0);
		brochure->builder().set_userdata(root, "mapp-location-max-x", 0);
		brochure->builder().set_userdata(root, "mapp-location-max-y", 0);
	}
	else
	{
		root = *iter;
	}

	if (!location(root, root_location))
	{
		throw std::runtime_error("root location node invalid");
	}
}

bool mapp::Atlas::do_find_child_location_by_tile(
	const thoreau::TileCoordinate& tile_coordinate,
	const Location& parent,
	Location& result) const
{
	if (tile_coordinate.x < parent.min.x ||
		tile_coordinate.x > parent.max.x ||
		tile_coordinate.y < parent.min.y ||
		tile_coordinate.y > parent.max.y)
	{
		return false;
	}

	bool match = false;
	if (parent.shape.inside(tile_coordinate) &&
		parent.shape.get_num_points() != 0)
	{
		result = parent;
		match = true;
	}

	auto c = children(parent);
	for (auto& i: c)
	{
		if (do_find_child_location_by_tile(tile_coordinate, i, result))
		{
			match = true;
			break;
		}
	}

	return match;
}

void mapp::Atlas::recompute_bounds(const Location& location)
{
	if (location.resource.get_id() == 0)
	{
		return;
	}

	int min_x, min_y;
	int max_x, max_y;

	if (location.shape.get_num_points() == 0)
	{
		min_x = thoreau::TileCoordinate::MAX_COORDINATE;
		min_y = thoreau::TileCoordinate::MAX_COORDINATE;
		max_x = thoreau::TileCoordinate::MIN_COORDINATE;
		max_y = thoreau::TileCoordinate::MIN_COORDINATE;
	}
	else
	{
		min_x = location.shape.get_min().x;
		min_y = location.shape.get_min().y;
		max_x = location.shape.get_max().x;
		max_y = location.shape.get_max().y;
	}

	auto c = children(location);
	for (auto& i: c)
	{
		min_x = std::min(i.min.x, min_x);
		min_y = std::min(i.min.y, min_y);
		max_x = std::max(i.max.x, max_x);
		max_y = std::max(i.max.y, max_y);
	}

	if (min_x > max_x || min_y > max_y)
	{
		min_x = 0;
		min_y = 0;
		max_x = 0;
		max_y = 0;
	}

	brochure->builder().set_userdata(
		location.resource,
		"mapp-location-min-x",
		min_x);
	brochure->builder().set_userdata(
		location.resource,
		"mapp-location-min-y",
		min_y);
	brochure->builder().set_userdata(
		location.resource,
		"mapp-location-max-x",
		max_x);
	brochure->builder().set_userdata(
		location.resource,
		"mapp-location-max-y",
		max_y);

	recompute_bounds(parent(location));
}
