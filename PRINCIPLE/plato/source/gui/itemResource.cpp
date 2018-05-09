// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "mapp/game/game.hpp"
#include "plato/gui/itemResource.hpp"
#include "principle/common/gui.hpp"
#include "principle/deps/imgui/imgui.h"

void plato::ItemResourceView::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	const twoflower::Resource& resource) const
{
	auto& game = manager.get_game();
	mapp::Item item;
	if (!game.item(resource, item))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid item.",
			resource.get_id(),
			resource.get_name().c_str());
		return;
	}

	ImGui::Text(
		"Item %s (id: %d)",
		resource.get_name().c_str(),
		resource.get_id());

	if (item.object_id != mapp::Item::NULL_OBJECT_ID)
	{
		ImGui::Text("Object ID: %d", item.object_id);
	}
	else
	{
		ImGui::Text("Object ID: (none)");
	}

	ImGui::Text("Value: %d", item.value);

	if (ImGui::CollapsingHeader("Shops"))
	{
		auto shops = game.shops(item);
		if (shops.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& shop: shops)
			{
				ImGui::Bullet();
				ImGui::TextWrapped(
					"%s (id: %d)",
					shop.resource.get_name().c_str(),
					shop.resource.get_id());

				ImGui::Indent();
				{
					ImGui::Text("Quantity: %d", game.quantity(shop, item));

					auto price = game.price(shop, item);
					if (price.size() == 1)
					{
						ImGui::Text(
							"Price: %d %s",
							price[0].first,
							price[0].second.resource.get_name().c_str());
					}
					else if (price.size() > 1)
					{
						ImGui::Text("Price:");
						for (auto& currency: price)
						{
							ImGui::BulletText(
									"%d %s",
									currency.first,
									currency.second.resource.get_name().c_str());
						}
					}
				}
				ImGui::Unindent();
			}
		}
	}

	if (ImGui::CollapsingHeader("Resource"))
	{
		manager.view(resource, true);
	}
}

void plato::ItemResourceEditor::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	twoflower::Resource& resource) const
{
	auto& game = manager.get_game();
	mapp::Item item;
	if (!game.item(resource, item))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid item.",
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
			resource.builder().set_name(name);
			brochure.builder().update_resource(resource);
		}
	}
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(256);
	{
		ImGui::Text("Object ID");
		if (ImGui::InputInt("##object_id", &item.object_id))
		{
			mapp::ItemDefinition definition = item;
			item = game.update(item, definition);
		}
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Unset"))
	{
		mapp::ItemDefinition definition = item;
		definition.object_id = mapp::Item::NULL_OBJECT_ID;
		item = game.update(item, definition);
	}

	if (ImGui::CollapsingHeader("Shops"))
	{
		ImGui::BeginGroup();

		if (ImGui::Button("Add"))
		{
			ImGui::OpenPopup("add_shop");
		}

		auto shops = game.shops(item);
		if (shops.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			bool remove_item = false;
			int shop_id = 0;
			for (auto& shop: shops)
			{
				ImGui::PushID(shop.resource.get_id());

				ImGui::Bullet();
				if (ImGui::Button("-"))
				{
					remove_item = true;
					shop_id = shop.resource.get_id();
				}

				ImGui::SameLine();
				ImGui::TextWrapped(
					"%s (id: %d)",
					shop.resource.get_name().c_str(),
					shop.resource.get_id());

				ImGui::PopID();
			}

			if (remove_item)
			{
				ImGui::OpenPopup("confirm_remove_shop");
				ImGui::SetNextWindowSize(ImVec2(240, 0));
				ImGui::GetStateStorage()->SetInt(
					ImGui::GetID("pending_shop"), shop_id);
			}

			shop_id = ImGui::GetStateStorage()->GetInt(
				ImGui::GetID("pending_shop"), 0);
			if (ImGui::BeginPopup("confirm_remove_shop"))
			{
				ImGui::Dummy(ImVec2(240, 1)); // XXX: Popup is too thin.

				mapp::Shop shop;
				if (!brochure.resources().has(shop_id) ||
					!game.shop(brochure.resources().get(shop_id), shop))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::TextWrapped(
					"Are you sure you wish to remove '%s' (id: %d) from"
					" the shop %s (id: %d)?",
					item.resource.get_name().c_str(),
					item.resource.get_id(),
					shop.resource.get_name().c_str(),
					shop.resource.get_id());

				if (ImGui::Button("Yes"))
				{
					game.remove(shop, item);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("No"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		if (ImGui::BeginPopup("add_shop"))
		{
			ImGui::ListBoxHeader("##Shop");
			auto resources = brochure.resources();

			int selected_index = ImGui::GetStateStorage()->GetInt(
				ImGui::GetID("shop_index"), 0);
			twoflower::Resource selected_shop;
			int index = 0;
			for (auto i = resources.by_type("shop"); i != resources.end(); ++i)
			{
				if (ImGui::Selectable(i->get_name().c_str(), selected_index == index))
				{
					selected_index = index;
				}

				if (selected_index == index)
				{
					selected_shop = *i;
				}

				++index;
			}
			ImGui::ListBoxFooter();
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("shop_index"), selected_index);

			int quantity = ImGui::GetStateStorage()->GetInt(
				ImGui::GetID("shop_quantity"), 1);

			ImGui::PushItemWidth(128);
			{
				ImGui::Text("Quantity");
				if (ImGui::InputInt("##quantity", &quantity) && quantity > 0)
				{
					ImGui::GetStateStorage()->SetInt(ImGui::GetID("shop_quantity"), quantity);
				}
			}
			ImGui::PopItemWidth();

			if (ImGui::Button("Add"))
			{
				mapp::Shop shop;
				if (game.shop(selected_shop, shop))
				{
					game.add(shop, item, quantity);
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Resource"))
	{
		manager.edit(resource, true);
	}
}
