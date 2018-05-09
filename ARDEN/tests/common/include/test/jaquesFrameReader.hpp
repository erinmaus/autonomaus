// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TEST_COMMON_JAQUES_FRAME_READER_HPP
#define TEST_COMMON_JAQUES_FRAME_READER_HPP

#include <string>
#include <fstream>
#include "touchstone/trace/trace.hpp"

namespace test
{
	class JaquesFrameReader
	{
	public:
		JaquesFrameReader(const std::string& filename);
		~JaquesFrameReader() = default;

		bool read(touchstone::Trace& trace);

		void save_screenshot(const std::string& filename);

	private:
		std::ifstream trace_file_input;
		std::string current_frame;
		std::string current_screenshot;
	};
}

#endif
