// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdint>
#include <snappy.h>
#include <turbojpeg.h>
#include "principle/common/jaquesFrameReader.hpp"
#include "touchstone/trace/callStream.hpp"

principle::JaquesFrameReader::JaquesFrameReader(
	const std::string& filename) :
		trace_file_input(filename)
{
	while (!this->trace_file_input.eof())
	{
		Frame frame;
		this->trace_file_input.read(
			(char*)&frame.uncompressed_trace_size, sizeof(std::uint64_t));
		this->trace_file_input.read(
			(char*)&frame.compressed_trace_size, sizeof(std::uint64_t));

		frame.trace_offset = this->trace_file_input.tellg();

		this->trace_file_input.seekg(
			frame.compressed_trace_size, std::ios::cur);

		this->trace_file_input.read(
			(char*)&frame.screenshot_size, sizeof(std::uint64_t));

		frame.screenshot_offset = this->trace_file_input.tellg();

		if (!this->trace_file_input)
		{
			break;
		}

		this->trace_file_input.seekg(
			frame.screenshot_size, std::ios::cur);

		this->frames.push_back(frame);
	}
}

bool principle::JaquesFrameReader::read(
	std::size_t frame_num, touchstone::Trace& trace)
{
	this->trace_file_input.clear();

	auto& frame = this->frames.at(frame_num);
	this->trace_file_input.seekg(frame.trace_offset);

	std::string input_frame_data;
	input_frame_data.resize(frame.compressed_trace_size);
	this->trace_file_input.read(&input_frame_data[0], frame.compressed_trace_size);

	if (!this->trace_file_input)
	{
		return false;
	}

	this->current_frame_data.clear();
	snappy::Uncompress(
		input_frame_data.data(), input_frame_data.size(),
		&this->current_frame_data);

	touchstone::CallStream call_stream(
		(const std::uint8_t*)this->current_frame_data.data(),
		this->current_frame_data.size());
	trace.extract(call_stream);

	return true;
}

kvncer::Texture* principle::JaquesFrameReader::load_screenshot(
	std::size_t frame_num)
{
	this->trace_file_input.clear();

	auto& frame = this->frames.at(frame_num);
	this->trace_file_input.seekg(frame.screenshot_offset);

	std::string screenshot_data;
	screenshot_data.resize(frame.screenshot_size);

	this->trace_file_input.read(
		(char*)&screenshot_data[0], frame.screenshot_size);
	if (!this->trace_file_input)
	{
		return nullptr;
	}

	int width, height, subsampling;
	tjhandle decompressor = tjInitDecompress();
	tjDecompressHeader2(
		decompressor,
		(unsigned char*)&screenshot_data[0], screenshot_data.size(),
		&width, &height, &subsampling);

	std::string screenshot_pixels;
	screenshot_pixels.resize(width * height * 4);

	tjDecompress2(
		decompressor,
		(unsigned char*)screenshot_data.data(), screenshot_data.size(),
		(unsigned char*)&screenshot_pixels[0],
		width, 0, height,
		TJPF_RGBA, TJFLAG_FASTDCT);

	tjDestroy(decompressor);

	auto texture = new kvncer::Texture(
		kvncer::Texture::component_format_rgba,
		false,
		width, height);
	texture->update(
			0, 0, width, height,
			kvncer::Texture::component_format_rgba,
			kvncer::Texture::component_type_unsigned_byte,
			(const std::uint8_t*)screenshot_pixels.data());

	return texture;
}

std::size_t principle::JaquesFrameReader::count() const
{
	return this->frames.size();
}
