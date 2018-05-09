// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "autonomaus/components/gui/chatLogComponent.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/state/guiProvider.hpp"

const std::size_t autonomaus::ChatLogComponent::HOUR;
const std::size_t autonomaus::ChatLogComponent::MINUTE;
const std::size_t autonomaus::ChatLogComponent::SECOND;
const std::size_t autonomaus::ChatLogComponent::HISTORY_LENGTH;

autonomaus::ChatLogComponent::ChatLogComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	auto current_time = std::time(nullptr);
	auto current_time_components = std::localtime(&current_time);
	this->reference_time_stamp = std::make_tuple(
		current_time_components->tm_hour,
		current_time_components->tm_min,
		current_time_components->tm_sec);

	FILE* file = std::fopen(Autonomaus::get_config_path("chat.log").c_str(), "a");
	std::fprintf(file, "[%02d:%02d:%02d] *** Started AUTONOMAUS.\n",
		current_time_components->tm_hour,
		current_time_components->tm_min,
		current_time_components->tm_sec);
	std::fclose(file);
}

void autonomaus::ChatLogComponent::show()
{
	this->visible = true;
}

void autonomaus::ChatLogComponent::update()
{
	auto& gui = *get_autonomaus().get_gui_state();
	if (gui.has("game"))
	{
		update_chat(gui.get("game"));
	}

	if (this->visible)
	{
		draw();
	}
}

void autonomaus::ChatLogComponent::reload()
{
	// Nothing.
}

std::string autonomaus::ChatLogComponent::get_name() const
{
	return "Chat Log";
}

void autonomaus::ChatLogComponent::draw()
{
	ImGui::Begin("Chat Log", &this->visible);
	{
		ImGui::Checkbox("Write to Log", &this->write_to_log);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Write messages to AUTONOMAUS log as well.");
		}

		ImGui::Separator();
		ImGui::Spacing();

		for (auto message: this->history)
		{
			ImGui::TextWrapped("%s", std::get<MESSAGE>(message).c_str());
		}
	}
	ImGui::End();
}

void autonomaus::ChatLogComponent::update_chat(const chomsky::GUIContent& content)
{
	auto main = content.try_get("main").try_get("panels");
	auto num_panels = main.count("panels");

	for (std::size_t i = 0; i < num_panels; ++i)
	{
		auto& panel = main.get("panels", i);
		if (panel.has("content"))
		{
			auto& content = panel.get("content");
			if (content.get_widget_class() == "chat-panel-content" &&
				!content.get_tags().has("quick-chat"))
			{
				extract_log(content.try_get("log"));
				break;
			}
		}
	}
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

static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

void autonomaus::ChatLogComponent::extract_log(const chomsky::GUIContent& content)
{
	std::size_t count = content.count("log");

	for (std::size_t i = count; i > 0; --i)
	{
		auto& entry = content.get("log", i - 1);
		auto value = entry.get_text().to_string();
		trim(value);

		auto time_stamp_start = value.find("[");
		if (time_stamp_start == std::string::npos ||
			time_stamp_start + 1 >= value.length())
		{
			continue;
		}

		auto time_stamp_end = value.find("]", time_stamp_start + 1);
		if (time_stamp_end == std::string::npos)
		{
			continue;
		}

		try
		{
			std::size_t hour_end;
			int hour = std::stoi(value.substr(time_stamp_start + 1), &hour_end);
			std::size_t minute_end;
			int minute = std::stoi(value.substr(hour_end), &minute_end);
			std::size_t second_end;
			int second = std::stoi(value.substr(minute_end), &second_end);

			if (hour < std::get<HOUR>(this->reference_time_stamp))
			{
				hour += 24;
			}

			if (value[time_stamp_end + 1] != ' ')
			{
				value.insert(time_stamp_end + 1, " ");
			}

			auto time_stamp = std::make_tuple(hour, minute, second);
			add_message(time_stamp, value);
		}
		catch (...)
		{
			static bool warned = false;
			if (!warned)
			{
				auto time_stamp_text = value.substr(time_stamp_start, time_stamp_end - time_stamp_start + 1);
				Log::warn("Couldn't parse time_stamp message: '{0}'", time_stamp_text);
				warned = true;
			}
		}
	}
}

void autonomaus::ChatLogComponent::add_message(
	const TimeStamp& time_stamp,
	const std::string& value)
{
	auto message = std::make_tuple(time_stamp, value);
	if (this->history.count(message) == 0)
	{
		if (this->write_to_log)
		{
			Log::info("[Chat]: {0}", value);
		}

		this->history.insert(message);

		std::ofstream stream(Autonomaus::get_config_path("chat.log"), std::ios::app);
		stream << value << std::endl;

		if (this->history.size() > HISTORY_LENGTH)
		{
			this->history.erase(this->history.begin());
		}
	}
}
