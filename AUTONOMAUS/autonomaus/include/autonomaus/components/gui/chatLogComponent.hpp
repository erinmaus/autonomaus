// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_GUI_CHAT_LOG_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_GUI_CHAT_LOG_COMPONENT_HPP

#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/gui.hpp"

namespace autonomaus
{
	class ChatLogComponent : public Component
	{
	public:
		explicit ChatLogComponent(Autonomaus& autonomaus);
		~ChatLogComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();

		bool write_to_log = false;

		static const std::size_t HOUR = 0;
		static const std::size_t MINUTE = 1;
		static const std::size_t SECOND = 2;
		typedef std::tuple<int, int, int> TimeStamp;

		static const std::size_t TIME_STAMP = 0;
		static const std::size_t MESSAGE = 1;
		typedef std::tuple<TimeStamp, std::string> Message;

		const static std::size_t HISTORY_LENGTH = 512;
		std::set<Message> history;

		TimeStamp reference_time_stamp;

		void update_chat(const chomsky::GUIContent& content);
		void extract_log(const chomsky::GUIContent& log);
		void add_message(const TimeStamp& time_stamp, const std::string& value);
	};
}

#endif
