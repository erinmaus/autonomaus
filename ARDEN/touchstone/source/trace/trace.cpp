// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/trace/call.hpp"
#include "touchstone/trace/callStream.hpp"
#include "touchstone/trace/trace.hpp"

touchstone::Trace::~Trace()
{
	clear();
}

void touchstone::Trace::extract(CallStream stream)
{
	while (!stream.is_end_of_stream())
	{
		auto call = read_call(++this->current_call, stream);
		this->calls.push_back(call);
	}
}

std::size_t touchstone::Trace::extract_single_call(
	CallStream stream, std::size_t offset)
{
	stream.seek(offset);
	auto call = read_call(++this->current_call, stream);
	this->calls.push_back(call);

	return stream.tell();
}

void touchstone::Trace::clear()
{
	for (auto call: this->calls)
	{
		delete call;
	}

	this->calls.clear();
}

std::size_t touchstone::Trace::count() const
{
	return this->calls.size();
}

const touchstone::Call* touchstone::Trace::at(std::size_t index) const
{
	return this->calls.at(index);
}

touchstone::Trace::const_iterator touchstone::Trace::begin() const
{
	return this->calls.begin();
}

touchstone::Trace::const_iterator touchstone::Trace::end() const
{
	return this->calls.end();
}
