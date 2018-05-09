// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <chrono>
#include "kompvter/events/bufferEvent.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/events/minimapDrawEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/events/viewportEvent.hpp"
#include "kompvter/listeners/bufferListener.hpp"
#include "kompvter/listeners/guiListener.hpp"
#include "kompvter/listeners/itemIconListener.hpp"
#include "kompvter/listeners/minimapListener.hpp"
#include "kompvter/listeners/modelListener.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "test/jaquesFrameReader.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/call.hpp"
#include "rosalind/processors/bufferProcessor.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "rosalind/processors/modelProcessor.hpp"
#include "rosalind/processors/textureProcessor.hpp"

class BufferListener : public kompvter::BufferListener
{
public:
	void on_buffer_create(const kompvter::BufferEvent& event) override
	{
		std::printf("\tBufferListener::on_buffer_create\n");
	}

	void on_buffer_delete(const kompvter::BufferEvent& event) override
	{
		std::printf("\tBufferListener::on_buffer_delete\n");
	}
};

class GUIListener : public kompvter::GUIListener
{
public:
	void on_draw_gui(const kompvter::GUIDrawEvent& event) override
	{
		std::printf("\tGUIListener::on_draw_gui\n");
	}

	void on_present() override
	{
		std::printf("\tGUIListener::on_present\n");
	}

	void on_set_viewport(const kompvter::ViewportEvent&) override
	{
		std::printf("\tGUIListener::on_set_viewport\n");
	}
};

class ItemIconListener : public kompvter::ItemIconListener
{
public:
	void on_draw_gui(const kompvter::GUIDrawEvent& event) override
	{
		std::printf("\tItemIconListener::on_draw_gui\n");
	}

	void on_draw_model(const kompvter::ModelDrawEvent& event) override
	{
		std::printf("\tItemIconListener::on_draw_model\n");
	}

	void on_transfer(const kompvter::ItemIconEvent& event) override
	{
		std::printf("\tItemIconListener::on_transfer\n");
	}

	void on_clear(const kompvter::ItemIconEvent& event) override
	{
		std::printf("\tItemIconListener::on_clear\n");
	}
};

class MinimapListener : public kompvter::MinimapListener
{
public:
	void on_draw_minimap(const kompvter::MinimapDrawEvent& event) override
	{
		std::printf("\tMinimapListener::on_draw_minimap\n");
	}
};

class ModelListener : public kompvter::ModelListener
{
public:
	void on_draw_model(const kompvter::ModelDrawEvent& event) override
	{
		std::printf("\tModelListener::on_draw_model\n");
	}

	void on_draw_terrain(const kompvter::ModelDrawEvent& event) override
	{
		std::printf("\tModelListener::on_draw_terrain\n");
	}

	void on_draw_water(const kompvter::ModelDrawEvent& event) override
	{
		std::printf("\tModelListener::on_draw_water\n");
	}
};

class TextureListener : public kompvter::TextureListener
{
public:
	void on_texture_create(const kompvter::TextureEvent& event) override
	{
		std::printf("\tTextureListener::on_texture_create\n");
	}

	void on_texture_delete(const kompvter::TextureEvent& event) override
	{
		std::printf("\tTextureListener::on_texture_delete\n");
	}
};

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

	BufferListener buffer_listener;
	kompvter::BufferManager buffer_manager;
	rosalind::BufferProcessor buffer_processor(&context);
	buffer_processor.add(&buffer_listener);
	buffer_processor.add(&buffer_manager);

	TextureListener texture_listener;
	kompvter::TextureManager texture_manager;
	rosalind::TextureProcessor texture_processor(&context);
	texture_processor.add(&texture_listener);
	texture_processor.add(&texture_manager);

	GUIListener gui_listener;
	rosalind::GUIProcessor gui_processor(&context, &buffer_manager, &texture_manager);
	gui_processor.add(&gui_listener);

	ItemIconListener item_icon_listener;
	rosalind::ItemIconProcessor item_icon_processor(&context, &buffer_manager, &texture_manager);
	item_icon_processor.add(&item_icon_listener);

	MinimapListener minimap_listener;
	rosalind::MinimapProcessor minimap_processor(&context, &buffer_manager, &texture_manager);
	minimap_processor.add(&minimap_listener);

	ModelListener model_listener;
	rosalind::ModelProcessor model_processor(&context, &buffer_manager, &texture_manager);
	model_processor.add(&model_listener);

	std::size_t current_frame = 1;
	while (reader.read(gl_trace))
	{
		auto start = std::chrono::system_clock::now();
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
		auto stop = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = stop - start;

		std::printf("finished frame %lu in %.4lf ms\n", current_frame, diff.count() * 1000);

		++current_frame;
		gl_trace.clear();
	}

	std::printf("done!\n");

	return 0;
}
