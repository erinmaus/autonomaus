// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include "test/jaquesFrameReader.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/call.hpp"

int main(int argc, const char* argv[])
{
	if (argc != 4)
	{
		std::fprintf(stderr, "%s <trace> <frame> <filename>\n", argv[0]);

		return 1;
	}

	std::size_t target_frame = atoll(argv[2]);

	printf("seeking to frame %lu...\n", target_frame);

	test::JaquesFrameReader reader(argv[1]);
	touchstone::GLTrace gl_trace;
	std::size_t current_frame = 1;
	while (reader.read(gl_trace) && current_frame < target_frame)
	{
		++current_frame;
		if (current_frame % 1000 == 0)
		{
			printf("processed %lu frames...\n", current_frame);
		}

		gl_trace.clear();
	}

	reader.save_screenshot(argv[3]);
	std::printf("saved frame %lu to '%s'\n", current_frame, argv[3]);

	return 0;
}
