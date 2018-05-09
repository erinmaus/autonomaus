// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mapp/atlas/atlas.hpp"
#include "plato/utility.hpp"
#include "plato/gui/locationResource.hpp"
#include "plato/tools/complexShapeTool.hpp"
#include "plato/tools/toolFactory.hpp"
#include "principle/common/gui.hpp"
#include "principle/deps/imgui/imgui.h"

void plato::LocationResourceView::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	const twoflower::Resource& resource) const
{
	auto& atlas = manager.get_atlas();
	mapp::Location location;
	if (!atlas.location(resource, location))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid location.",
			resource.get_id(),
			resource.get_name().c_str());
		return;
	}

	ImGui::Text(
		"Location %s (id: %d)",
		resource.get_name().c_str(),
		resource.get_id());

	ImGui::Bullet();
	ImGui::TextWrapped("Address: %s", atlas.address(location).c_str());

	auto parent = atlas.parent(location);
	ImGui::BulletText(
		"Parent: %s (id: %d)",
		parent.resource.get_name().c_str(),
		parent.resource.get_id());

	ImGui::BulletText(
		"Anchor: (%d, %d; %d) [%s]",
		location.anchor.x,
		location.anchor.y,
		location.anchor.layer,
		location.has_anchor ? "set" : "unset");

	ImGui::BulletText(
		"Bounds: (%d, %d) -> (%d, %d)",
		location.min.x, location.max.x,
		location.max.x, location.max.y);

	if (location.shape.get_num_points() != 0)
	{
		ImGui::BulletText("Shape: %lu points", location.shape.get_num_points());
	}

	if (ImGui::CollapsingHeader("Children"))
	{
		ImGui::BeginGroup();

		auto children = atlas.children(location);
		if (children.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& child: children)
			{
				ImGui::Bullet();
				ImGui::TextWrapped(
					"%s (%d)",
					child.resource.get_name().c_str(),
					child.resource.get_id());
			}
		}

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Features"))
	{
		ImGui::BeginGroup();

		auto amenities = atlas.amenities(location);
		if (amenities.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& amenity: amenities)
			{
				ImGui::Bullet();
				ImGui::TextWrapped(
					"%s (%d)",
					amenity.resource.get_name().c_str(),
					amenity.resource.get_id());
			}
		}

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Resource"))
	{
		manager.view(resource, true);
	}
}

void plato::LocationResourceEditor::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	twoflower::Resource& resource) const
{
	auto& atlas = manager.get_atlas();
	mapp::Location location;
	if (!atlas.location(resource, location))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid location.",
			resource.get_id(),
			resource.get_name().c_str());
		return;
	}

	ImGui::PushItemWidth(256);
	{
		auto name = resource.get_name();
		ImGui::Text("Name");
		ImGui::SameLine();
		if (ImGui::InputString("##name", name))
		{
			mapp::LocationDefinition definition = location;
			definition.name = name;
			atlas.update(location, definition);
			resource = location.resource;
		}
	}
	ImGui::PopItemWidth();

	if (ImGui::Button("Edit Shape"))
	{
		auto tool = new ComplexShapeTool(
			manager.get_toolbox(),
			[this, location, &manager](auto shape)
			{
				mapp::LocationDefinition definition = location;
				definition.shape = shape;

				auto& atlas = manager.get_atlas();
				atlas.update(location, definition);
			});
		manager.get_toolbox().use(tool);
	}

	if (ImGui::Checkbox("Set Anchor", &location.has_anchor))
	{
		mapp::LocationDefinition definition = location;
		atlas.update(location, definition);
	}

	if (location.has_anchor)
	{
		ImGui::Text("Anchor:");
		ImGui::PushItemWidth(256);
		if (ImGui::InputInt3("##anchor", &location.anchor.x))
		{
			if (location.anchor.x < thoreau::TileCoordinate::MIN_COORDINATE ||
				location.anchor.x > thoreau::TileCoordinate::MAX_COORDINATE)
			{
				location.anchor.x = location.min.x + (location.max.x - location.min.x) / 2;
			}

			if (location.anchor.y < thoreau::TileCoordinate::MIN_COORDINATE ||
				location.anchor.y > thoreau::TileCoordinate::MAX_COORDINATE)
			{
				location.anchor.y = location.min.y + (location.max.y - location.min.y) / 2;
			}

			if (location.anchor.layer < thoreau::TileCoordinate::MIN_COORDINATE ||
				location.anchor.layer > thoreau::TileCoordinate::MAX_COORDINATE)
			{
				location.anchor.layer = location.shape.get_layer();
			}

			mapp::LocationDefinition definition = location;
			atlas.update(location, definition);
		}
		ImGui::PopItemWidth();
	}

	if (ImGui::CollapsingHeader("Children"))
	{
		ImGui::BeginGroup();

		if (ImGui::Button("Add"))
		{
			manager.get_toolbox().get_factory().add_new_child_location(location);
		}

		auto children = atlas.children(location);
		if (children.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& child: children)
			{
				ImGui::PushID(child.resource.get_id());

				ImGui::Bullet();
				if (ImGui::Button("-"))
				{
					ImGui::OpenPopup("confirm_remove_child");
					ImGui::GetStateStorage()->SetInt(
						ImGui::GetID("pending_child"), child.resource.get_id());
				}

				ImGui::SameLine();
				ImGui::TextWrapped(
					"%s (id: %d)",
					child.resource.get_name().c_str(),
					child.resource.get_id());

				int child_id = ImGui::GetStateStorage()->GetInt(
					ImGui::GetID("pending_child"), 0);
				if (ImGui::BeginPopup("confirm_remove_child"))
				{
					mapp::Location child;
					if (!brochure.resources().has(child_id) ||
						!atlas.location(brochure.resources().get(child_id), child))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::TextWrapped(
						"Are you sure you wish to remove '%s' (id: %d)?",
						atlas.address(child).c_str(),
						child_id);
					ImGui::TextColored(ImVec4(1, 0, 0, 1),
						"All children nodes will be erased,"
						" and any actions that depend on them!");

					if (ImGui::Button("Yes"))
					{
						atlas.remove(child);
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();
					if (ImGui::Button("No"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
		}

		if (ImGui::Button("Connect Children"))
		{
			connect_walkable_children(
				brochure,
				atlas,
				location,
				manager.get_toolbox().get_map());
		}

		ImGui::SameLine();
		if (ImGui::Button("Connect Children (Recursive)"))
		{
			connect_walkable_children(
				brochure,
				atlas,
				location,
				manager.get_toolbox().get_map(),
				true);
		}

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Features"))
	{
		ImGui::BeginGroup();

		if (ImGui::Button("Add"))
		{
			mapp::AmenityDefinition definition;
			definition.name = "(blank)";

			atlas.add(definition, location);
		}

		auto amenities = atlas.amenities(location);
		if (amenities.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& amenity: amenities)
			{
				ImGui::PushID(amenity.resource.get_id());

				ImGui::Bullet();
				if (ImGui::Button("-"))
				{
					ImGui::OpenPopup("confirm_remove_amenity");
					ImGui::GetStateStorage()->SetInt(
						ImGui::GetID("pending_amenity"), amenity.resource.get_id());
				}

				ImGui::SameLine();

				std::string name = amenity.resource.get_name();
				if (ImGui::InputString("##name", name))
				{
					mapp::AmenityDefinition definition = amenity;
					definition.name = name;
					atlas.update(amenity, definition);
				}

				ImGui::SameLine();
				ImGui::TextWrapped(
					"(id: %d)",
					amenity.resource.get_id());

				int amenity_id = ImGui::GetStateStorage()->GetInt(
					ImGui::GetID("pending_amenity"), 0);
				if (ImGui::BeginPopup("confirm_remove_amenity"))
				{
					mapp::Amenity amenity;
					if (!brochure.resources().has(amenity_id) ||
						!atlas.amenity(brochure.resources().get(amenity_id), amenity))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::TextWrapped(
						"Are you sure you wish to remove feature '%s' (id: %d)?",
						amenity.resource.get_name().c_str(),
						amenity_id);

					if (ImGui::Button("Yes"))
					{
						atlas.remove(amenity);
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();
					if (ImGui::Button("No"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
		}

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Resource"))
	{
		manager.edit(resource, true);
	}
}
