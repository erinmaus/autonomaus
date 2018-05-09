#include <fstream>
#include <cstdio>
#include "kommon/io/jaquesFrameReader.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiQuery.hpp"
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

	kvre::FlatDrawBuffer flat_draw_buffer(event);
	element_stream.update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);
}


typedef std::tuple<std::string /* thing */, int /* quantity */> Requirement;

struct Item
{
	std::string name;
	float xp = 0.0f;
	std::set<Requirement> requirements;
	std::set<Requirement> materials;
};

struct Action
{
	std::string name;
	std::map<std::string, Item> items;
};

std::map<std::string, Action> actions;

void extract(const chomsky::GUIContent& root)
{
	std::string action;
	{
		static const chomsky::GUIQuery action_query("*.main.windows[*]:make-x-window.items.drop-down.button.text");
		std::vector<chomsky::GUIContent> c;
		if (!action_query.collect(root, c) || c.empty())
		{
			return;
		}

		action = c[0].get_text().to_string();
	}

	std::string item;
	{
		static const chomsky::GUIQuery item_query("*.main.windows[*]:make-x-window.recipe.header.name");
		std::vector<chomsky::GUIContent> c;
		if (!item_query.collect(root, c) || c.empty())
		{
			return;
		}

		item = c[0].get_text().to_string();
	}

	float xp = HUGE_VALF;
	{
		static const chomsky::GUIQuery xp_query("*.main.windows[*]:make-x-window.recipe.rows[*].columns[0].text");

		std::vector<chomsky::GUIContent> c;
		if (xp_query.collect(root, c) && !c.empty())
		{
			xp = std::atof(c[0].get_text().to_string().c_str());
		}
	}

	std::set<Requirement> requirements;
	std::set<Requirement> materials;
	{
		static const chomsky::GUIQuery row_query("*.main.windows[*]:make-x-window.recipe.rows[*]");
		std::vector<chomsky::GUIContent> rows;
		if (row_query.collect(root, rows) && !rows.empty())
		{
			for (auto& row: rows)
			{
				if (!row.has("text"))
				{
					continue;
				}

				if (row.get_height() < 10)
				{
					continue;
				}

				auto name = row.get("text").get_text().to_string();
				for (auto& c: name)
				{
					if (c == '\n')
					{
						c = ' ';
					}
				}

				std::string quantity;
				if (row.has("quantity"))
				{
					quantity = row.get("quantity").get_text().to_string();
				}

				if (quantity.empty())
				{
					quantity = "1";
				}

				if (row.get_tags().has("requirement"))
				{
					requirements.emplace(name, std::atoi(quantity.c_str()));
				}
				else if (row.get_tags().has("material"))
				{
					materials.emplace(name, std::atoi(quantity.c_str()));
				}
			}
		}
	}

	actions[action].name = action;
	actions[action].items[item].name = item;
	actions[action].items[item].requirements.insert(requirements.begin(), requirements.end());
	actions[action].items[item].materials.insert(materials.begin(), materials.end());

	if (xp != HUGE_VALF)
	{
		actions[action].items[item].xp = xp;
	}
}

void dump()
{
	for (auto& action: actions)
	{
		std::printf("ACTION %s\n", action.first.c_str());

		for (auto& item: action.second.items)
		{
			std::printf("ITEM %f %s\n", item.second.xp, item.first.c_str());

			for (auto& requirement: item.second.requirements)
			{
				if (item.second.materials.count(requirement) != 0)
				{
					continue;
				}

				auto name = std::get<0>(requirement);
				auto quantity = std::get<1>(requirement);

				std::printf("REQUIRE %d %s\n", quantity, name.c_str());
			}

			for (auto& material: item.second.materials)
			{
				auto name = std::get<0>(material);
				auto quantity = std::get<1>(material);

				std::printf("INPUT %d %s\n", quantity, name.c_str());
			}
		}
	}
}

int main(int argc, const char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "%s <NECRONOMICON path> <trace>\n", argv[0]);
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
		updater.update(gl_trace, current_frame);
		updater.disable_draw(current_frame);

		resource_manager.get_texture_fingerprint_manager().wait();

		chomsky::GUIContent content;
		if (parser.parse(gui.element_stream, content))
		{
			extract(content);
		}

		gui.element_stream.clear();

		++current_frame;
		gl_trace.clear();
	}

	dump();

	return 0;
}
