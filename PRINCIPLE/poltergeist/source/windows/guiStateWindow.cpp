// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "poltergeist/application.hpp"
#include "principle/common/gui.hpp"
#include "principle/deps/imgui/imgui.h"
#include "chomsky/gui/guiQuery.hpp"

poltergeist::Application::GUIStateWindow::GUIStateWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	open();
}

std::string poltergeist::Application::GUIStateWindow::get_title() const
{
	return "GUI State";
}

void poltergeist::Application::GUIStateWindow::update_gui()
{
	if (ImGui::Button("Process"))
	{
		this->application->process_gui_state();
	}

	ImGui::SameLine();
	if (ImGui::Button("Perform Query") && !this->query.empty())
	{
		this->query_results.clear();
		try
		{
			chomsky::GUIQuery query(this->query);
			query.collect(this->application->gui_state, this->query_results);
		}
		catch (const std::exception& e)
		{
			printf("Error running query: %s\n", e.what());
		}
	}

	if (!this->query_results.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Clear Results"))
		{
			this->query_results.clear();
		}
	}

	ImGui::InputString("Query", this->query);

	if (!this->application->gui_state_success)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Text("failed to process stream:");
		ImGui::TextWrapped("%s", this->application->gui_state_error.c_str());
		ImGui::PopStyleColor();
	}
	else
	{
		if (this->application->gui_state.empty())
		{
			ImGui::Text("(empty)");
		}
		else
		{
			if (this->query_results.empty())
			{
				show_content(this->application->gui_state);
			}
			else
			{
				for (auto& i: this->query_results)
				{
					show_content(i);
				}
			}
		}
	}
}

void poltergeist::Application::GUIStateWindow::show_content(
	const chomsky::GUIContent& content)
{
	ImGui::Indent();

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

		if (ImGui::IsItemHovered())
		{
			this->application->set_highlight(
				child.get_x(), child.get_x() + child.get_width(),
				child.get_y() + child.get_height(), child.get_y());
		}

		if (show)
		{
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

			if (!child.get_text().empty() && ImGui::CollapsingHeader("Text"))
			{
				ImGui::Text("Unstyled:");
				ImGui::Text("%s", child.get_text().to_string().c_str());

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

			show_content(child);

			ImGui::TreePop();
		}

		ImGui::PopID();

		current = next;
		current_index = next_index;
	}

	ImGui::Unindent();
}
