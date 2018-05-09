// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/time.hpp"
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/input/gestures/guiClickContentGesture.hpp"
#include "autonomaus/input/gestures/guiMoveToContentGesture.hpp"
#include "autonomaus/input/gestures/mousePressGesture.hpp"
#include "autonomaus/input/gestures/typeGesture.hpp"
#include "autonomaus/input/gestures/worldMouseMoveGesture.hpp"
#include "autonomaus/input/gestures/worldViewportMouseMoveGesture.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"

autonomaus::MotionSequence::MotionSequence(Cortex& cortex)
{
	this->cortex = &cortex;
}

autonomaus::MotionSequence::~MotionSequence()
{
	if (pending())
	{
		cancel();
	}
}

void autonomaus::MotionSequence::left_click()
{
	click(eiznebahn::MouseButton::button_left);
}

void autonomaus::MotionSequence::left_click(const chomsky::GUIContent& content)
{
	left_click(content, [] { return true; });
}

void autonomaus::MotionSequence::left_click(
	const chomsky::GUIContent& content,
	const Guard& guard)
{
	click(content, eiznebahn::MouseButton::button_left, guard);
}

void autonomaus::MotionSequence::left_click(const glm::vec3& world_position)
{
	return left_click(world_position, [] { return true; });
}

void autonomaus::MotionSequence::left_click(
	const glm::vec3& world_position,
	const Guard& guard)
{
	return click(world_position, eiznebahn::MouseButton::button_left, guard);
}

void autonomaus::MotionSequence::right_click()
{
	click(eiznebahn::MouseButton::button_right);
}

void autonomaus::MotionSequence::right_click(const chomsky::GUIContent& content)
{
	right_click(content, [] { return true; });
}

void autonomaus::MotionSequence::right_click(
	const chomsky::GUIContent& content,
	const Guard& guard)
{
	click(content, eiznebahn::MouseButton::button_right, guard);
}

void autonomaus::MotionSequence::right_click(const glm::vec3& world_position)
{
	return right_click(world_position, [] { return true; });
}

void autonomaus::MotionSequence::right_click(
	const glm::vec3& world_position,
	const Guard& guard)
{
	return click(world_position, eiznebahn::MouseButton::button_right, guard);
}

void autonomaus::MotionSequence::click(const chomsky::GUIContent& content, int button)
{
	click(content, button, [] { return true; });
}

void autonomaus::MotionSequence::click(
	const chomsky::GUIContent& content,
	int button,
	const Guard& guard)
{
	hover(content, guard);

	GesturePointer gesture;
	{
		auto draw_state = cortex->get_autonomaus().get_draw_state();
		auto& input_manager = cortex->get_autonomaus().get_input_manager();
		auto width = draw_state.get_gui_viewport().z;
		auto height = draw_state.get_gui_viewport().w;

		gesture = std::make_unique<GUIClickContentGesture>(
			input_manager,
			content,
			width, height,
			button);
	}

	// The guard is only queried when approaching the element, not when
	// clicking. This ensures, for example, when the right click menu option is
	// clicked, while waiting for the click to finish, the guard doesn't cause a
	// premature failure because the right click menu is no longer open.
	//
	// Similarly, a click can't be exactly be canceled!
	this->motions.emplace_back(*this->cortex, gesture, [] { return true; });
}

void autonomaus::MotionSequence::click(const glm::vec3& world_position, int button)
{
	click(world_position, button, [] { return true; });
}

void autonomaus::MotionSequence::click(
	const glm::vec3& world_position,
	int button,
	const Guard& guard)
{
	hover(world_position, guard);

	GesturePointer gesture;
	{
		auto& input_manager = cortex->get_autonomaus().get_input_manager();
		gesture = std::make_unique<MousePressGesture>(
			input_manager,
			button,
			true);
	}

	// See click(const chomsky::GUIContent&, int button, const Guard& guard).
	this->motions.emplace_back(*this->cortex, gesture, [] { return true; });
}

void autonomaus::MotionSequence::click(int button)
{
	GesturePointer gesture;
	{
		auto& input_manager = cortex->get_autonomaus().get_input_manager();
		gesture = std::make_unique<MousePressGesture>(
			input_manager,
			button,
			true);
	}

	this->motions.emplace_back(*this->cortex, gesture, [] { return true; });
}

void autonomaus::MotionSequence::hover(const chomsky::GUIContent& content)
{
	return hover(content, [] { return true; });
}

void autonomaus::MotionSequence::hover(
	const chomsky::GUIContent& content,
	const Guard& guard)
{
	GesturePointer gesture;
	{
		auto draw_state = cortex->get_autonomaus().get_draw_state();
		auto& input_manager = cortex->get_autonomaus().get_input_manager();
		auto width = draw_state.get_gui_viewport().z;
		auto height = draw_state.get_gui_viewport().w;

		gesture = std::make_unique<GUIMoveToContentGesture>(
			input_manager,
			content,
			width, height);
	}

	this->motions.emplace_back(*this->cortex, gesture, guard);
}

void autonomaus::MotionSequence::hover(const glm::vec3& world_position)
{
	hover(world_position, [] { return true; });
}

void autonomaus::MotionSequence::hover(
	const glm::vec3& world_position,
	const Guard& guard)
{
	GesturePointer gesture;
	{
		auto draw_state = cortex->get_autonomaus().get_draw_state();
		auto camera = Camera(draw_state);
		auto& input_manager = cortex->get_autonomaus().get_input_manager();

		gesture = std::make_unique<WorldMouseMoveGesture>(
			input_manager,
			world_position,
			camera);
	}

	this->motions.emplace_back(*this->cortex, gesture, guard);
}

void autonomaus::MotionSequence::scroll_up(const chomsky::GUIContent& content, int ticks)
{
	return scroll_up(content, ticks, [] { return true; });
}

void autonomaus::MotionSequence::scroll_up(
	const chomsky::GUIContent& content,
	int ticks,
	const Guard& guard)
{
	hover(content, guard);

	for (int i = 0; i < ticks; ++i)
	{
		click(content, eiznebahn::MouseButton::button_up, guard);
	}
}

void autonomaus::MotionSequence::scroll_up(const glm::vec2& world_screen_position, int ticks)
{
	return scroll_up(world_screen_position, ticks, [] { return true; });
}

void autonomaus::MotionSequence::scroll_up(
	const glm::vec2& world_screen_position,
	int ticks,
	const Guard& guard)
{
	auto draw_state = cortex->get_autonomaus().get_draw_state();
	auto camera = Camera(draw_state);
	auto& input_manager = cortex->get_autonomaus().get_input_manager();

	GesturePointer to_position = std::make_unique<WorldViewportMouseMoveGesture>(
		input_manager, world_screen_position, camera);
	this->motions.emplace_back(*this->cortex, to_position, guard);

	for (int i = 0; i < ticks; ++i)
	{
		GesturePointer scroll = std::make_unique<MousePressGesture>(
			input_manager, eiznebahn::MouseButton::button_up, true);
		this->motions.emplace_back(*this->cortex, scroll, guard);
	}
}

void autonomaus::MotionSequence::scroll_down(const chomsky::GUIContent& content, int ticks)
{
	return scroll_down(content, ticks, [] { return true; });
}

void autonomaus::MotionSequence::scroll_down(
	const chomsky::GUIContent& content,
	int ticks,
	const Guard& guard)
{
	hover(content, guard);

	for (int i = 0; i < ticks; ++i)
	{
		click(content, eiznebahn::MouseButton::button_down, guard);
	}
}

void autonomaus::MotionSequence::scroll_down(const glm::vec2& world_screen_position, int ticks)
{
	return scroll_down(world_screen_position, ticks, [] { return true; });
}

void autonomaus::MotionSequence::scroll_down(
	const glm::vec2& world_screen_position,
	int ticks,
	const Guard& guard)
{
	auto draw_state = cortex->get_autonomaus().get_draw_state();
	auto camera = Camera(draw_state);
	auto& input_manager = cortex->get_autonomaus().get_input_manager();

	GesturePointer to_position = std::make_unique<WorldViewportMouseMoveGesture>(
		input_manager, world_screen_position, camera);
	this->motions.emplace_back(*this->cortex, to_position, guard);

	for (int i = 0; i < ticks; ++i)
	{
		GesturePointer scroll = std::make_unique<MousePressGesture>(
			input_manager, eiznebahn::MouseButton::button_down, true);
		this->motions.emplace_back(*this->cortex, scroll, guard);
	}
}

void autonomaus::MotionSequence::drag(
	const chomsky::GUIContent& source,
	const chomsky::GUIContent& destination,
	float speed)
{
	drag(source, destination, speed, [] { return true; });
}

void autonomaus::MotionSequence::drag(
	const chomsky::GUIContent& source,
	const chomsky::GUIContent& destination,
	float speed,
	const Guard& guard)
{
	auto draw_state = cortex->get_autonomaus().get_draw_state();
	auto& input_manager = cortex->get_autonomaus().get_input_manager();
	auto width = draw_state.get_gui_viewport().z;
	auto height = draw_state.get_gui_viewport().w;

	GesturePointer to_source = std::make_unique<GUIMoveToContentGesture>(
		input_manager, source, width, height);
	this->motions.emplace_back(*this->cortex, to_source, guard);

	GesturePointer press = std::make_unique<MousePressGesture>(
		input_manager, eiznebahn::MouseButton::button_left, false);
	this->motions.emplace_back(*this->cortex, press, guard, eiznebahn::MouseButton::button_left);

	GesturePointer to_destination = std::make_unique<GUIMoveToContentGesture>(
		input_manager, destination, width, height, speed);
	this->motions.emplace_back(*this->cortex, to_destination, guard, eiznebahn::MouseButton::button_left);

	GesturePointer release = std::make_unique<MousePressGesture>(
		input_manager, eiznebahn::MouseButton::button_left, true);
	this->motions.emplace_back(*this->cortex, release, guard, eiznebahn::MouseButton::button_left);
}

void autonomaus::MotionSequence::drag(
	const chomsky::GUIContent& content,
	int x, int y,
	float speed)
{
	drag(content, x, y, speed, [] { return true; });
}

void autonomaus::MotionSequence::drag(
	const chomsky::GUIContent& content,
	int x, int y,
	float speed,
	const Guard& guard)
{
	auto draw_state = cortex->get_autonomaus().get_draw_state();
	auto& input_manager = cortex->get_autonomaus().get_input_manager();
	auto width = draw_state.get_gui_viewport().z;
	auto height = draw_state.get_gui_viewport().w;

	int to_x = RNG::next(0, content.get_width()) + content.get_x();
	int to_y = RNG::next(0, content.get_height()) + content.get_y();

	GesturePointer to_source = std::make_unique<GUIMoveToContentGesture>(
		input_manager, to_x, to_y, 1, 1, width, height, speed);
	this->motions.emplace_back(*this->cortex, to_source, guard);

	GesturePointer press = std::make_unique<MousePressGesture>(
		input_manager, eiznebahn::MouseButton::button_left, false);
	this->motions.emplace_back(*this->cortex, press, guard, eiznebahn::MouseButton::button_left);

	GesturePointer to_destination = std::make_unique<GUIMoveToContentGesture>(
		input_manager,
		to_x + x, to_y + y,
		1, 1,
		width, height, speed);
	this->motions.emplace_back(*this->cortex, to_destination, guard, eiznebahn::MouseButton::button_left);

	GesturePointer release = std::make_unique<MousePressGesture>(
		input_manager, eiznebahn::MouseButton::button_left, true);
	this->motions.emplace_back(*this->cortex, release, guard, eiznebahn::MouseButton::button_left);
}

void autonomaus::MotionSequence::drag(
	const glm::vec2& source,
	const glm::vec2& destination,
	int button)
{
	drag(source, destination, button, [] { return true; });
}

void autonomaus::MotionSequence::drag(
	const glm::vec2& source,
	const glm::vec2& destination,
	int button,
	const Guard& guard)
{
	auto draw_state = cortex->get_autonomaus().get_draw_state();
	auto camera = Camera(draw_state);
	auto& input_manager = cortex->get_autonomaus().get_input_manager();

	GesturePointer to_source = std::make_unique<WorldViewportMouseMoveGesture>(
		input_manager, source, camera);
	this->motions.emplace_back(*this->cortex, to_source, guard);

	GesturePointer press = std::make_unique<MousePressGesture>(
		input_manager, button, false);
	this->motions.emplace_back(*this->cortex, press, guard, button);

	GesturePointer to_destination = std::make_unique<WorldViewportMouseMoveGesture>(
		input_manager, destination, camera);
	this->motions.emplace_back(*this->cortex, to_destination, guard, button);

	GesturePointer release = std::make_unique<MousePressGesture>(
		input_manager, button, true);
	this->motions.emplace_back(*this->cortex, release, guard, button);
}

void autonomaus::MotionSequence::type(const std::string& text, bool ignore_case)
{
	return type(text, ignore_case, [] { return true; });
}

void autonomaus::MotionSequence::type(
	const std::string& text,
	bool ignore_case,
	const Guard& guard)
{
	GesturePointer gesture;
	{
		auto& input_manager = cortex->get_autonomaus().get_input_manager();
		gesture = std::make_unique<TypeGesture>(
			input_manager, text, ignore_case);
	}

	this->motions.emplace_back(*this->cortex, gesture, guard);
}

void autonomaus::MotionSequence::wait(float duration, const Guard& guard)
{
	this->motions.emplace_back(duration, guard);
}

void autonomaus::MotionSequence::invoke(const std::function<void()>& function)
{
	this->motions.emplace_back(HUGE_VALF, [function] { function(); return true; });
}

autonomaus::Cortex::Result::Status autonomaus::MotionSequence::update()
{
	if (!pending())
	{
		return this->status;
	}

	auto& motion = this->motions.at(this->current_motion);
	if (!this->is_ready)
	{
		if (!motion.start())
		{
			cancel();
			this->is_valid = false;
			this->status = Cortex::Result::Status::failure;
			return this->status;
		}

		this->is_ready = true;
	}

	if (!motion.update())
	{
		cancel();
		this->is_valid = false;
		this->status = Cortex::Result::Status::failure;
		return this->status;
	}

	if (motion.pending())
	{
		this->status = Cortex::Result::Status::working;
	}
	else
	{
		++this->current_motion;
		if (this->current_motion >= this->motions.size())
		{
			assert(this->current_motion == this->motions.size());
			this->status = Cortex::Result::Status::success;
		}
		else
		{
			this->is_ready = false;
			this->status = Cortex::Result::Status::working;
		}
	}
	
	return this->status;
}

void autonomaus::MotionSequence::cancel()
{
	if (pending())
	{
		auto& motion = this->motions.at(this->current_motion);
		motion.cancel();

		if (motion.button != eiznebahn::MouseButton::button_none)
		{
			auto& spoofer = this->cortex->get_autonomaus().get_input_manager().get_spoofer();
			spoofer.mouse_button_release(motion.button);
		}

		this->is_valid = false;
	}
}

bool autonomaus::MotionSequence::pending() const
{
	return this->is_valid && this->current_motion < this->motions.size();
}

autonomaus::MotionSequence::Motion::Motion(
	Cortex& cortex,
	GesturePointer& pointer,
	const Guard& guard,
	int button)
{
	this->type = type_gesture;
	this->gesture = std::move(pointer);
	this->button = button;
	this->cortex = &cortex;
	this->guard = guard;
}

autonomaus::MotionSequence::Motion::Motion(
	Cortex& cortex,
	const std::string& action,
	const Message& message,
	const Guard& guard)
{
	this->type = type_poke;
	this->cortex = &cortex;
	this->action = action;
	this->message = message;
	this->guard = guard;
}

autonomaus::MotionSequence::Motion::Motion(Cortex::Result::Status status)
{
	this->type = type_poke;
	this->result = Cortex::Result(status);
}

autonomaus::MotionSequence::Motion::Motion(float duration, const Guard& guard)
{
	this->type = type_wait;
	this->duration = duration;
	this->guard = guard;
}

bool autonomaus::MotionSequence::Motion::pending()
{
	if (this->type == type_gesture)
	{
		return gesture->pending();
	}
	else if (this->type == type_poke)
	{
		return result.get() == Cortex::Result::Status::working || result.get() == Cortex::Result::Status::empty;
	}
	else if (this->type == type_wait)
	{
		return !this->guard();
	}

	return false;
}

bool autonomaus::MotionSequence::Motion::start()
{
	if (failed())
	{
		return false;
	}

	if (this->type == type_gesture)
	{
		if (gesture->play() != Gesture::success)
		{
			return false;
		}

		return true;
	}
	else if (this->type == type_poke)
	{
		if (this->cortex != nullptr)
		{
			this->result = this->cortex->poke(this->action, this->message);
		}

		return this->result.get() != Cortex::Result::Status::failure;
	}
	else if (this->type == type_wait)
	{
		this->current_time = get_current_time();

		return true;
	}
	else
	{
		return false;
	}
}

bool autonomaus::MotionSequence::Motion::update()
{
	if (failed())
	{
		return false;
	}

	if (this->type == type_gesture)
	{
		return true;
	}
	else if (this->type == type_poke)
	{
		if (this->result.get() == Cortex::Result::Status::failure)
		{
			return false;
		}

		return true;
	}
	else if (this->type == type_wait)
	{
		if (this->current_time + this->duration > get_current_time())
		{
			return true;
		}

		return false;
	}
	else
	{
		return false;
	}
}

void autonomaus::MotionSequence::Motion::cancel()
{
	if (this->type == type_gesture)
	{
		this->gesture->cancel();
	}
	else if (this->type == type_poke)
	{
		this->result.cancel();
	}
}

const float autonomaus::MotionSequence::Motion::GUARD_WAIT_TIME =
	Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");

bool autonomaus::MotionSequence::Motion::failed()
{
	if (this->type == type_wait)
	{
		return false;
	}

	if (this->failing)
	{
		auto last_update = this->cortex->get_autonomaus().get_game_state().get_last_update_timestamp();

		if (this->guard())
		{
			this->failing = false;
		}
		else if (this->fail_time + GUARD_WAIT_TIME < last_update)
		{
			return true;
		}
	}

	if (!this->failing && !this->guard())
	{
		this->failing = true;
		this->fail_time = get_current_time();
	}

	return false;
}
