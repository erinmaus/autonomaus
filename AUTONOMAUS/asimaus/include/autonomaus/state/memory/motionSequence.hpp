// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_MOTION_SEQUENCE_HPP
#define AUTONOMAUS_STATE_MEMORY_MOTION_SEQUENCE_HPP

#include <functional>
#include <glm/glm.hpp>
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/input/gestures/gesture.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace autonomaus
{
	class MotionSequence
	{
	public:
		typedef std::function<bool()> Guard;

		MotionSequence(Cortex& cortex);
		MotionSequence(MotionSequence&& other) = default;
		~MotionSequence();

		void left_click();

		void left_click(const chomsky::GUIContent& content);
		void left_click(
			const chomsky::GUIContent& content,
			const Guard& guard);

		void left_click(const glm::vec3& world_position);
		void left_click(const glm::vec3& world_position, const Guard& guard);

		void right_click();

		void right_click(const chomsky::GUIContent& content);
		void right_click(
			const chomsky::GUIContent& content,
			const Guard& guard);

		void right_click(const glm::vec3& world_position);
		void right_click(const glm::vec3& world_position, const Guard& guard);

		void click(const chomsky::GUIContent& content, int button);
		void click(
			const chomsky::GUIContent& content,
			int button,
			const Guard& guard);

		void click(const glm::vec3& world_position, int button);
		void click(
			const glm::vec3& world_position,
			int button,
			const Guard& guard);

		void click(int button);

		void hover(const chomsky::GUIContent& content);
		void hover(const chomsky::GUIContent& content, const Guard& guard);

		void hover(const glm::vec3& world_position);
		void hover(const glm::vec3& world_position, const Guard& guard);

		void scroll_up(const chomsky::GUIContent& content, int ticks);
		void scroll_up(
			const chomsky::GUIContent& content,
			int ticks,
			const Guard& guard);

		void scroll_up(const glm::vec2& world_screen_position, int ticks);
		void scroll_up(
			const glm::vec2& world_screen_position,
			int ticks,
			const Guard& guard);

		void scroll_down(const chomsky::GUIContent& content, int ticks);
		void scroll_down(
			const chomsky::GUIContent& content,
			int ticks,
			const Guard& guard);

		void scroll_down(const glm::vec2& world_screen_position, int ticks);
		void scroll_down(
			const glm::vec2& world_screen_position,
			int ticks,
			const Guard& guard);

		void drag(
			const chomsky::GUIContent& source,
			const chomsky::GUIContent& destination,
			float speed = 1.0f);
		void drag(
			const chomsky::GUIContent& source,
			const chomsky::GUIContent& destination,
			float speed,
			const Guard& guard);

		// 'x' and 'y' are in GUI co-ordinates. (+Y is up).
		void drag(
			const chomsky::GUIContent& source,
			int x, int y,
			float speed = 1.0f);
		void drag(
			const chomsky::GUIContent& source,
			int x, int y,
			float speed,
			const Guard& guard);

		// 'source' and 'destination' are relative to the world viewport.
		void drag(
			const glm::vec2& source,
			const glm::vec2& destination,
			int button);
		void drag(
			const glm::vec2& source,
			const glm::vec2& destination,
			int button,
			const Guard& guard);

		void type(
			const std::string& text,
			bool ignore_case);
		void type(
			const std::string& text,
			bool ignore_case,
			const Guard& guard);

		template <typename C>
		void poke(const std::string& action, const Message& message);

		template <typename C>
		void poke(
			const std::string& action,
			const Message& message,
			const Guard& guard);

		void wait(float hint, const Guard& guard);

		template <typename C>
		void wait(float hint);

		void invoke(const std::function<void()>& func);

		Cortex::Result::Status update();
		void cancel();

		bool pending() const;

	private:
		Cortex* cortex;

		typedef std::unique_ptr<Gesture> GesturePointer;
		struct Motion
		{
			Motion(
				Cortex& cortex,
				GesturePointer& pointer,
				const Guard& guard,
				int button = eiznebahn::MouseButton::button_none);
			Motion(
				Cortex& cortex,
				const std::string& action,
				const Message& message,
				const Guard& guard);
			Motion(Cortex::Result::Status status);
			Motion(float duration, const Guard& guard);;

			bool pending();
			bool start();
			bool update();
			void cancel();

			enum
			{
				type_gesture,
				type_poke,
				type_wait
			};
			int type;

			GesturePointer gesture;
			int button = eiznebahn::MouseButton::button_none;

			Cortex* cortex = nullptr;
			std::string action;
			Message message;
			Cortex::Result result;

			float duration = 0.0f;
			float current_time = 0.0f;

			Guard guard;
			bool failing = false;
			float fail_time = 0.0f;
			static const float GUARD_WAIT_TIME;
			bool failed();
		};

		std::vector<Motion> motions;
		std::size_t current_motion = 0;
		Cortex::Result::Status status = Cortex::Result::Status::working;

		bool is_ready = false;
		bool is_valid = true;
	};
}

template <typename C>
void autonomaus::MotionSequence::poke(
	const std::string& action,
	const Message& message)
{
	poke<C>(action, message, [] { return true; });
}

template <typename C>
void autonomaus::MotionSequence::poke(
	const std::string& action,
	const Message& message,
	const Guard& guard)
{
	auto c = cortex->get_autonomaus().get_userdata<C>();
	if (c == nullptr)
	{
		this->motions.emplace_back(Cortex::Result::Status::failure);
	}
	else
	{
		this->motions.emplace_back(*c, action, message, guard);
	}
}

template <typename C>
void autonomaus::MotionSequence::wait(float hint)
{
	auto c = cortex->get_autonomaus().get_userdata<C>();
	if (c == nullptr)
	{
		this->motions.emplace_back(Cortex::Result::Status::failure);
	}
	else
	{
		wait(hint, [c] { return c->is_visible(); });
	}
}

#endif
