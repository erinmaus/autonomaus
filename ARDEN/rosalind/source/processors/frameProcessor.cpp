// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "rosalind/processors/frameProcessor.hpp"

void rosalind::FrameProcessor::on_frame(float timestamp)
{
	notify(std::mem_fn(&kompvter::FrameListener::on_draw_frame), timestamp);
}

void rosalind::FrameProcessor::enable_draw(float timestamp)
{
	assert(!this->draw_enabled);
	this->draw_enabled = true;
	notify(std::mem_fn(&kompvter::FrameListener::on_draw_enabled), timestamp);
}

void rosalind::FrameProcessor::disable_draw(float timestamp)
{
	assert(this->draw_enabled);
	this->draw_enabled = false;
	notify(std::mem_fn(&kompvter::FrameListener::on_draw_disabled), timestamp);
}
