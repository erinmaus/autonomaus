// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/core/demangle.hpp>
#include "autonomaus/components/asimaus/debuggerComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"

autonomaus::DebuggerComponent::DebuggerComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::DebuggerComponent::start(
	mashina::Tree& tree, mashina::Executor& executor)
{
	if (this->tree != nullptr)
	{
		stop();
	}

	this->tree = &tree;
	this->executor = &executor;
	this->working = false;
	show();
}

void autonomaus::DebuggerComponent::stop()
{
	this->context.reset();
	this->tree = nullptr;
	this->executor = nullptr;
	this->previous_node = nullptr;
	this->current_node = nullptr;
	this->next_node = nullptr;
}

void autonomaus::DebuggerComponent::show()
{
	this->visible = true;
}

void autonomaus::DebuggerComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

void autonomaus::DebuggerComponent::reload()
{
	// Nothing.
}

std::string autonomaus::DebuggerComponent::get_name() const
{
	// Debug. But harsher. Because if I'm debugging, I'm not usually happy.
	return "ENTWANZEN!";
}

void autonomaus::DebuggerComponent::draw()
{
	bool state_inline = this->draw_state_view_inline;
	bool tree_inline = this->draw_tree_view_inline;

	ImGui::Begin("ENTWANZEN", &this->visible);
	{
		bool enable_debug = get_autonomaus().get_is_debug_enabled();
		if (ImGui::Checkbox("Enable Debug Mode", &enable_debug))
		{
			if (enable_debug && !get_autonomaus().get_is_debug_enabled())
			{
				get_autonomaus().enable_debug();
			}
			else if (!enable_debug && get_autonomaus().get_is_debug_enabled())
			{
				get_autonomaus().disable_debug();
			}
		}

		ImGui::SameLine();
		bool suppress_input = get_autonomaus().get_is_input_suppressed();
		if (ImGui::Checkbox("Suppress Real Input", &suppress_input))
		{
			if (suppress_input && !get_autonomaus().get_is_input_suppressed())
			{
				get_autonomaus().suppress_input();
			}
			else if (!suppress_input && get_autonomaus().get_is_input_suppressed())
			{
				get_autonomaus().restore_input();
			}
		}

		if (this->tree != nullptr)
		{
			draw_controls();

			if (this->context && !this->context->done())
			{
				ImGui::Text("Status:");
				ImGui::SameLine();
				switch (this->context->status())
				{
					case mashina::Status::none:
						ImGui::Text("none");
						break;
					case mashina::Status::success:
						ImGui::TextColored(ImVec4(0, 1, 0, 1), "success");
						break;
					case mashina::Status::failure:
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "failure");
						break;
					case mashina::Status::working:
						ImGui::Text("working...");
						break;
				}

				switch (this->context->type())
				{
					case mashina::DebugBreak::before:
						ImGui::Text("Break: before");
						break;
					case mashina::DebugBreak::after:
						ImGui::Text("Break: after");
						break;
				}

				ImVec2 frame_size;
				{
					auto window_size = ImGui::GetWindowSize();
					frame_size.x = window_size.x;
					frame_size.y = window_size.y * 0.75f;
					if (this->draw_tree_view_inline && this->draw_state_view_inline)
					{
						frame_size.y /= 2.0f;
					}
				}

				if (tree_inline)
				{
					ImGui::Text("Tree");

					ImGui::SameLine();
					if (ImGui::SmallButton("Expand"))
					{
						this->draw_tree_view_inline = false;
					}

					ImGui::BeginChildFrame(0, frame_size);
					draw_tree();
					ImGui::EndChildFrame();
				}

				if (state_inline)
				{
					ImGui::Text("State");

					ImGui::SameLine();
					if (ImGui::SmallButton("Expand"))
					{
						this->draw_state_view_inline = false;
					}

					ImGui::BeginChildFrame(1, frame_size);
					draw_state();
					ImGui::EndChildFrame();
				}
			}
		}
	}
	ImGui::End();

	if (!tree_inline)
	{
		ImGui::Begin("ENTWANZEN (Tree)", &this->draw_tree_view_inline);
		draw_tree();
		ImGui::End();
	}

	if (!state_inline)
	{
		ImGui::Begin("ENTWANZEN (State)", &this->draw_state_view_inline);
		draw_state();
		ImGui::End();
	}
}

void autonomaus::DebuggerComponent::draw_controls()
{
	if (this->context && !this->context->done())
	{
		if (ImGui::Button("Step"))
		{
			this->context->next();
		}

		ImGui::SameLine();
		if (ImGui::Button("Step Out"))
		{
			this->context->out();
		}

		ImGui::SameLine();
		if (ImGui::Button("Step Over"))
		{
			this->context->over();
		}

		ImGui::SameLine();
		if (ImGui::Button("Continue"))
		{
			this->context->finish();
		}

		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			this->context.reset();
			this->working = false;
		}
	}
	else
	{
		if (ImGui::Button(this->working ? "Resume" : "Start"))
		{
			this->working = true;
			this->context = std::make_unique<mashina::DebugContext>(
				*this->tree, *this->executor);
		}
	}
}

void autonomaus::DebuggerComponent::draw_tree()
{
	if (!this->context || this->context->done())
	{
		ImGui::BulletText("(none)");
		return;
	}

	if (this->current_node != &this->context->node())
	{
		this->next_node = &this->context->node();
	}

	auto& tree = this->context->tree();
	draw_tree_node(tree, tree.root());
}

void autonomaus::DebuggerComponent::draw_tree_node(
	mashina::Tree& tree, mashina::Node& node)
{
	static const int FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

	auto name = boost::core::demangle(typeid(node).name());
	bool visible;
	{
		if (this->previous_node == &node && this->next_node == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
			visible = ImGui::TreeNodeEx(name.c_str(), FLAGS);
			ImGui::PopStyleColor();
		}
		else if (this->current_node == &node)
		{
			if (this->next_node == nullptr)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
			}
			visible = ImGui::TreeNodeEx(name.c_str(), FLAGS);
			ImGui::PopStyleColor();
		}
		else if (this->next_node == &node)
		{
			ImGui::SetScrollHere(0.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			visible = ImGui::TreeNodeEx(name.c_str(), FLAGS);
			ImGui::PopStyleColor();

			this->previous_node = this->current_node;
			this->current_node = this->next_node;
			this->next_node = nullptr;
		}
		else
		{
			visible = ImGui::TreeNodeEx(name.c_str(), FLAGS);
		}
	}

	if (visible)
	{
		auto current = tree.children_begin(node);
		auto end = tree.children_end(node);

		while (current != end)
		{
			draw_tree_node(tree, *current);
			++current;
		}

		ImGui::TreePop();
	}
}

void autonomaus::DebuggerComponent::draw_state()
{
	if (!this->context || this->context->done())
	{
		return;
	}

	this->context->state().for_each_property([](auto& key, auto& value)
	{
		if (key.empty() || key[0] == '_' || key.substr(0, 2) == "0x")
		{
			return;
		}

		ImGui::TextWrapped("%s: %s", key.c_str(), value.c_str());
		ImGui::Separator();
	});
}
