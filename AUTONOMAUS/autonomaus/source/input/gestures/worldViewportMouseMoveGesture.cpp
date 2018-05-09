// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/core/rng.hpp"
#include "autonomaus/input/mousePathFilter.hpp"
#include "autonomaus/input/gestures/worldViewportMouseMoveGesture.hpp"
#include "eiznebahn/mouse/mouseMovementProvider.hpp"

const std::size_t autonomaus::WorldViewportMouseMoveGesture::MIN_PATHS = 10;
const float autonomaus::WorldViewportMouseMoveGesture::DRIFT_THRESHOLD = 1.0f;
const float autonomaus::WorldViewportMouseMoveGesture::LENGTH_THRESHOLD = 0.25f;

autonomaus::WorldViewportMouseMoveGesture::WorldViewportMouseMoveGesture(
	InputManager& input_manager,
	const glm::vec2& position,
	const Camera& camera) : Gesture(input_manager)
{
	this->position = position;
	this->camera = camera;
}

int autonomaus::WorldViewportMouseMoveGesture::try_find_path(
	const glm::vec2& position,
	float& result_length, float& result_direction,
	eiznebahn::MousePath& result_path)
{
	if (!this->camera.is_visible(this->position))
	{
		return error_outside_viewport;
	}

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

int autonomaus::WorldViewportMouseMoveGesture::play()
{
	int result;

	float length, direction;
	eiznebahn::MousePath path;
	result = try_find_path(this->position, length, direction, path);
	if (result != success)
	{
		return result;
	}

	path.transform(length, direction);
	{
		auto end_point = path.get(path.count() - 1);
		auto x = std::get<eiznebahn::MousePath::X>(end_point);
		auto y = std::get<eiznebahn::MousePath::Y>(end_point);

		int difference_x = this->position.x - x;
		int difference_y = this->position.y - y;

		path.translate(difference_x, difference_y);
	}

	auto& queue = get_input_manager().get_queue();
	auto group = queue.start(eiznebahn::InputQueue::channel_mouse);
	queue.push<eiznebahn::MouseMovementProvider>(path);
	queue.stop();

	get_input_manager().preview(path);

	set_group(group);
	return success;
}
