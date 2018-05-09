// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_JAQUES_FRAME_READER_HPP
#define PRINCIPLE_COMMON_JAQUES_FRAME_READER_HPP

#include <string>
#include <fstream>
#include "kvncer/graphics/texture.hpp"
#include "touchstone/trace/trace.hpp"

namespace principle
{
	class JaquesFrameReader
	{
	public:
		JaquesFrameReader(const std::string& filename);
		~JaquesFrameReader() = default;

		bool read(std::size_t frame_num, touchstone::Trace& trace);
		kvncer::Texture* load_screenshot(std::size_t frame_num);

		std::size_t count() const;

	private:
		std::ifstream trace_file_input;
		std::string current_frame_data;

		struct Frame
		{
			std::uint64_t trace_offset;
			std::uint64_t compressed_trace_size;
			std::uint64_t uncompressed_trace_size;
			std::uint64_t screenshot_offset;
			std::uint64_t screenshot_size;
		};
		std::vector<Frame> frames;
	};
}

#endif
