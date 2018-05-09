// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMMON_IO_JAQUES_FRAME_READER_HPP
#define KOMMON_IO_JAQUES_FRAME_READER_HPP

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include "touchstone/trace/trace.hpp"

namespace kommon
{
	class JaquesFrameReader
	{
	public:
		JaquesFrameReader(const std::string& filename);
		~JaquesFrameReader() = default;

		bool read(std::size_t frame_num, touchstone::Trace& trace);
		std::vector<std::uint8_t> load_screenshot(
			std::size_t frame_num,
			int& width, int& height);

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
