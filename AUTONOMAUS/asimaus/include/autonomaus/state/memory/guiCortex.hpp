// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_GUI_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_GUI_CORTEX_HPP

#include <string>
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class GUICortex : public Cortex
	{
	public:
		~GUICortex();

		virtual bool is_visible() const;
		virtual bool is_ready() const;

		void update() override;

	protected:
		GUICortex(
			const std::string& name,
			Autonomaus& autonomaus,
			const chomsky::GUIQuery& root,
			const std::string& gui = "game");

		Cortex::Result poke(
			const chomsky::GUIContent& content,
			const std::string& action,
			bool ignore_tool_tip);
		Cortex::Result poke_any(
			const chomsky::GUIContent& content,
			const std::vector<std::string>& actions,
			bool ignore_tool_tip);
		Cortex::Result poke(
			const chomsky::GUIContent& content,
			const std::string& action,
			bool ignore_tool_tip,
			const MotionSequence::Guard& guard);
		Cortex::Result poke_any(
			const chomsky::GUIContent& content,
			const std::vector<std::string>& actions,
			bool ignore_tool_tip,
			const MotionSequence::Guard& guard);

		glm::ivec2 get_mouse_position() const;
		glm::ivec2 point_to_screen(const glm::ivec2& point) const;
		glm::ivec2 screen_to_point(const glm::ivec2& screen) const;

		bool try_get_global_root(chomsky::GUIContent& result) const;
		bool try_get_root(chomsky::GUIContent& result) const;
		bool try_get_root(const chomsky::GUIQuery& query, chomsky::GUIContent& result) const;
		bool try_get_child(const chomsky::GUIQuery& query, chomsky::GUIContent& result) const;
		bool try_get_content_under_mouse(chomsky::GUIContent& result) const;
		bool try_get_content_under_point(const glm::ivec2& point, chomsky::GUIContent& result) const;
		bool try_get_element_under_mouse(chomsky::GUIElement& result) const;
		bool try_get_element_under_point(const glm::ivec2& point, chomsky::GUIElement& result) const;
		bool try_get_text(const chomsky::GUIContent& content, std::string& result) const;
		bool try_get_data(const chomsky::GUIContent& content, const std::string& data, std::string& result) const;
		bool try_get_name(const chomsky::GUIContent& content, std::string& name) const;
		bool try_get_item(const chomsky::GUIContent& content, Memory& result) const;
		bool try_get_item(const chomsky::GUIElement& element, Memory& result) const;

		bool try_get_vertical_scroll_bar_percent(
			const chomsky::GUIContent& scroll_bar,
			float& value) const;
		bool try_get_vertical_scroll_bar_percent(
			const chomsky::GUIQuery& query,
			float& value) const;
		bool try_get_vertical_scroll_area(
			const chomsky::GUIContent& scroll_bar,
			float visible_area,
			float& value) const;
		bool try_get_vertical_scroll_area(
			const chomsky::GUIQuery& query,
			float visible_area,
			float& value) const;

	private:
		std::string gui;
		chomsky::GUIQuery root_query;
		chomsky::GUIContent root_content;
		bool valid = false;

		bool do_try_get_content_under_point(
			const chomsky::GUIContent& content,
			const glm::ivec2& pointer,
			chomsky::GUIContent& result) const;

		struct PokeAction;
	};
}

#endif
