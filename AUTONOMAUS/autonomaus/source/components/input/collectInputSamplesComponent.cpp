// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "autonomaus/core/time.hpp"
#include "autonomaus/components/input/collectInputSamplesComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/state/guiProvider.hpp"

autonomaus::CollectInputSamplesComponent::CollectInputSamplesComponent(
	Autonomaus& autonomaus) : Component(autonomaus)
{
	// Nothing.
}

void autonomaus::CollectInputSamplesComponent::show()
{
	this->visible = true;
}

void autonomaus::CollectInputSamplesComponent::reload()
{
	// Nothing.
}

void autonomaus::CollectInputSamplesComponent::update()
{
	if (this->capturing_mouse_input)
	{
		show_mouse_input();
		save_mouse_input();
	}

	if (this->capturing_keyboard_input)
	{
		save_keyboard_input();
	}

	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::CollectInputSamplesComponent::get_name() const
{
	return "Input Sample Collector";
}

void autonomaus::CollectInputSamplesComponent::draw()
{
	ImGui::Begin("Input Sample Collector", &this->visible);
	{
		draw_mouse();

		ImGui::Separator();
		ImGui::Spacing();

		draw_keyboard();
	}
	ImGui::End();
}

void autonomaus::CollectInputSamplesComponent::draw_mouse()
{
	ImGui::BeginGroup();
	ImGui::PushID("mouse");
	{
		ImGui::Text("Mouse");
		if (this->capturing_mouse_input)
		{
			const ImVec4 YELLOW = ImVec4(1, 1, 0, 1);
			ImGui::TextColored(YELLOW, "Capturing mouse input.");

			if (ImGui::Button("Cancel"))
			{
				capture_mouse_input(false);
				this->capturing_mouse_input = false;
			}
		}
		else
		{
			ImGui::Text("Not capturing mouse input.");

			if (ImGui::Button("Capture"))
			{
				capture_mouse_input(true);
			}
		}

		ImGui::Text("On click, highlight...");
		ImGui::Indent();
		ImGui::Checkbox("Elements", &this->show_gui_element);
		ImGui::Checkbox("Contents", &this->show_gui_content);
		ImGui::Unindent();
	}
	ImGui::PopID();
	ImGui::EndGroup();
}

void autonomaus::CollectInputSamplesComponent::draw_keyboard()
{
	ImGui::BeginGroup();
	ImGui::PushID("keyboard");
	{
		ImGui::Text("Keyboard");
		if (this->capturing_keyboard_input)
		{
			const ImVec4 YELLOW = ImVec4(1, 1, 0, 1);
			ImGui::TextColored(YELLOW, "Capturing keyboard input.");

			if (ImGui::Button("Cancel"))
			{
				capture_keyboard_input(false);
				this->capturing_keyboard_input = false;
			}
		}
		else
		{
			ImGui::Text("Not capturing keyboard input.");

			if (ImGui::Button("Capture"))
			{
				capture_keyboard_input(true);
			}
		}
	}
	ImGui::PopID();
	ImGui::EndGroup();
}

void autonomaus::CollectInputSamplesComponent::capture_mouse_input(bool enable)
{
	if (enable)
	{
		this->capturing_mouse_input = true;
		this->capture_mouse_start = get_current_time();

		std::ofstream stream("mouse.dat", std::ios::app);
		stream << "session" << std::endl;
	}
	else
	{
		this->capturing_mouse_input = false;
	}
}

void autonomaus::CollectInputSamplesComponent::show_mouse_input()
{
	auto& gui_hook = get_autonomaus().get_gui_draw_hook();
	if (gui_hook.wants_mouse_captured() || gui_hook.wants_keyboard_captured())
	{
		return;
	}

	auto& input_state = get_autonomaus().get_input_state();
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();
	float current_time = get_current_time();
	if (this->last_mouse_movement != HUGE_VALF)
	{
		auto& overlay = gui_hook.after();
		if (this->last_mouse_x != input_state.get_mouse_x() ||
			this->last_mouse_y != input_state.get_mouse_y())
		{
			glm::vec2 path[2] =
			{
				glm::vec2(this->last_mouse_x, this->last_mouse_y),
				glm::vec2(input_state.get_mouse_x(), input_state.get_mouse_y())
			};

			overlay.start(this->mouse_move_tag, false, 0.1f);
			overlay.draw_path(path, 2, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
			overlay.stop();
		}

		if (!this->last_left_click &&
			input_state.is_mouse_button_pressed(eiznebahn::MouseButton::button_left))
		{
			overlay.start(this->mouse_click_tag, false, 0.25f);
			int mouse_x = input_state.get_mouse_x();
			int mouse_y = viewport.w - input_state.get_mouse_y();

			if (this->show_gui_element)
			{
				show_gui_elements(overlay, mouse_x, mouse_y);
			}

			if (this->show_gui_content)
			{
				show_gui_contents(overlay, mouse_x, mouse_y);
			}

			overlay.stop();

			this->mouse_move_tag = GUIOverlay::Tag();
		}
	}
	this->last_left_click = input_state.is_mouse_button_pressed(eiznebahn::MouseButton::button_left);
	this->last_mouse_x = input_state.get_mouse_x();
	this->last_mouse_y = input_state.get_mouse_y();
	this->last_mouse_movement = current_time;
}

void autonomaus::CollectInputSamplesComponent::show_gui_elements(
	GUIOverlay& overlay,
	int mouse_x, int mouse_y)
{
	auto& gui = get_autonomaus().get_gui_state();
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();

	for (std::size_t i = 0; i < gui->get_num_elements(); ++i)
	{
		auto& element = gui->get_element(i);
		if (!element.get_is_textured())
		{
			continue;
		}

		glm::vec4 color;
		if (element.get_key().empty())
		{
			color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		}

		if (mouse_x >= element.get_x() &&
			mouse_x < element.get_x() + element.get_width() &&
			mouse_y >= element.get_y() &&
			mouse_y < element.get_y() + element.get_height())
		{
			int element_y = viewport.w - (element.get_y() + element.get_height());
			overlay.draw_rectangle_outline(
				glm::vec2(element.get_x(), element_y),
				element.get_width(), element.get_height(),
				color,
				2.0f);
		}
	}
}

void autonomaus::CollectInputSamplesComponent::show_gui_contents(
	GUIOverlay& overlay,
	int mouse_x, int mouse_y)
{
	auto& gui = get_autonomaus().get_gui_state();
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();

	auto callback = [&overlay, &viewport](const chomsky::GUIContent& content)
	{
		if (!content.is_widget())
		{
			return;
		}

		int content_y = viewport.w - (content.get_y() + content.get_height());
		overlay.draw_rectangle_outline(
			glm::vec2(content.get_x(), content_y),
			content.get_width(), content.get_height(),
			glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
			4.0f);
	};

	for (auto& i: *gui)
	{
		for_each_content_under_cursor(i.second, callback);
	}
}

void autonomaus::CollectInputSamplesComponent::save_mouse_input()
{
	auto& input_state = get_autonomaus().get_input_state();
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();
	int mouse_x = input_state.get_mouse_x();
	int mouse_y = viewport.w - input_state.get_mouse_y();

	float current_time = get_current_time() - this->capture_mouse_start;
	std::ofstream stream("mouse.dat", std::ios::app);
	stream << "frame" << " "
		<< get_autonomaus().get_current_frame() << " "
		<< (int)(current_time * 1000)
		<< std::endl;
	stream << "position" << " "
		<< mouse_x << " "
		<< mouse_y
		<< std::endl;

	for (int i = eiznebahn::MouseButton::button_min; i < eiznebahn::MouseButton::button_max; ++i)
	{
		if (input_state.is_mouse_button_pressed(i))
		{
			stream << "pressed" << " " << i << std::endl;
		}
	}

	auto& gui = get_autonomaus().get_gui_state();
	for (std::size_t i = 0; i < gui->get_num_elements(); ++i)
	{
		auto& element = gui->get_element(i);
		if (!element.get_is_textured() || element.get_key().empty())
		{
			continue;
		}

		if (mouse_x >= element.get_x() &&
			mouse_x < element.get_x() + element.get_width() &&
			mouse_y >= element.get_y() &&
			mouse_y < element.get_y() + element.get_height())
		{
			stream << "element" << " "
				<< element.get_x() << " "
				<< element.get_y() << " "
				<< element.get_width() << " "
				<< element.get_height() << " "
				<< element.get_key()
				<< std::endl;
		}
	}

	auto callback = [&stream](const chomsky::GUIContent& content)
	{
		if (!content.is_widget())
		{
			return;
		}

		stream << "widget" << " "
			<< content.get_x() << " "
			<< content.get_y() << " "
			<< content.get_width() << " "
			<< content.get_height() << " "
			<< content.get_widget_class() << " "
			<< content.get_name()
			<< std::endl;
	};
	for (auto& i: *gui)
	{
		for_each_content_under_cursor(i.second, callback);
	}
}


void autonomaus::CollectInputSamplesComponent::for_each_content_under_cursor(
	const chomsky::GUIContent& content,
	const std::function<void(const chomsky::GUIContent&)>& callback)
{
	auto& input_state = get_autonomaus().get_input_state();
	auto viewport = get_autonomaus().get_draw_state().get_gui_viewport();
	int mouse_x = input_state.get_mouse_x();
	int mouse_y = viewport.w - input_state.get_mouse_y();

	if (mouse_x >= content.get_x() &&
		mouse_x < content.get_x() + content.get_width() &&
		mouse_y >= content.get_y() &&
		mouse_y < content.get_y() + content.get_height())
	{
		if (content.get_name() != "root" && content.get_name() != "main")
		{
			callback(content);
		}

		for (auto& i: content)
		{
			for_each_content_under_cursor(*i.second, callback);
		}
	}
}

void autonomaus::CollectInputSamplesComponent::capture_keyboard_input(bool enable)
{
	if (enable)
	{
		this->capturing_keyboard_input = true;
		this->capture_keyboard_start = get_current_time();

		std::ofstream stream("keyboard.dat", std::ios::app);
		stream << "session" << std::endl;
	}
	else
	{
		this->capturing_keyboard_input = false;
	}
}

void autonomaus::CollectInputSamplesComponent::save_keyboard_input()
{
	auto& input_state = get_autonomaus().get_input_state();
	auto& gui_hook = get_autonomaus().get_gui_draw_hook();
	if (gui_hook.wants_mouse_captured() || gui_hook.wants_keyboard_captured())
	{
		return;
	}

	float current_time = get_current_time();
	std::ofstream stream("keyboard.dat", std::ios::app);
	stream << "frame" << " "
		<< get_autonomaus().get_current_frame() << " "
		<< (int)((current_time - this->capture_keyboard_start) * 1000)
		<< std::endl;

	for (int i = eiznebahn::KeySequence::key_code_min; i < eiznebahn::KeySequence::key_code_max; ++i)
	{
		bool previously_pressed = (this->keys.count(i) != 0);
		bool currently_pressed = input_state.is_key_pressed(i);

		if (previously_pressed && !currently_pressed)
		{
			stream << "released" << " "
				<< i << " "
				<< (int)((current_time - this->keys[i]) * 1000)
				<< std::endl;
			this->keys.erase(i);
		}
		else if (!previously_pressed && currently_pressed)
		{
			stream << "pressed" << " " << i << std::endl;
			this->keys[i] = current_time;
		}
	}
}