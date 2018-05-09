// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <glm/glm.hpp>
#include "eiznebahn/mouse/mousePath.hpp"

void eiznebahn::MousePath::add(int x, int y, float delay)
{
	this->points.emplace_back(x, y, delay);
	this->duration += delay;
	this->is_dirty = true;
}

eiznebahn::MousePath::Point eiznebahn::MousePath::get(std::size_t index) const
{
	return this->points.at(index);
}

bool eiznebahn::MousePath::empty() const
{
	return this->points.empty();
}

std::size_t eiznebahn::MousePath::count() const
{
	return this->points.size();
}

int eiznebahn::MousePath::get_length() const
{
	if (count() < 2)
	{
		return 0.0f;
	}

	auto start = this->points.front();
	float start_x = std::get<X>(start);
	float start_y = std::get<Y>(start);

	auto stop = this->points.back();
	float stop_x = std::get<X>(stop);
	float stop_y = std::get<Y>(stop);

	float difference_x = stop_x - start_x;
	float difference_y = stop_y - start_y;
	float difference_x_squared = difference_x * difference_x;
	float difference_y_squared = difference_y * difference_y;
	float value = std::sqrt(difference_x_squared + difference_y_squared);

	return value;
}

float eiznebahn::MousePath::get_direction() const
{
	if (count() < 2)
	{
		return 0.0f;
	}

	auto start = this->points.front();
	float start_x = std::get<X>(start);
	float start_y = std::get<Y>(start);

	auto stop = this->points.back();
	float stop_x = std::get<X>(stop);
	float stop_y = std::get<Y>(stop);

	float difference_x = stop_x - start_x;
	float difference_y = stop_y - start_y;
	float value = std::atan2(difference_y, difference_x);
	if (value < 0.0f)
	{
		value += M_PI * 2.0f;
	}

	return value;
}

float eiznebahn::MousePath::get_duration() const
{
	return this->duration;
}

float eiznebahn::MousePath::get_drift() const
{
	if (this->is_dirty)
	{
		refresh();
		this->is_dirty = false;
	}

	return this->drift;
}

void eiznebahn::MousePath::transform(float length, float direction)
{
	if (count() < 2)
	{
		return;
	}

	float scale = length / get_length();
	float rotation = direction - get_direction();

	this->duration = 0.0f;
	this->is_dirty = true;
	for (std::size_t i = 0; i < count(); ++i)
	{
		auto& point = this->points[i];
		float x = std::get<X>(point);
		float y = std::get<Y>(point);
		float delay = std::get<DELAY>(point);

		float result_x = x * std::cos(rotation) - y * std::sin(rotation);
		result_x *= scale;
		result_x = std::floor(result_x + 0.5f);
		float result_y = y * std::cos(rotation) + x * std::sin(rotation);
		result_y *= scale;
		result_y = std::floor(result_y + 0.5f);

		delay *= scale;

		this->points[i] = std::make_tuple((int)result_x, (int)result_y, delay);
		this->duration += delay;
	}
}

void eiznebahn::MousePath::translate(int x, int y)
{
	for (std::size_t i = 0; i < this->points.size(); ++i)
	{
		auto point = this->points[i];
		int translated_x = std::get<X>(point) + x;
		int translated_y = std::get<Y>(point) + y;
		float delay = std::get<DELAY>(point);
		this->points[i] = std::make_tuple(translated_x, translated_y, delay);
	}
}

void eiznebahn::MousePath::time(float multiplier)
{
	for (std::size_t i = 0; i < this->points.size(); ++i)
	{
		auto point = this->points[i];
		int x = std::get<X>(point);
		int y = std::get<Y>(point);
		float delay = std::get<DELAY>(point) * multiplier;
		this->points[i] = std::make_tuple(x, y, delay);
	}
}

eiznebahn::MousePath::const_iterator eiznebahn::MousePath::begin() const
{
	return this->points.begin();
}

eiznebahn::MousePath::const_iterator eiznebahn::MousePath::end() const
{
	return this->points.end();
}

void eiznebahn::MousePath::refresh() const
{
	this->drift = 0.0f;
	if (count() < 2)
	{
		return;
	}

	auto start = this->points.front();
	auto stop = this->points.back();
	auto a = glm::vec2(std::get<X>(start), std::get<Y>(start));
	auto b = glm::vec2(std::get<X>(stop), std::get<Y>(stop));
	auto length = glm::length(a - b);
	auto length_squared = length * length;

	for (auto& point: this->points)
	{
		auto c = glm::vec2(std::get<X>(point), std::get<Y>(point));
		float t = glm::dot(c - a, b - a) / length_squared;
		t = glm::clamp(t, 0.0f, 1.0f);
		auto closest = a + t * (b - a);
		this->drift += glm::length(c - closest);
	}
}
