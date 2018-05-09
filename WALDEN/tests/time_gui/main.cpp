#include <cstdio>
#include <fstream>
#include <valgrind/callgrind.h>
#include "kommon/io/jaquesFrameReader.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/gpd/compiler.hpp"
#include "rosalind/common/updater.hpp"
#include "glooper/resources/resourceManager.hpp"

static void add_compiler_input(
	chomsky::gpd::Compiler& compiler,
	const std::string& filename)
{
	std::ifstream stream(filename);
	std::string source = std::string(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>());

	compiler.add_input(filename, source);
}

struct GUI :
	public kompvter::FrameListener,
	public kompvter::GUIListener
{
public:
	GUI(
		kompvter::TextureManager& texture_manager,
		chomsky::TextureKeyManager& key_manager);

	void on_draw_enabled(float timestamp) override;
	void on_draw_disabled(float timestamp) override;
	void on_draw_gui(const kompvter::GUIDrawEvent& event) override;

	bool draw_enabled = false;
	chomsky::GUIElementStream element_stream;
};

GUI::GUI(
	kompvter::TextureManager& texture_manager,
	chomsky::TextureKeyManager& key_manager) :
		element_stream(texture_manager, key_manager)
{
}

void GUI::on_draw_enabled(float timestamp)
{
	draw_enabled = true;
}

void GUI::on_draw_disabled(float timestamp)
{
	draw_enabled = false;
}

void GUI::on_draw_gui(const kompvter::GUIDrawEvent& event)
{
	if (event.get_framebuffer() != 0)
	{
		return;
	}

	CALLGRIND_START_INSTRUMENTATION;
	kvre::FlatDrawBuffer flat_draw_buffer(event);
	element_stream.update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);
	CALLGRIND_STOP_INSTRUMENTATION;
}

#include <unistd.h>
static float get_current_time()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return t.tv_sec + t.tv_nsec / 1000000000.0f;
}

int main(int argc, const char* argv[])
{
	CALLGRIND_STOP_INSTRUMENTATION;
	if (argc < 3)
	{
		fprintf(stderr, "%s <necronomicon path> <trace>\n", argv[0]);
		return 1;
	}

	glooper::ResourceManager resource_manager(argv[1]);
	resource_manager.reload();

	rosalind::Updater updater(
		resource_manager.get_buffer_manager(),
		resource_manager.get_texture_manager());
	chomsky::GUIParser parser(
		resource_manager.get_texture_manager(),
		resource_manager.get_texture_dictionary());
	{
		chomsky::gpd::Compiler compiler;
		resource_manager.for_each_file(
			"gui/game",
			[&compiler](auto& filename)
			{
				add_compiler_input(compiler, filename);
			});

		try
		{
			compiler.compile(parser);
		}
		catch (const std::exception& exception)
		{
			fprintf(stderr, "%s\n", exception.what());
			return 1;
		}
	}

	updater.get_texture_processor().add(&resource_manager.get_texture_fingerprint_manager());
	updater.get_texture_processor().add(&resource_manager.get_texture_key_manager());

	updater.get_texture_processor().add(&resource_manager.get_item_icon_manager());
	updater.get_item_icon_processor().add(&resource_manager.get_item_icon_manager());

	GUI gui(
		resource_manager.get_texture_manager(),
		resource_manager.get_texture_key_manager());
	updater.get_gui_processor().add(&gui);
	updater.get_frame_processor().add(&gui);

	kommon::JaquesFrameReader reader(argv[2]);

	std::size_t current_frame = 0;
	touchstone::GLTrace gl_trace;
	while (reader.read(current_frame, gl_trace))
	{
		updater.enable_draw(current_frame);
		float diff_collect;
		{
			float before = get_current_time();
			updater.update(gl_trace, current_frame);
			float after = get_current_time();
			diff_collect = (after - before) * 1000;
		}
		updater.disable_draw(current_frame);

		resource_manager.get_texture_fingerprint_manager().wait();

		auto start_parse = get_current_time();
		bool success = false;
		CALLGRIND_START_INSTRUMENTATION;
		for (int i = 0; i < 1; ++i)
		{
			chomsky::GUIContent content;
			success = parser.parse(gui.element_stream, content);
			gui.element_stream.seek(0);
		}
		CALLGRIND_STOP_INSTRUMENTATION;
		gui.element_stream.clear();
		auto stop_parse = get_current_time();
		auto diff_parse = (stop_parse - start_parse) * 1000;

		if (success)
		{
			printf("successfully parsed element stream\n");
		}
		printf("finished frame %lu in %.4f ms (%.4f building)\n", current_frame, diff_parse, diff_collect);

		++current_frame;
		gl_trace.clear();
	}

	printf("done!\n");

	return 0;
}
