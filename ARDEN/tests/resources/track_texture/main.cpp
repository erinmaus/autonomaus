// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <boost/container/flat_set.hpp>
#include <vector>
#include <stack>
#include "test/jaquesFrameReader.hpp"
#include "touchstone/detail/cow.hpp"
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/gl/resources/dependency.hpp"
#include "touchstone/gl/resources/textureResource.hpp"
#include "touchstone/gl/resources/textureResourceManager.hpp"
#include "touchstone/trace/discreteCall.hpp"
#include "touchstone/trace/discreteCallSet.hpp"

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
		std::printf("frame %lu:\n", current_frame);
		++current_frame;

		for (auto call: gl_trace)
		{
			context.update(call);
		}

		gl_trace.clear();

		if (current_frame > 1950)
		{
			break;
		}

		if (current_frame != 1900)
		{
			continue;
		}

		bool has_textures = false;
		auto& texture_resource_manager = context.get_resource_manager<touchstone::TextureResource>();
		for (auto& texture: texture_resource_manager)
		{
			touchstone::DiscreteCallSet call_set;
			touchstone::Dependency::walk(texture.second, call_set);

			std::printf("\tframe texture %u:\n", texture.second.get_name());
			for (auto call: call_set)
			{
				std::printf(
					"\t\tcall %lu: %s\n",
					call->get_call_num(), call->get_name());
			}

			has_textures = true;
		}

		if (!has_textures)
		{
			std::printf("\t(no textures)\n");
		}

		std::fflush(stdout);
	}

	std::printf("done!\n");

	return 0;
}
