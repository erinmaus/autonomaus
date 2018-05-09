// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/chomsky.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/itemIconManager.hpp"
#include "chomsky/gui/parser/field.hpp"
#include "chomsky/gui/parser/group.hpp"
#include "chomsky/gui/parser/matchText.hpp"
#include "chomsky/resources/modelVertexCloud.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/model/model.hpp"
#include "kvre/ui/flatDrawBuffer.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"

chomsky::ItemIconManager::ItemIconManager(
	const kompvter::TextureManager& texture_manager,
	TextureKeyManager& texture_key_manager,
	TextureDictionary& texture_dictionary) :
		gui_parser(texture_manager, texture_dictionary)
{
	this->texture_key_manager = &texture_key_manager;
	this->texture_manager = &texture_manager;
	this->texture_dictionary = &texture_dictionary;

	auto quantity_text_node = new parser::MatchText();
	quantity_text_node->set_wildcard();
	quantity_text_node->set_shadowed(true);
	quantity_text_node->set_include_spaces(false);

	auto quantity_field_node = new parser::Field();
	quantity_field_node->set_name("quantity");
	quantity_field_node->add(quantity_text_node);

	auto root_node = new parser::Group();
	root_node->add(quantity_field_node);

	this->gui_parser.add_node(quantity_text_node);
	this->gui_parser.add_node(quantity_field_node);
	this->gui_parser.add_node(root_node);
	this->gui_parser.set_root_node(root_node);
}

void chomsky::ItemIconManager::add_special(
	const std::string& entry,
	const ItemIconSilhouette& silhouette)
{
	this->specials[entry] = silhouette;
}

void chomsky::ItemIconManager::remove_special(const std::string& entry)
{
	this->specials.erase(entry);
}

bool chomsky::ItemIconManager::has_special(const std::string& entry) const
{
	return this->specials.count(entry) != 0;
}

bool chomsky::ItemIconManager::has_item(const std::string& key)
{
	return this->item_icons_by_key.count(key) != 0;
}

const chomsky::ItemIconSilhouette& chomsky::ItemIconManager::get_item(
	const std::string& key) const
{
	auto iter = this->item_icons_by_key.find(key);
	assert(iter != this->item_icons_by_key.end());

	return iter->second->silhouette;
}

void chomsky::ItemIconManager::reset()
{
	this->current_draws.clear();
	this->current_specials.clear();
	this->current_silhouettes.clear();
	this->current_quantity.clear();
	this->item_icons.clear();
	this->item_icons_by_key.clear();

	this->current_item_id = 0;
}

void chomsky::ItemIconManager::on_draw_gui(const kompvter::GUIDrawEvent& event)
{
	kvre::FlatDrawBuffer flat_draw_buffer(event);
	GUIElementStream element_stream(
		*this->texture_manager, *this->texture_key_manager);
	element_stream.update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);

	GUIContent result;
	if (this->gui_parser.parse(element_stream, result) && result.has("quantity"))
	{
		auto quantity = result.get("quantity").get_text().to_string();
		this->current_quantity[event.get_color_attachment_name()] = quantity;
	}
}

void chomsky::ItemIconManager::on_draw_model(
	const kompvter::ModelDrawEvent& event)
{
	auto& cloud = this->current_draws[event.get_color_attachment_name()];
	auto& match = this->current_matches[event.get_color_attachment_name()];

	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	kvre::Model model(event.get_vertex_attributes());
	for (auto j = begin; j != end; ++j)
	{
		auto position = model.get_position(*j);
		auto color = glm::vec3(model.get_color(*j));

		ModelVertexCloud::Vertex vertex;
		vertex.color = color;

		glm::vec4 transformed_position(position, 0.0f);
		transformed_position = event.get_view_matrix() * transformed_position;
		vertex.position = glm::vec3(transformed_position);

		cloud.add(vertex);

		auto tile_coordinate = thoreau::tile_from_world(position);
		auto tile_relative_position = position;
		tile_relative_position.x -= tile_coordinate.x * thoreau::TILE_SIZE;
		tile_relative_position.z -= tile_coordinate.y * thoreau::TILE_SIZE;

		if (tile_relative_position.x < 0.0f)
		{
			tile_relative_position.x = thoreau::TILE_SIZE + tile_relative_position.x;
		}

		if (tile_relative_position.z < 0.0f)
		{
			tile_relative_position.z = thoreau::TILE_SIZE + tile_relative_position.z;
		}

		auto& tile_point_cloud = match.add(tile_coordinate);
		tile_point_cloud.add(tile_relative_position, color);
	}
}

void chomsky::ItemIconManager::on_transfer(const kompvter::ItemIconEvent& event)
{
	flatten(event.get_source_texture_name());
	transfer(event.get_source_texture_name(), event.get_destination_texture_name());
}

void chomsky::ItemIconManager::on_clear(const kompvter::ItemIconEvent& event)
{
	auto name = event.get_texture_name();
	this->current_draws.erase(name);
	this->current_specials.erase(name);
	this->current_silhouettes.erase(name);
	this->current_quantity.erase(name);
	this->current_matches.erase(name);
}

void chomsky::ItemIconManager::on_texture_update(const kompvter::TextureEvent& event)
{
	invalidate(
		event.get_name(),
		event.get_x(), event.get_y(),
		event.get_width(), event.get_height());
}

void chomsky::ItemIconManager::transfer(int source_name, int destination_name)
{
	std::vector<std::pair<int, std::string>> specials(
		this->current_specials.lower_bound(source_name),
		this->current_specials.upper_bound(source_name));
	for (auto& i: specials)
	{
		this->current_specials.emplace(destination_name, i.second);
	}

	auto source_silhouette = this->current_silhouettes.find(source_name);
	if (source_silhouette == this->current_silhouettes.end())
	{
		return;
	}

	auto source_match = this->current_matches.find(source_name);
	if (source_match == this->current_matches.end())
	{
		return;
	}

	bool is_special = false;
	for (auto& special: this->specials)
	{
		if (ItemIconSilhouette::match(special.second, source_silhouette->second))
		{
			this->current_specials.emplace(destination_name, special.first);
			is_special = true;
		}
	}

	if (!is_special)
	{
		auto& destination_silhouette = this->current_silhouettes[destination_name];
		destination_silhouette.merge(source_silhouette->second);

		auto& destination_match = this->current_matches[destination_name];
		for (auto& i: source_match->second)
		{
			auto& j = destination_match.add(i.first);
			j.merge(i.second);
		}

		auto source_quantity = this->current_quantity.find(source_name);
		if (source_quantity != this->current_quantity.end())
		{
			this->current_quantity[destination_name] = source_quantity->second;
		}
	}
}

void chomsky::ItemIconManager::on_texture_copy(const kompvter::TextureEvent& event)
{
	if (event.get_width() != config::ITEM_ICON_WIDTH ||
		event.get_height() != config::ITEM_ICON_HEIGHT)
	{
		return;
	}

	flatten(event.get_source_name());
	if (event.get_destination_x() == event.get_source_x() &&
		event.get_destination_y() == event.get_source_y())
	{
		auto source_silhouette = this->current_silhouettes.find(event.get_source_name());
		if (source_silhouette != this->current_silhouettes.end())
		{
			this->current_silhouettes[event.get_destination_name()] = source_silhouette->second;
		}

		auto source_match = this->current_matches.find(event.get_source_name());
		if (source_match != this->current_matches.end())
		{
			this->current_matches[event.get_destination_name()] = source_match->second;
		}

		auto source_quantity = this->current_quantity.find(event.get_source_name());
		if (source_quantity != this->current_quantity.end())
		{
			this->current_quantity[event.get_destination_name()] = source_quantity->second;
		}

		auto specials = this->current_specials.equal_range(event.get_source_name());
		for (auto i = specials.first; i != specials.second; ++i)
		{
			this->current_specials.emplace(event.get_destination_name(), i->second);
		}
	}
	else
	{
		auto source_silhouette = this->current_silhouettes.find(event.get_source_name());
		if (source_silhouette == this->current_silhouettes.end())
		{
			return;
		}

		invalidate(
			event.get_destination_name(),
			event.get_destination_x(), event.get_destination_y(),
			event.get_width(), event.get_height());

		std::string key = source_silhouette->second.to_string();
		std::string runtime_key = key + "$" + std::to_string(++this->current_item_id);

		this->texture_key_manager->add(
			event.get_destination_name(),
			event.get_destination_x(), event.get_destination_y(),
			event.get_width(), event.get_height(),
			runtime_key);

		auto& definition = this->texture_dictionary->add(runtime_key);
		if (this->texture_dictionary->has(key))
		{
			auto& source_definition = this->texture_dictionary->get(key);
			for (auto j = source_definition.tags_begin(); j != source_definition.tags_end(); ++j)
			{
				definition.add_tag(*j);
			}

			for (auto j = source_definition.data_begin(); j != source_definition.data_end(); ++j)
			{
				definition.add_data(j->first, j->second);
			}
		}

		definition.set_value("item");
		definition.add_tag("runtime");
		definition.add_data("item-key", key);
		auto specials = this->current_specials.equal_range(event.get_source_name());
		for (auto i = specials.first; i != specials.second; ++i)
		{
			if (!this->texture_dictionary->has(i->second))
			{
				continue;
			}

			auto& special_definition = this->texture_dictionary->get(i->second);
			for (auto j = special_definition.tags_begin(); j != special_definition.tags_end(); ++j)
			{
				definition.add_tag(*j);
			}

			for (auto j = special_definition.data_begin(); j != special_definition.data_end(); ++j)
			{
				definition.add_data(j->first, j->second);
			}
		}

		auto quantity = this->current_quantity.find(event.get_source_name());
		if (quantity != this->current_quantity.end())
		{
			definition.add_data("item-quantity", quantity->second);
		}
		else
		{
			definition.add_data("item-quantity", "1");
		}

		ItemIcon icon;
		icon.silhouette = source_silhouette->second;
		icon.key = runtime_key;
		icon.x = event.get_destination_x();
		icon.y = event.get_destination_y();
		icon.width = event.get_width();
		icon.height = event.get_height();

		auto source_match = this->current_matches.find(event.get_source_name());
		if (source_match != this->current_matches.end())
		{
			std::size_t n = 0;
			for (auto& i: source_match->second)
			{
				n += i.second.count();
				icon.silhouette.point_cloud_match.set(i.first, i.second);
			}
		}

		auto v = this->item_icons.emplace(event.get_destination_name(), icon);
		this->item_icons_by_key[runtime_key] = &v->second;
	}
}

void chomsky::ItemIconManager::invalidate(
	int texture_name, int x, int y, int width, int height)
{
	auto current = this->item_icons.equal_range(texture_name);
	for (auto i = current.first; i != current.second; /* Nothing. */)
	{
		if (x + width <= i->second.x || i->second.x + i->second.width <= x)
		{
			++i;
			continue;
		}

		if (y + height <= i->second.y || i->second.y + i->second.height <= y)
		{
			++i;
			continue;
		}

		this->texture_dictionary->remove(i->second.key);
		this->item_icons_by_key.erase(i->second.key);
		i = this->item_icons.erase(i);
	}
}

void chomsky::ItemIconManager::flatten(int texture_name)
{
	auto draw = this->current_draws.find(texture_name);
	if (draw == this->current_draws.end())
	{
		return;
	}

	auto silhouette = this->current_silhouettes.find(texture_name);
	if (silhouette != this->current_silhouettes.end())
	{
		return;
	}

	this->current_silhouettes[texture_name].project(draw->second);
}
