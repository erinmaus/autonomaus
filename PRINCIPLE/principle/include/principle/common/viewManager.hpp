// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_VIEW_MANAGER_HPP
#define PRINCIPLE_COMMON_VIEW_MANAGER_HPP

#include <array>
#include <map>
#include <glm/glm.hpp>
#include "principle/common/view.hpp"

namespace principle
{
	class Display;
	class KeyboardEvent;
	class MouseEvent;

	class ViewManager : public View
	{
	public:
		static const int MAX_VIEWS = 4;
		static const int DIVIDER_SIZE = 4;

		ViewManager(Display* display, const View* parent = nullptr);
		~ViewManager() = default;

		float get_vertical_divider() const;
		void set_vertical_divider(float value);
		void enable_vertical_divider(float value = 0.5f);
		void disable_vertical_divider();
		bool get_is_vertical_divider_enabled() const;

		float get_horizontal_divider() const;
		void set_horizontal_divider(float value);
		void enable_horizontal_divider(float value = 0.5f);
		void disable_horizontal_divider();
		bool get_is_horizontal_divider_enabled() const;

		glm::vec2 get_position() const override;
		void set_position(const glm::vec2& value) override;

		glm::vec2 get_size() const override;
		void set_size(const glm::vec2& value) override;

		void perform_layout() override;
		void update(float delta) override;
		void draw() override;

		void on_mouse_button_down(const MouseEvent& event) override;
		void on_mouse_button_up(const MouseEvent& event) override;
		void on_mouse_move(const MouseEvent& event) override;
		void on_mouse_scroll(const MouseEvent& event) override;
		void on_key_down(const KeyboardEvent& event) override;
		void on_key_up(const KeyboardEvent& event) override;

		bool has_view(int index) const;
		View* get_view(int index) const;
		void set_view(int index, View* view);
		void unset_view(int index);

	private:
		void layout_view(
			int index,
			const glm::vec2& position, const glm::vec2& size);
		void draw_view(int index);

		Display* display;
		const View* parent;

		typedef std::array<View*, MAX_VIEWS> ViewCollection;
		ViewCollection views;

		float vertical_divider = 0.0f;
		bool vertical_divider_enabled = false;
		float horizontal_divider = 0.0f;
		bool horizontal_divider_enabled = false;

		bool is_dragging_vertical_divider = false;
		int vertical_divider_drag_button = 0;
		bool is_dragging_horizontal_divider = false;
		int horizontal_divider_drag_button = 0;
		int active_view = 0;
		bool is_view_active = false;
		int view_active_input_count = 0;
		void set_active_view(const glm::vec2& focus_position);

	public:
		typedef ViewCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
