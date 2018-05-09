// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <chrono>
#include "rosalind/common/updater.hpp"

rosalind::Updater::Updater(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager) :
		buffer_processor(&context),
		texture_processor(&context),
		gui_processor(&context, &buffer_manager, &texture_manager),
		item_icon_processor(&context, &buffer_manager, &texture_manager),
		minimap_processor(&context, &buffer_manager, &texture_manager),
		model_processor(&context, &buffer_manager, &texture_manager)
{
	this->buffer_processor.add(&buffer_manager);
	this->texture_processor.add(&texture_manager);
}

void rosalind::Updater::update(const touchstone::Trace& trace, float timestamp)
{
	this->frame_processor.on_frame(timestamp);
	this->buffer_processor.start_frame();
	this->texture_processor.start_frame();
	this->gui_processor.start_frame();
	this->item_icon_processor.start_frame();
	this->minimap_processor.start_frame();
	this->model_processor.start_frame();

	for (auto call: trace)
	{
		this->buffer_processor.before_update(call);
		this->texture_processor.before_update(call);
		this->gui_processor.before_update(call);
		this->item_icon_processor.before_update(call);
		this->minimap_processor.before_update(call);
		this->model_processor.before_update(call);

		this->context.update(call);

		this->buffer_processor.after_update(call);
		this->texture_processor.after_update(call);
		this->gui_processor.after_update(call);
		this->item_icon_processor.after_update(call);
		this->minimap_processor.after_update(call);
		this->model_processor.after_update(call);
	}
}

void rosalind::Updater::enable_draw(float timestamp)
{
	this->frame_processor.enable_draw(timestamp);
}

void rosalind::Updater::disable_draw(float timestamp)
{
	this->frame_processor.disable_draw(timestamp);
}

rosalind::BufferProcessor& rosalind::Updater::get_buffer_processor()
{
	return this->buffer_processor;
}

const rosalind::BufferProcessor& rosalind::Updater::get_buffer_processor() const
{
	return this->buffer_processor;
}

rosalind::TextureProcessor& rosalind::Updater::get_texture_processor()
{
	return this->texture_processor;
}

const rosalind::TextureProcessor& rosalind::Updater::get_texture_processor() const
{
	return this->texture_processor;
}

rosalind::FrameProcessor& rosalind::Updater::get_frame_processor()
{
	return this->frame_processor;
}

const rosalind::FrameProcessor& rosalind::Updater::get_frame_processor() const
{
	return this->frame_processor;
}

rosalind::GUIProcessor& rosalind::Updater::get_gui_processor()
{
	return this->gui_processor;
}

const rosalind::GUIProcessor& rosalind::Updater::get_gui_processor() const
{
	return this->gui_processor;
}

rosalind::MinimapProcessor& rosalind::Updater::get_minimap_processor()
{
	return this->minimap_processor;
}

const rosalind::MinimapProcessor& rosalind::Updater::get_minimap_processor() const
{
	return this->minimap_processor;
}

rosalind::ModelProcessor& rosalind::Updater::get_model_processor()
{
	return this->model_processor;
}

const rosalind::ModelProcessor& rosalind::Updater::get_model_processor() const
{
	return this->model_processor;
}

rosalind::ItemIconProcessor& rosalind::Updater::get_item_icon_processor()
{
	return this->item_icon_processor;
}

const rosalind::ItemIconProcessor& rosalind::Updater::get_item_icon_processor() const
{
	return this->item_icon_processor;
}
