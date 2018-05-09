// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "mapp/game/game.hpp"
#include "plato/gui/shopResource.hpp"
#include "plato/tools/toolFactory.hpp"
#include "principle/common/gui.hpp"
#include "principle/deps/imgui/imgui.h"

void plato::ShopResourceView::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	const twoflower::Resource& resource) const
{
	auto& game = manager.get_game();
	auto& atlas = manager.get_atlas();
	mapp::Shop shop;
	if (!game.shop(resource, shop))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid shop.",
			resource.get_id(),
			resource.get_name().c_str());
		return;
	}

	ImGui::Text(
		"Shop %s (id: %d)",
		resource.get_name().c_str(),
		resource.get_id());

	if (shop.location.resource.get_id())
	{
		ImGui::Text(
			"Location: %s (id: %d)",
			atlas.address(shop.location).c_str(),
			shop.location.resource.get_id());
	}
	else
	{
		ImGui::Text("Location: (none)");
	}

	if (ImGui::CollapsingHeader("Inventory"))
	{
		auto inventory = game.inventory(shop);
		if (inventory.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto& item: inventory)
			{
				ImGui::Bullet();
				ImGui::TextWrapped(
					"%s (id: %d)",
					item.resource.get_name().c_str(),
					item.resource.get_id());

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

void plato::ShopResourceEditor::update(
	BrochureWidgetManager& manager,
	twoflower::Brochure& brochure,
	twoflower::Resource& resource) const
{
	auto& game = manager.get_game();
	auto& atlas = manager.get_atlas();
	mapp::Shop shop;
	if (!game.shop(resource, shop))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Resource %d ('%s') not valid shop.",
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

	if (shop.location.resource.get_id())
	{
		ImGui::TextWrapped(
			"Location: %s (id: %d)",
			atlas.address(shop.location).c_str(),
			shop.location.resource.get_id());
	}
	else
	{
		ImGui::Text("Location: (none)");
	}

	if (ImGui::Button("Edit Location"))
	{
		manager.get_toolbox().get_factory().find_location(
			[shop, &manager, &brochure](auto parent)
			{
				mapp::ShopDefinition definition = shop;
				definition.location = parent;

				mapp::Game game(brochure);
				game.update(shop, definition);
			});
	}

	if (ImGui::CollapsingHeader("Inventory"))
	{
		ImGui::BeginGroup();

		if (ImGui::Button("Add"))
		{
			manager.get_toolbox().get_factory().find_resource(
				"item",
				[shop, &brochure](auto resource)
				{
					mapp::Item item;
					mapp::Game game(brochure);
					if (game.item(resource, item))
					{
						game.add(shop, item, 1);
					}
				});
		}

		auto inventory = game.inventory(shop);
		if (inventory.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			bool add_price = false;
			bool remove_item = false;
			int item_id = 0;

			for (auto& item: inventory)
			{
				ImGui::PushID(item.resource.get_id());

				ImGui::Bullet();
				if (ImGui::Button("-"))
				{
					remove_item = true;
					item_id = item.resource.get_id();
				}

				ImGui::SameLine();
				ImGui::TextWrapped(
					"%s (id: %d)",
					item.resource.get_name().c_str(),
					item.resource.get_id());

				ImGui::Indent();
				ImGui::PushItemWidth(128);
				{
					int quantity = game.quantity(shop, item);
					ImGui::Text("Quantity");

					ImGui::SameLine();
					if (ImGui::InputInt("##quantity", &quantity) &&
						quantity > 0)
					{
						game.add(shop, item, quantity);
					}

					auto price = game.price(shop, item);
					if (price.empty())
					{
						int coin_price = ImGui::GetStateStorage()->GetInt(
							ImGui::GetID("coin_price"), 0);
						ImGui::Text("Coins");

						ImGui::SameLine();
						if (ImGui::InputInt("###price", &coin_price) && coin_price > 0)
						{
							auto coins = game.currency("Coins");
							if (coins.resource.get_id())
							{
								game.add(shop, item, coins, coin_price);
							}
							else
							{
								printf("no coins :(\n");
							}
						}
					}
					else
					{
						for (auto& p: price)
						{
							ImGui::PushID(p.second.resource.get_id());

							ImGui::Text("%s", p.second.resource.get_name().c_str());

							ImGui::SameLine();
							std::string id = "###";
							id += p.second.resource.get_name();

							int currency_price = p.first;
							if (ImGui::InputInt(id.c_str(), &currency_price) && currency_price > 0)
							{
								game.add(shop, item, p.second, currency_price);
							}

							ImGui::SameLine();
							if (ImGui::Button("Remove"))
							{
								game.remove(shop, item, p.second);
							}

							ImGui::PopID();
						}
					}

					if (ImGui::Button("+ Currency"))
					{
						add_price = true;
						item_id = item.resource.get_id();
					}
				}
				ImGui::PopItemWidth();
				ImGui::Unindent();

				ImGui::PopID();
			}

			if (add_price)
			{
				ImGui::OpenPopup("add_price");
				ImGui::SetNextWindowSize(ImVec2(240, 0));
				ImGui::GetStateStorage()->SetInt(
					ImGui::GetID("pending_item"), item_id);
			}

			if (remove_item)
			{
				ImGui::OpenPopup("confirm_remove_shop");
				ImGui::SetNextWindowSize(ImVec2(240, 0));
				ImGui::GetStateStorage()->SetInt(
					ImGui::GetID("pending_item"), item_id);
			}

			item_id = ImGui::GetStateStorage()->GetInt(
				ImGui::GetID("pending_item"), 0);
			if (ImGui::BeginPopup("add_price"))
			{
				int selected_index = ImGui::GetStateStorage()->GetInt(
					ImGui::GetID("currency_index"), 0);
				ImGui::ListBoxHeader("##Currency");

				auto resources = brochure.resources();
				twoflower::Resource selected_currency;
				int index = 0;
				for (auto i = resources.by_type("currency"); i != resources.end(); ++i)
				{
					if (ImGui::Selectable(i->get_name().c_str(), selected_index == index))
					{
						selected_index = index;
					}

					if (selected_index == index)
					{
						selected_currency = *i;
					}

					++index;
				}
				ImGui::ListBoxFooter();
				ImGui::GetStateStorage()->SetInt(ImGui::GetID("currency_index"), selected_index);

				int price = ImGui::GetStateStorage()->GetInt(
					ImGui::GetID("shop_price"), 1);

				ImGui::PushItemWidth(128);
				{
					ImGui::Text("Price");
					if (ImGui::InputInt("##price", &price) && price > 0)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("shop_price"), price);
					}
				}
				ImGui::PopItemWidth();

				if (ImGui::Button("Add"))
				{
					mapp::Currency currency;
					mapp::Item item;

					if (brochure.resources().has(item_id) &&
						game.item(brochure.resources().get(item_id), item) &&
						game.currency(selected_currency, currency))
					{
						game.add(shop, item, currency, price);
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

			if (ImGui::BeginPopup("confirm_remove_shop"))
			{
				ImGui::Dummy(ImVec2(240, 1)); // XXX: Popup is too thin.
				ImGui::SetWindowSize(ImVec2(240, 0));

				mapp::Item item;
				if (!brochure.resources().has(item_id) ||
					!game.item(brochure.resources().get(item_id), item))
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

		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Resource"))
	{
		manager.edit(resource, true);
	}
}
