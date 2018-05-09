// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_MOUSE_EVENT_HPP
#define PRINCIPLE_COMMON_MOUSE_EVENT_HPP

#include <glm/glm.hpp>

namespace principle
{
	class MouseEvent
	{
	public:
		MouseEvent() = default;
		~MouseEvent() = default;

		glm::vec2 get_position() const;
		void set_position(const glm::vec2& value);

		glm::vec2 get_difference() const;
		void set_difference(const glm::vec2& value);

		float get_wheel() const;
		void set_wheel(float value);

		int get_button() const;
		void set_button(int value);

	private:
		glm::vec2 position = glm::vec2(0.0f);
		glm::vec2 difference = glm::vec2(0.0f);
		float wheel = 0.0f;
		int button = 0;
	};
}

#endif
