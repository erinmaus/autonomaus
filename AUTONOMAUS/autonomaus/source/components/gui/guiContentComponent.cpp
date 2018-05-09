// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/components/gui/guiContentComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/state/guiProvider.hpp"

autonomaus::GUIContentComponent::GUIContentComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::GUIContentComponent::show()
{
	this->visible = true;
}

void autonomaus::GUIContentComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

void autonomaus::GUIContentComponent::reload()
{
	this->is_empty = true;
}

std::string autonomaus::GUIContentComponent::get_name() const
{
	return "GUI Content";
}

void autonomaus::GUIContentComponent::draw()
{
	ImGui::Begin("GUI Content", &this->visible);
	{
		if (ImGui::Button("Fetch"))
		{
			this->gui = *get_autonomaus().get_gui_state();
			this->is_empty = false;
		}

		if (this->is_empty)
		{
			ImGui::Text("No state fetched.");
		}
		else
		{
			for (auto& i: this->gui)
			{
				draw_content(i.second);
			}
		}
	}
	ImGui::End();
}

void autonomaus::GUIContentComponent::draw_content(
	const chomsky::GUIContent& content)
{
	auto current = content.begin();
	int current_index = 0;
	bool is_array = content.count() > 1;

	while (current != content.end())
	{
		auto next = current;
		auto next_index = current_index + 1;
		++next;
		if (next != content.end())
		{
			if (next->first != current->first)
			{
				is_array = false;
				next_index = 0;
			}
		}

		auto& child = *current->second;
		ImGui::PushID(&child);

		bool show;
		if (is_array)
		{
			show = ImGui::TreeNode(
				&child, "%s[%d] (%s)",
				child.get_name().c_str(), current_index,
				child.is_widget() ? child.get_widget_class().c_str() : "property");
		}
		else
		{
			show = ImGui::TreeNode(
				&child, "%s (%s)",
				child.get_name().c_str(),
				child.is_widget() ? child.get_widget_class().c_str() : "property");
		}

		if (ImGui::IsItemClicked())
		{
			highlight(child.get_x(), child.get_y(), child.get_width(), child.get_height());
		}

		if (show)
		{
			if (ImGui::Button("Highlight"))
			{
				highlight(child.get_x(), child.get_y(), child.get_width(), child.get_height());
			}

			ImGui::BulletText("Position: (%d, %d)", child.get_x(), child.get_y());
			ImGui::BulletText("Size: (%d, %d)", child.get_width(), child.get_height());

			if (!child.get_tags().empty() && ImGui::CollapsingHeader("Tags"))
			{
				auto& tags = child.get_tags();
				if (tags.empty())
				{
					ImGui::Text("(none)");
				}
				else
				{
					for (auto& tag: tags)
					{
						ImGui::BulletText("%s", tag.c_str());
					}
				}
			}

			ImGui::Text("Text");
			ImGui::Indent();
			if (!child.get_text().empty())
			{
				ImGui::Text("Unstyled:");
				ImGui::TextWrapped("%s", child.get_text().to_string().c_str());

				ImGui::Spacing();

				ImGui::Text("Styled:");
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
					bool first = true;
					for (auto segment: child.get_text())
					{
						auto text = std::get<chomsky::GUIText::SEGMENT_STRING>(segment);
						auto color = std::get<chomsky::GUIText::SEGMENT_COLOR>(segment);

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
						if (!first)
						{
							ImGui::SameLine();
						}
						first = false;
						ImGui::Text("%s", text.c_str());
						ImGui::PopStyleColor();
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
				}
			}
			else
			{
				ImGui::Text("(none)");
			}
			ImGui::Unindent();

			draw_content(child);

			ImGui::TreePop();
		}

		current = next;
		current_index = next_index;

		ImGui::PopID();
	}
}

void autonomaus::GUIContentComponent::highlight(
	int x, int y, int width, int height)
{
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();
	y = viewport.w - (y + height);

	auto& overlay = get_autonomaus().get_gui_draw_hook().after();
	GUIOverlay::Tag tag;
	overlay.start(tag);
	overlay.draw_rectangle_outline(
		glm::vec2(x, y), width, height,
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	overlay.stop();
}
