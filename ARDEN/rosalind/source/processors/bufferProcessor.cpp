// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "rosalind/processors/bufferProcessor.hpp"

rosalind::BufferProcessor::BufferProcessor(const touchstone::Context* context)
{
	this->context = context;
}

void rosalind::BufferProcessor::before_update(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::BufferData::token:
			process_buffer_data_before((const touchstone::call::BufferData*)call);
			break;
	}
}

void rosalind::BufferProcessor::after_update(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::GenBuffers::token:
			process_create_buffers_after((const touchstone::call::GenBuffers*)call);
			break;
		case touchstone::call::BufferData::token:
			process_buffer_data_after((const touchstone::call::BufferData*)call);
			break;
		case touchstone::call::BufferSubData::token:
			process_buffer_data_after((const touchstone::call::BufferSubData*)call);
			break;
		case touchstone::call::DeleteBuffers::token:
			process_delete_buffers_after((const touchstone::call::DeleteBuffers*)call);
			break;
	}
}

void rosalind::BufferProcessor::process_create_buffers_after(const touchstone::call::GenBuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		kompvter::BufferEvent event;
		event.set_name(call->get_parameter_buffers()[i]);

		notify(std::mem_fn(&kompvter::BufferListener::on_buffer_create), event);
	}
}

void rosalind::BufferProcessor::process_buffer_data_before(const touchstone::call::BufferData* call)
{
	auto& buffer = this->context->get_current_resource<touchstone::BufferResource>(call->get_parameter_target());
	if (buffer.get_size() != call->get_size())
	{
		kompvter::BufferEvent event;
		event.set_name(buffer.get_name());
		event.set_data_size(call->get_parameter_size());

		notify(std::mem_fn(&kompvter::BufferListener::on_buffer_allocate), event);
	}
}

void rosalind::BufferProcessor::process_buffer_data_after(const touchstone::call::BufferData* call)
{
	kompvter::BufferEvent event;
	auto& buffer = this->context->get_current_resource<touchstone::BufferResource>(call->get_parameter_target());
	event.set_name(buffer.get_name());
	event.set_data_size(call->get_parameter_size());

	if (call->get_parameter_data() == nullptr)
	{
		notify(std::mem_fn(&kompvter::BufferListener::on_buffer_clear), event);
	}
	else
	{
		event.set_data((const std::uint8_t*)call->get_parameter_data());
		notify(std::mem_fn(&kompvter::BufferListener::on_buffer_update), event);
	}
}

void rosalind::BufferProcessor::process_buffer_data_after(const touchstone::call::BufferSubData* call)
{
	kompvter::BufferEvent event;
	auto& buffer = this->context->get_current_resource<touchstone::BufferResource>(call->get_parameter_target());
	event.set_name(buffer.get_name());
	event.set_offset(call->get_parameter_offset());
	event.set_data_size(call->get_parameter_size());

	event.set_data((const std::uint8_t*)call->get_parameter_data());
	notify(std::mem_fn(&kompvter::BufferListener::on_buffer_update), event);
}

void rosalind::BufferProcessor::process_delete_buffers_after(const touchstone::call::DeleteBuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		kompvter::BufferEvent event;
		event.set_name(call->get_parameter_buffers()[i]);

		notify(std::mem_fn(&kompvter::BufferListener::on_buffer_delete), event);
	}
}
