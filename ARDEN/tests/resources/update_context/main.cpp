// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <chrono>
#include "test/jaquesFrameReader.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/call.hpp"

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		std::fprintf(stderr, "%s <trace>\n", argv[0]);

		return 1;
	}

	std::setbuf(stdout, NULL);

	test::JaquesFrameReader reader(argv[1]);
	touchstone::GLTrace gl_trace;
	touchstone::Context context;
	std::size_t current_frame = 1;
	while (reader.read(gl_trace))
	{
		auto start = std::chrono::system_clock::now();
		for (auto call: gl_trace)
		{
			context.update(call);
		}
		auto stop = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = stop - start;

		std::printf("finished frame %lu in %.4lf ms\n", current_frame, diff.count() * 1000);

		++current_frame;
		gl_trace.clear();
	}

	std::printf("done!\n");

	return 0;
}
