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
	if (argc != 2)
	{
		std::fprintf(stderr, "%s <trace>\n", argv[0]);

		return 1;
	}

	test::JaquesFrameReader reader(argv[1]);
	touchstone::GLTrace gl_trace;
	std::size_t current_frame = 1;
	while (reader.read(gl_trace))
	{
		std::printf("frame %lu:\n", current_frame);

		for (auto call: gl_trace)
		{
			std::printf(
				"\tcall %lu: %s (%lu bytes)\n",
				call->get_call_num(), call->get_name(), call->get_size());
		}

		++current_frame;
		gl_trace.clear();
	}

	std::printf("done!\n");

	return 0;
}
