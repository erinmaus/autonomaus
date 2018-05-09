// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_VIEW_HPP
#define PRINCIPLE_COMMON_VIEW_HPP

#include <glm/glm.hpp>

namespace principle
{
	class KeyboardEvent;
	class MouseEvent;

	class View
	{
	public:
		View() = default;
		virtual ~View() = default;

		virtual glm::vec2 get_position() const;
		virtual void set_position(const glm::vec2& value);

		virtual glm::vec2 get_size() const;
		virtual void set_size(const glm::vec2& value);

		virtual void perform_layout();

		virtual void update(float delta);
		virtual void draw();

		virtual void on_mouse_button_down(const MouseEvent& event);
		virtual void on_mouse_button_up(const MouseEvent& event);
		virtual void on_mouse_move(const MouseEvent& event);
		virtual void on_mouse_scroll(const MouseEvent& event);
		virtual void on_key_down(const KeyboardEvent& event);
		virtual void on_key_up(const KeyboardEvent& event);

	protected:
		void apply_viewport();

	private:
		glm::vec2 position = glm::vec2(0.0f, 0.0f);
		glm::vec2 size = glm::vec2(0.0f, 0.0f);
	};
}

#endif
