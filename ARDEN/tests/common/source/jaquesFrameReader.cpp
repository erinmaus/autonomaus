// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdint>
#include <snappy.h>
#include "test/jaquesFrameReader.hpp"
#include "touchstone/trace/callStream.hpp"

test::JaquesFrameReader::JaquesFrameReader(const std::string& filename) :
	trace_file_input(filename)
{
	// Nothing.
}

bool test::JaquesFrameReader::read(touchstone::Trace& trace)
{
	std::string input_frame;
	std::uint64_t uncompressed_frame_size, compressed_frame_size;

	this->trace_file_input.read(
		(char*)&uncompressed_frame_size, sizeof(std::uint64_t));
	this->trace_file_input.read(
		(char*)&compressed_frame_size, sizeof(std::uint64_t));

	if (!this->trace_file_input)
	{
		return false;
	}

	input_frame.resize(compressed_frame_size);
	this->trace_file_input.read(&input_frame[0], compressed_frame_size);

	if (!this->trace_file_input)
	{
		return false;
	}

	this->current_frame.clear();
	snappy::Uncompress(
		input_frame.data(), input_frame.size(), &this->current_frame);

	touchstone::CallStream call_stream(
		(const std::uint8_t*)this->current_frame.data(),
		this->current_frame.size());
	trace.extract(call_stream);

	std::uint64_t screenshot_size;
	this->trace_file_input.read((char*)&screenshot_size, sizeof(std::uint64_t));

	this->current_screenshot.resize(screenshot_size);
	this->trace_file_input.read(&this->current_screenshot[0], screenshot_size);

	return true;
}

void test::JaquesFrameReader::save_screenshot(const std::string& filename)
{
	std::ofstream output(filename, std::ios::out | std::ios::binary);
	output.write(
		this->current_screenshot.data(),
		this->current_screenshot.size());
}
