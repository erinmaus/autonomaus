// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <boost/filesystem.hpp>
#include "autonomaus/core/time.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/guiProvider.hpp"

autonomaus::GUIProvider::~GUIProvider()
{
	if (get_is_attached())
	{
		get_manager()->get_resource_manager().remove_listener(this);
	}
}

void autonomaus::GUIProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);
	manager.get_resource_manager().add_listener(this);

	load_parsers();
	this->element_stream = std::make_unique<chomsky::GUIElementStream>(
		manager.get_resource_manager().get_texture_manager(),
		manager.get_resource_manager().get_texture_key_manager());

	auto& processors = manager.get_event_processors();
	processors.get_gui_processor().add(this);
	processors.get_frame_processor().add(this);
}

void autonomaus::GUIProvider::detach()
{
	get_manager()->get_resource_manager().remove_listener(this);

	auto& processors = get_manager()->get_event_processors();
	processors.get_gui_processor().remove(this);
	processors.get_frame_processor().remove(this);

	Base::detach();
}

void autonomaus::GUIProvider::update()
{
	auto& working = get_slot()->get_working();
	working.elements.clear();
	
	if (this->element_stream->size() == 0)
	{
		return;
	}

	for (auto& i: this->parsers)
	{
		auto& name = i.first;
		auto& parser = *i.second;

		chomsky::GUIContent content("root");
		if (parser.parse(*this->element_stream, content))
		{
			working.contents[name] = content;
		}

		this->element_stream->seek(0);
	}

	working.elements.insert(
		working.elements.begin(),
		this->element_stream->begin(),
		this->element_stream->end());
	this->element_stream->clear();
}

void autonomaus::GUIProvider::on_reload_resources()
{
	load_parsers();
}

void autonomaus::GUIProvider::on_draw_enabled(float timestamp)
{
	this->draw_enabled = true;
}

void autonomaus::GUIProvider::on_draw_disabled(float timestamp)
{
	this->draw_enabled = false;
}

void autonomaus::GUIProvider::on_draw_gui(const kompvter::GUIDrawEvent& event)
{
	if (!this->draw_enabled)
	{
		return;
	}

	if (event.get_framebuffer() != 0)
	{
		return;
	}

	kvre::FlatDrawBuffer flat_draw_buffer(event);
	this->element_stream->update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);
}

void autonomaus::GUIProvider::load_parsers()
{
	auto& working = get_slot()->get_working();
	working.contents.clear();
	working.compile_errors.clear();

	auto& resource_manager = get_manager()->get_resource_manager();

	resource_manager.for_each_directory(
		"gui", false,
		[this](auto& path) { this->load_parser(path); });
}

void autonomaus::GUIProvider::load_parser(const std::string& root_path)
{
	boost::filesystem::path path(root_path);
	std::string name = path.stem().string();

	auto& resource_manager = get_manager()->get_resource_manager();
	try
	{
		chomsky::gpd::Compiler compiler;
		resource_manager.for_each_file(
			root_path,
			[&compiler](auto& filename)
			{
				std::ifstream stream(filename);
				std::string source = std::string(
					std::istreambuf_iterator<char>(stream),
					std::istreambuf_iterator<char>());

				compiler.add_input(filename, source);
			});

		auto parser = std::make_unique<chomsky::GUIParser>(
			resource_manager.get_texture_manager(),
			resource_manager.get_texture_dictionary());
		compiler.compile(*parser);

		this->parsers[name] = std::move(parser);

		Log::info("Created GUI parser '{0}'.", name);
	}
	catch (const std::exception& exception)
	{
		get_slot()->get_working().compile_errors[name] = exception.what();


		Log::critical("Failed to create GUI parser '{0}': {1}", name, exception.what());
	}
}
