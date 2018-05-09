// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/mouse/mouseMovementProvider.hpp"

eiznebahn::MouseMovementProvider::MouseMovementProvider(const MousePath& path) :
	path(path)
{
	if (path.empty())
	{
		this->pending = false;
	}
	else
	{
		this->pending = true;
		this->target_time = 0.0f;
	}
}

void eiznebahn::MouseMovementProvider::update(float delta, InputSpoofer& spoofer)
{
	assert(!done());

	this->current_time += delta;
	while (this->current_time >= this->target_time)
	{
		++this->current_index;
		if (this->current_index >= this->path.count())
		{
			assert(this->current_index == this->path.count());
			this->pending = false;
			break;
		}

		auto next = this->path.get(this->current_index);
		auto x = std::get<MousePath::X>(next);
		auto y = std::get<MousePath::Y>(next);
		spoofer.mouse_move(x, y);

		this->target_time += std::get<MousePath::DELAY>(next);
	}
}

void eiznebahn::MouseMovementProvider::cancel()
{
	this->pending = false;
}

bool eiznebahn::MouseMovementProvider::done() const
{
	return !this->pending;
}
