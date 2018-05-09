// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "chomsky/gui/gpd/compiler.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "poltergeist/application.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/graphics/utility.hpp"
#include "principle/scene/material.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kvre/ui/flatDrawBuffer.hpp"
#include "kvncer/render/gl.hpp"

poltergeist::Application::Application() :
	principle::Application("poltergeist"),
	poltergeist_window(this),
	gui_draws_window(this),
	edit_texture_definition_window(this),
	gui_state_window(this),
	diff_window(this),
	item_icons_window(this),
	item_icon_3d_view_window(this)
{
	this->gui_shader = principle::Material::load_shader_from_file(
		"gui.vert.glsl", "gui.frag.glsl");
	this->gui_shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	this->gui_shader->bind_input("vert_Texture", kvncer::Shader::vertex_input_texture_coordinate);
	this->gui_shader->bind_input("vert_TextureMeta", kvncer::Shader::vertex_input_texture_meta);
	this->gui_shader->bind_input("vert_Color", kvncer::Shader::vertex_input_color);

	this->highlight_shader = principle::Material::load_shader_from_file(
		"gui_highlight.vert.glsl", "gui_highlight.frag.glsl");
	this->highlight_shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);

	auto& game_manager = get_game_manager();
	game_manager.get_frame_processor().add(this);
	game_manager.get_gui_processor().add(this);
}

poltergeist::Application::~Application()
{
	delete this->gui_shader;
}

void poltergeist::Application::on_draw_frame(float timestamp)
{
	++this->current_frame;
}

void poltergeist::Application::on_draw_enabled(float timestamp)
{
	this->gui_draws.clear();
	this->viewport_width = get_display()->get_width();
	this->viewport_height = get_display()->get_height();
	this->is_draw_enabled = true;

	this->element_streams.clear();
}

void poltergeist::Application::on_draw_disabled(float timestamp)
{
	get_display()->resize(this->viewport_width, this->viewport_height);
	this->previous_focus = 0;
	this->current_focus = 0;
	this->is_draw_enabled = false;

	update_diff();
}

void poltergeist::Application::on_draw_gui(const kompvter::GUIDrawEvent& event)
{
	if (!this->is_draw_enabled)
	{
		return;
	}

	kvre::FlatDrawBuffer flat_draw_buffer(event);

	auto& element_stream = get_element_stream(event.get_framebuffer());
	element_stream.update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);

	auto draw = std::make_unique<GUIDraw>();
	principle::generate_gui_mesh(&flat_draw_buffer, draw->mesh, draw->vertex_buffer);
	principle::generate_index_data(
		event.get_vertex_index_mapper(), event.get_draw_parameters(),
		draw->mesh, draw->index_buffer);
	draw->texture = event.get_atlas_name();

	this->gui_draws.emplace(event.get_framebuffer(), std::move(draw));
}

void poltergeist::Application::on_set_viewport(const kompvter::ViewportEvent& event)
{
	this->viewport_width = event.get_width();
	this->viewport_height = event.get_height();
}

void poltergeist::Application::on_mouse_move(const principle::MouseEvent& event)
{
	if (this->is_camera_dragging)
	{
		this->camera_offset.x += event.get_difference().x / this->camera_scale;
		this->camera_offset.y -= event.get_difference().y / this->camera_scale;
	}
}

void poltergeist::Application::on_mouse_button_down(
	const principle::MouseEvent& event)
{
	if (event.get_button() == 2)
	{
		this->is_camera_dragging = true;
	}
	else
	{
		auto& element_stream = get_element_stream(this->current_framebuffer);
		if (element_stream.size() == 0)
		{
			return;
		}

		auto position = event.get_position();
		position.y = get_display()->get_height() - position.y;

		auto view_matrix = glm::inverse(get_view_matrix());
		auto transformed_position = view_matrix * glm::vec4(position, 0.0f, 1.0f);

		std::size_t start;
		if (this->current_focus == 0 || event.get_button() == 1)
		{
			start = element_stream.size() - 1;
		}
		else
		{
			start = this->current_focus - 1;
		}

		std::size_t next_element;
		if(!try_get_element(start, transformed_position.x, transformed_position.y, next_element))
		{
			next_element = this->current_focus;
		}

		this->previous_focus = this->current_focus;
		this->current_focus = next_element;
	}
}

void poltergeist::Application::on_mouse_button_up(const principle::MouseEvent& event)
{
	if (event.get_button() == 2)
	{
		this->is_camera_dragging = false;
	}
}

void poltergeist::Application::on_mouse_scroll(const principle::MouseEvent& event)
{
	float difference = this->previous_scroll - event.get_wheel();
	this->camera_scale = glm::max(this->camera_scale + glm::sign(difference) / 4.0f, 0.125f);
	this->previous_scroll = event.get_wheel();
}

void poltergeist::Application::on_start_trace()
{
	this->current_frame = 0;
	clear_diffs();

	this->item_icon_3d_view_window.close();
	this->camera_offset = glm::vec2(0.0f);
	this->camera_scale = 1.0f;
}

void poltergeist::Application::on_finish_trace()
{
	process_gui_state();
}

void poltergeist::Application::update_gui()
{
	this->highlight_active = false;
	principle::Application::update_gui();
}

void poltergeist::Application::draw()
{
	glViewport(0, 0, this->viewport_width, this->viewport_height);
	principle::Application::draw();

	this->gui_shader->use();
	this->gui_shader->set("principle_Texture", 1);

	auto view = get_view_matrix();
	this->gui_shader->set("principle_View", view);
	auto projection = get_projection_matrix();
	this->gui_shader->set("principle_Projection", projection);

	auto& texture_resource_manager = get_game_manager().get_texture_resource_manager();
	auto draws = this->gui_draws.equal_range(this->current_framebuffer);
	for (auto i = draws.first; i != draws.second; ++i)
	{
		auto& gui_draw = i->second;
		auto texture = texture_resource_manager.get_texture(gui_draw->texture);
		if (texture == nullptr)
		{
			continue;
		}

		texture->bind(0);
		gui_draw->mesh.draw();
	}

	if (this->highlight_active)
	{
		this->highlight_shader->use();
		this->highlight_shader->set("principle_View", view);
		this->highlight_shader->set("principle_Projection", projection);
		this->highlight_shader->set("principle_Color", glm::vec4(1, 0, 0, 1));

		this->highlight_mesh.draw();
	}
}

chomsky::GUIElementStream& poltergeist::Application::get_element_stream(
	int framebuffer)
{
	auto iter = this->element_streams.find(framebuffer);
	if (iter == this->element_streams.end())
	{
		auto& game_manager = get_game_manager();
		auto& texture_manager = game_manager.get_texture_manager();
		auto& texture_key_manager = game_manager.get_texture_key_manager();
		auto result = this->element_streams.emplace(
			framebuffer,
			chomsky::GUIElementStream(texture_manager, texture_key_manager));
		iter = result.first;
	}

	return iter->second;
}

bool poltergeist::Application::try_get_element(
	std::size_t start, int x, int y, std::size_t& result)
{
	auto& element_stream = get_element_stream(this->current_framebuffer);
	for (std::size_t i = start; i > 0; --i)
	{
		auto& element = element_stream.at(i);
		if (x < element.get_x() || x > element.get_x() + element.get_width())
		{
			continue;
		}

		if (y < element.get_y() || y > element.get_y() + element.get_height())
		{
			continue;
		}

		result = i;
		return true;
	}

	return false;
}

void poltergeist::Application::set_highlight(std::size_t index)
{
	auto& element_stream = get_element_stream(this->current_framebuffer);
	if (index >= element_stream.size())
	{
		return;
	}

	auto& element = element_stream.at(index);

	int left, right, top, bottom;
	auto vertex = element.get_vertex(0);
	left = vertex.position.x;
	right = vertex.position.x;
	top = vertex.position.y;
	bottom = vertex.position.y;
	for (int i = 1; i < element.get_num_vertices(); ++i)
	{
		vertex = element.get_vertex(i);
		left = glm::min(left, vertex.position.x);
		right = glm::max(right, vertex.position.x);
		top = glm::max(top, vertex.position.y);
		bottom = glm::min(bottom, vertex.position.y);
	}

	set_highlight(left + 1, right - 1, top - 1, bottom + 1);
}

void poltergeist::Application::set_highlight(
	int left, int right, int top, int bottom)
{
	const static int LINE_WIDTH = 2;
	struct Vertex
	{
		int x, y;
	};
	Vertex vertices[] = {
		// left
		{ left - LINE_WIDTH, top + LINE_WIDTH },
		{ left, top + LINE_WIDTH },
		{ left - LINE_WIDTH, bottom - LINE_WIDTH },
		{ left, top + LINE_WIDTH },
		{ left - LINE_WIDTH, bottom - LINE_WIDTH },
		{ left, bottom - LINE_WIDTH },
		// right
		{ right + LINE_WIDTH, top + LINE_WIDTH },
		{ right, top + LINE_WIDTH },
		{ right + LINE_WIDTH, bottom - LINE_WIDTH },
		{ right, top + LINE_WIDTH },
		{ right + LINE_WIDTH, bottom - LINE_WIDTH },
		{ right, bottom - LINE_WIDTH },
		// top
		{ left - LINE_WIDTH, top + LINE_WIDTH },
		{ right + LINE_WIDTH, top + LINE_WIDTH },
		{ left - LINE_WIDTH, top },
		{ right + LINE_WIDTH, top + LINE_WIDTH },
		{ right + LINE_WIDTH, top },
		{ left - LINE_WIDTH, top },
		// bottom
		{ left - LINE_WIDTH, bottom },
		{ right + LINE_WIDTH, bottom },
		{ left - LINE_WIDTH, bottom - LINE_WIDTH },
		{ right + LINE_WIDTH, bottom },
		{ right + LINE_WIDTH, bottom - LINE_WIDTH },
		{ left - LINE_WIDTH, bottom - LINE_WIDTH },
	};
	std::size_t num_vertices = sizeof(vertices) / sizeof(Vertex);

	kompvter::Buffer vertex_buffer;
	vertex_buffer.allocate(sizeof(vertices));
	vertex_buffer.update(0, (const std::uint8_t*)vertices, sizeof(vertices));
	this->highlight_vertex_buffer.set_buffer(vertex_buffer);

	this->highlight_mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position,
		&this->highlight_vertex_buffer,
		2, GL_INT, GL_FALSE, 0, sizeof(Vertex));
	this->highlight_mesh.set_indices(nullptr, GL_NONE, 0, num_vertices);
	this->highlight_active = true;
}

static void add_compiler_input(
	chomsky::gpd::Compiler& compiler, ALLEGRO_FS_ENTRY* file)
{
	std::string filename = al_get_fs_entry_name(file);
	std::ifstream stream(filename);
	std::string source = std::string(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>());

	compiler.add_input(filename, source);
}

static void add_compiler_inputs(
	chomsky::gpd::Compiler& compiler, ALLEGRO_FS_ENTRY* directory)
{
	ALLEGRO_FS_ENTRY* child = al_read_directory(directory);
	while (child != nullptr)
	{
		if (al_open_directory(child))
		{
			add_compiler_inputs(compiler, child);
			al_close_directory(child);
		}
		else
		{
			add_compiler_input(compiler, child);
		}

		al_destroy_fs_entry(child);
		child = al_read_directory(directory);
	}
}

glm::mat4 poltergeist::Application::get_projection_matrix()
{
	return glm::ortho(
		0.0f, this->viewport_width,
		0.0f, this->viewport_height,
		-1.0f, 1.0f);
}

glm::mat4 poltergeist::Application::get_view_matrix()
{
	auto translation = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(glm::floor(this->camera_offset), 0.0f));
	auto scale = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(this->camera_scale, this->camera_scale, 1.0f));
	auto from_origin = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(this->viewport_width / 2.0f, this->viewport_height / 2.0f, 0.0f));
	auto to_origin = glm::translate(
		glm::mat4(1.0f),
		-glm::vec3(this->viewport_width / 2.0f, this->viewport_height / 2.0f, 0.0f));

	return from_origin * scale * to_origin * translation;
}

bool poltergeist::Application::process_gui_state()
{
	chomsky::gpd::Compiler compiler;
	{
		auto data_path = std::getenv("NECRONOMICON_DATA_PATH");
		if (data_path == nullptr)
		{
			this->gui_state_error = "enviroment variable NECRONOMICON_DATA_PATH not set";
			this->gui_state_success = false;
			return false;
		}

		std::string path = data_path;
		path += "/gui/";

		ALLEGRO_FS_ENTRY* root_entry = al_create_fs_entry(path.c_str());
		if (root_entry != nullptr && al_open_directory(root_entry))
		{
			add_compiler_inputs(compiler, root_entry);
			al_close_directory(root_entry);
		}
		else
		{
			if (root_entry != nullptr)
			{
				al_destroy_fs_entry(root_entry);
			}

			this->gui_state_error = "couldn't add GPD inputs from path: " + path;
			this->gui_state_success = false;
			return false;
		}
		al_destroy_fs_entry(root_entry);
	}

	auto& game = get_game_manager();
	chomsky::GUIParser parser(
		game.get_texture_manager(), game.get_texture_dictionary());

	try
	{
		compiler.compile(parser);
	}
	catch (const std::exception& exception)
	{
		this->gui_state_error = exception.what();
		this->gui_state_success = false;

		return false;
	}

	auto& element_stream = get_element_stream(this->current_framebuffer);
	element_stream.seek(0);
	if (!parser.parse(element_stream, this->gui_state))
	{
		this->gui_state_error = "no matches found";
		this->gui_state_success = false;
		this->current_focus = element_stream.tell();
		return false;
	}
	this->gui_state_error.clear();
	this->gui_state_success = true;
	return true;
}

void poltergeist::Application::update_diff()
{
	auto& element_stream = get_element_stream(this->current_framebuffer);
	if (!this->diff_previous_elements.empty())
	{
		std::size_t count = this->element_diff_range_end - this->element_diff_range_begin;
		for (std::size_t i = 0; i < count; ++i)
		{
			std::size_t current_index = i + this->element_diff_range_begin;
			if (current_index >= element_stream.size())
			{
				break;
			}

			if (i > diff_previous_elements.size())
			{
				add_diff(element_stream.at(current_index));
				continue;
			}

			auto previous_texture_name = diff_previous_elements[i].get_texture_name();
			auto previous_texture_region_name = diff_previous_elements[i].get_texture_region_name();
			auto current_texture_name = element_stream.at(current_index).get_texture_name();
			auto current_texture_region_name = element_stream.at(current_index).get_texture_region_name();

			if (previous_texture_name != current_texture_name ||
				previous_texture_region_name != current_texture_region_name)
			{
				add_diff(element_stream.at(current_index));
			}
		}
	}

	this->diff_previous_elements.clear();
	for (std::size_t i = this->element_diff_range_begin; i < this->element_diff_range_end; ++i)
	{
		if (i >= element_stream.size())
		{
			break;
		}

		this->diff_previous_elements.push_back(element_stream.at(i));
	}
}

void poltergeist::Application::add_diff(const chomsky::GUIElement& element)
{
	auto key = std::make_pair(element.get_texture_name(), element.get_texture_region_name());
	if (this->element_diffs_cache.count(key) == 0)
	{
		auto& texture_manager = get_game_manager().get_texture_manager();
		assert(texture_manager.has_texture(element.get_texture_name()));

		auto texture = texture_manager.get_texture(element.get_texture_name());
		auto region = texture.get_region_from_name(element.get_texture_region_name());
		if (region == nullptr)
		{
			return;
		}

		this->element_diffs.emplace_back(element, this->current_frame, region);
		this->element_diffs_cache.insert(key);
	}
}

void poltergeist::Application::clear_diffs()
{
	this->diff_previous_elements.clear();
	this->element_diffs.clear();
	this->element_diffs_cache.clear();
}

poltergeist::Application::ElementDiff::ElementDiff(
	const chomsky::GUIElement& element,
	std::size_t frame,
	const kompvter::TextureRegion* region) :
		element(element),
		frame(frame)
{
	this->texture = std::make_unique<kvncer::Texture>(
		region->get_pixel_format(), region->get_is_compressed(),
		region->get_width(), region->get_height());
	this->region = *region;

	if(region->get_is_compressed())
	{
		this->texture->compressed_update(
			0, 0,
			region->get_width(), region->get_height(),
			region->get_pixel_format(),
			region->get_pixels_data(), region->get_pixels_size());
	}
	else
	{
		this->texture->update(
			0, 0,
			region->get_width(), region->get_height(),
			region->get_pixel_format(), region->get_pixel_type(),
			region->get_pixels_data());
	}
}
