// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/core/rng.hpp"
#include "autonomaus/input/mousePathFilter.hpp"
#include "autonomaus/input/gestures/guiClickContentGesture.hpp"
#include "eiznebahn/mouse/mouseMovementProvider.hpp"
#include "eiznebahn/mouse/mouseButtonProvider.hpp"

const int autonomaus::GUIClickContentGesture::MIN = 32;
const int autonomaus::GUIClickContentGesture::MAX = 163;

const std::size_t autonomaus::GUIClickContentGesture::MIN_PATHS = 10;
const float autonomaus::GUIClickContentGesture::DRIFT_THRESHOLD = 16.0f;
const float autonomaus::GUIClickContentGesture::LENGTH_THRESHOLD = 0.25f;

autonomaus::GUIClickContentGesture::GUIClickContentGesture(
	InputManager& input_manager,
	const chomsky::GUIContent& content,
	int window_width, int window_height,
	int button) : Gesture(input_manager)
{
	this->content_x = content.get_x();
	this->content_y = content.get_y();
	this->content_width = content.get_width();
	this->content_height = content.get_height();
	this->window_width = window_width;
	this->window_height = window_height;
	this->button = button;
}

autonomaus::GUIClickContentGesture::GUIClickContentGesture(
	InputManager& input_manager,
	int x, int y,
	int width, int height,
	int window_width, int window_height,
	int button) : Gesture(input_manager)
{
	this->content_x = x;
	this->content_y = y;
	this->content_width = width;
	this->content_height = height;
	this->window_width = window_width;
	this->window_height = window_height;
	this->button = button;
}

int autonomaus::GUIClickContentGesture::try_find_path(
	const glm::vec2& position,
	float& result_length, float& result_direction,
	eiznebahn::MousePath& result_path)
{
	auto& state = get_input_manager().get_state();
	auto mouse_position = glm::vec2(state.get_mouse_x(), state.get_mouse_y());

	auto difference = glm::vec2(position) - mouse_position;
	float length = glm::length(difference);
	float direction = std::atan2(difference.y, difference.x);
	if (direction < 0.0f)
	{
		direction += M_PI * 2.0f;
	}

	MousePathFilter filter;
	{
		auto& paths = get_input_manager().get_paths();
		filter.collect(paths.begin(), paths.end());
	}

	filter.start(MIN_PATHS);
	if (!filter.select(filter.drift(0.0f, DRIFT_THRESHOLD)))
	{
		return error_no_matches;
	}

	filter.select(filter.length(length, LENGTH_THRESHOLD * length));
	filter.direction(direction);
	filter.stop();

	result_length = length;
	result_direction = direction;
	result_path = filter.get(RNG::next(filter.count()));

	return success;
}

int autonomaus::GUIClickContentGesture::play()
{
	int result;

	auto mouse_x = get_input_manager().get_state().get_mouse_x();
	auto mouse_y = get_input_manager().get_state().get_mouse_y();
	mouse_y = this->window_height - mouse_y;

	eiznebahn::MouseButton button;
	button.button = this->button;
	button.delay = RNG::next(MIN, MAX);

	if (mouse_x >= this->content_x &&
		mouse_x < this->content_x + this->content_width &&
		mouse_y >= this->content_y &&
		mouse_y < this->content_y + this->content_height)
	{
		auto& queue = get_input_manager().get_queue();
		auto group = queue.start(eiznebahn::InputQueue::channel_mouse);
		queue.push<eiznebahn::MouseButtonProvider>(button, true);
		queue.stop();

		set_group(group);

		return success;
	}

	// TODO: Randomize.
	glm::vec2 position;
	position.x = this->content_x + this->content_width / 2;
	position.x += RNG::nextf(-(this->content_width / 4), this->content_width / 4);
	position.y = this->content_y + this->content_height / 2;
	position.y += RNG::nextf(-(this->content_height / 4), this->content_height / 4);
	position.y = this->window_height - position.y;

	float length, direction;
	eiznebahn::MousePath path;
	result = try_find_path(position, length, direction, path);
	if (result != success)
	{
		return result;
	}

	path.transform(length, direction);
	{
		auto end_point = path.get(path.count() - 1);
		auto x = std::get<eiznebahn::MousePath::X>(end_point);
		auto y = std::get<eiznebahn::MousePath::Y>(end_point);

		int difference_x = position.x - x;
		int difference_y = position.y - y;

		path.translate(difference_x, difference_y);
	}

	auto& queue = get_input_manager().get_queue();
	auto group = queue.start(eiznebahn::InputQueue::channel_mouse);
	queue.push<eiznebahn::MouseMovementProvider>(path);
	queue.push<eiznebahn::MouseButtonProvider>(button, true);
	queue.stop();

	get_input_manager().preview(path);

	set_group(group);
	return success;
}
