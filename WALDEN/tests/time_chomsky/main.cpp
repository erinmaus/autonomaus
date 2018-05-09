// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <chrono>
#include "kommon/deps/ctpl/ctpl.h"
#include "kommon/io/jaquesFrameReader.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/call.hpp"
#include "rosalind/processors/bufferProcessor.hpp"
#include "rosalind/processors/frameProcessor.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "rosalind/processors/modelProcessor.hpp"
#include "rosalind/processors/textureProcessor.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/mapWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/utilityWorldUpdateBlock.hpp"

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		std::fprintf(stderr, "%s <trace>\n", argv[0]);

		return 1;
	}

	std::setbuf(stdout, NULL);

	ctpl::thread_pool thread_pool(2);

	kommon::JaquesFrameReader reader(argv[1]);
	touchstone::GLTrace gl_trace;
	touchstone::Context context;

	kompvter::BufferManager buffer_manager;
	rosalind::BufferProcessor buffer_processor(&context);
	buffer_processor.add(&buffer_manager);

	kompvter::TextureManager texture_manager;
	rosalind::TextureProcessor texture_processor(&context);
	texture_processor.add(&texture_manager);

	rosalind::GUIProcessor gui_processor(&context, &buffer_manager, &texture_manager);
	rosalind::ItemIconProcessor item_icon_processor(&context, &buffer_manager, &texture_manager);
	rosalind::MinimapProcessor minimap_processor(&context, &buffer_manager, &texture_manager);
	rosalind::ModelProcessor model_processor(&context, &buffer_manager, &texture_manager);
	rosalind::FrameProcessor frame_processor;

	thoreau::Map map;
	chomsky::WorldUpdateManager world_update_manager;
	chomsky::UtilityWorldUpdateBlock utility_block(world_update_manager);
	chomsky::MapWorldUpdateBlock map_block(
		world_update_manager,
		&map,
		utility_block.get_transform_state(),
		utility_block.get_bounds());
	chomsky::AnimatedWorldUpdateBlock animated_block(
		world_update_manager,
		map_block.get_tile_elevation_state());
	chomsky::StaticWorldUpdateBlock static_block(
		world_update_manager,
		utility_block.get_transform_state(),
		map_block.get_tile_elevation_state());

	buffer_processor.add(&world_update_manager);
	model_processor.add(&world_update_manager);
	frame_processor.add(&world_update_manager);

	std::size_t current_frame = 0;
	while (reader.read(current_frame, gl_trace))
	{
		auto start = std::chrono::system_clock::now();
		frame_processor.on_frame(current_frame);
		frame_processor.enable_draw(current_frame);
		for (auto call: gl_trace)
		{
			buffer_processor.before_update(call);
			texture_processor.before_update(call);
			gui_processor.before_update(call);
			item_icon_processor.before_update(call);
			model_processor.before_update(call);
			minimap_processor.before_update(call);

			context.update(call);

			buffer_processor.after_update(call);
			texture_processor.after_update(call);
			gui_processor.after_update(call);
			item_icon_processor.after_update(call);
			model_processor.after_update(call);
			minimap_processor.after_update(call);
		}
		frame_processor.disable_draw(current_frame);

		world_update_manager.begin_update();
		utility_block.update();
		map_block.update();
		auto a = thread_pool.push([&static_block](auto) { static_block.update(); });
		auto b = thread_pool.push([&animated_block](auto) { animated_block.update(); });
		a.wait();
		b.wait();
		world_update_manager.end_update();

		auto stop = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = stop - start;

		std::printf("finished frame %lu in %.4lf ms\n", current_frame, diff.count() * 1000);

		++current_frame;
		gl_trace.clear();
	}

	std::printf("done!\n");

	return 0;
}
