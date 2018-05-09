// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/graphics/worldOverlay.hpp"
#include "autonomaus/graphics/shapes.hpp"

static const char vertex_shader[] =
	"#version 140\n"
	"uniform mat4 maus_Projection;\n"
	"uniform mat4 maus_View;\n"
	"uniform mat4 maus_Model;\n"
	"in vec3 vert_Position;\n"
	"in vec2 vert_Texture;\n"
	"in vec4 vert_Color;\n"
	"out vec2 frag_Texture;\n"
	"out vec4 frag_Color;\n"
	"void main()\n"
	"{\n"
	"	vec4 world_position = \n"
	"		vec4(vert_Position, 1.0) *\n"
	"		maus_Model;\n"
	"	gl_Position =\n"
	"		world_position *\n"
	"		maus_View *\n"
	"		maus_Projection;\n"
	"	frag_Texture = vert_Texture;\n"
	"	frag_Color = vert_Color;\n"
	"}\n";
static const char fragment_shader[] =
	"#version 140\n"
	"uniform sampler2D maus_Texture;\n"
	"uniform float maus_UseTexture;\n"
	"uniform float maus_Alpha;\n"
	"uniform vec4 maus_Color;\n"
	"in vec2 frag_Texture;\n"
	"in vec4 frag_Color;\n"
	"out vec4 out_Color;\n"
	"void main()\n"
	"{\n"
	"	vec4 sample = texture(maus_Texture, frag_Texture);\n"
	"	vec4 result = frag_Color * mix(vec4(1.0), sample, maus_UseTexture);\n"
	"	result = vec4(result.rgb, result.a * maus_Alpha) * maus_Color;\n"
	"	out_Color = vec4(result.rgb * result.a, result.a);\n"
	"}\n";

autonomaus::WorldOverlay::WorldOverlay(Renderer& renderer, const DrawState& draw_state) :
	Overlay(renderer, draw_state)
{
	// Nothing.
}

void autonomaus::WorldOverlay::unload_resources()
{
	this->program.reset();
	for (int i = 0; i < mesh_max; ++i)
	{
		this->meshes[i].reset();
	}
}

void autonomaus::WorldOverlay::load_resources()
{
	this->program = std::make_unique<Program>(
		get_renderer(), vertex_shader, fragment_shader);

	this->meshes[mesh_box] = std::make_unique<Mesh>(get_renderer());
	make_box(*this->meshes[mesh_box].get(), glm::vec3(1.0f), glm::vec4(1.0f));
	this->meshes[mesh_box_outline] = std::make_unique<Mesh>(get_renderer());
	make_box_outline(*this->meshes[mesh_box_outline].get(), glm::vec3(1.0f), glm::vec4(1.0f));
	this->meshes[mesh_sphere] = std::make_unique<Mesh>(get_renderer());
	make_sphere(*this->meshes[mesh_sphere].get(), 1.0f, glm::vec4(1.0f));
}

void autonomaus::WorldOverlay::draw_box(
	const glm::vec3& position,
	const glm::vec3& size,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		glm::translate(glm::mat4(1.0f), position) *
		glm::scale(glm::mat4(1.0f), size);
	object->color = color;
	object->primitive = Mesh::primitive_triangles;
	object->mesh = mesh_box;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_box_outline(
	const glm::vec3& position,
	const glm::vec3& size,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		glm::translate(glm::mat4(1.0f), position) *
		glm::scale(glm::mat4(1.0f), size);
	object->color = color;
	object->primitive = Mesh::primitive_lines;
	object->mesh = mesh_box_outline;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_sphere(
	const glm::vec3& position,
	float radius,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		glm::translate(glm::mat4(1.0f), position) *
		glm::scale(glm::mat4(1.0f), glm::vec3(radius));
	object->color = color;
	object->primitive = Mesh::primitive_triangles;
	object->mesh = mesh_sphere;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_box(
	const glm::mat4& model_matrix,
	const glm::vec3& size,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		model_matrix *
		glm::translate(glm::mat4(1.0f), size * glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), size);
	object->color = color;
	object->primitive = Mesh::primitive_triangles;
	object->mesh = mesh_box;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_box_outline(
	const glm::mat4& model_matrix,
	const glm::vec3& size,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		model_matrix *
		glm::translate(glm::mat4(1.0f), size * glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), size);
	object->color = color;
	object->primitive = Mesh::primitive_lines;
	object->mesh = mesh_box_outline;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_sphere(
	const glm::mat4& model_matrix,
	float radius,
	const glm::vec4& color)
{
	auto object = new MeshDrawObject(this);
	object->model_matrix =
		model_matrix *
		glm::scale(glm::mat4(1.0f), glm::vec3(radius)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, radius * 0.5f, 0.0f));
	object->color = color;
	object->primitive = Mesh::primitive_triangles;
	object->mesh = mesh_sphere;

	add_draw(object);
}

void autonomaus::WorldOverlay::draw_tile(
	const thoreau::Map& map,
	const thoreau::TileCoordinate& tile_coordinate,
	const glm::vec4& color,
	int radius,
	float thickness,
	float offset)
{
	auto object = new GridDrawObject(this);
	object->line_thickness = thickness;
	object->color = color;

	int start_x = tile_coordinate.x - radius;
	int start_y = tile_coordinate.y - radius;
	int stop_x = tile_coordinate.x + radius;
	int stop_y = tile_coordinate.y + radius;

	object->position.x = 0.0f;
	object->position.y = offset;
	object->position.z = 0.0f;

	for (int i = start_x; i <= stop_x; ++i)
	{
		for (int j = start_y; j <= stop_y; ++j)
		{
			if (!map.has_tile(i, j, tile_coordinate.layer))
			{
				continue;
			}

			float center = map.get_tile(i, j, tile_coordinate.layer).get_max_elevation();
			float top_left = HUGE_VALF;
			float top_right = HUGE_VALF;
			float bottom_left = HUGE_VALF;
			float bottom_right = HUGE_VALF;

			if (map.has_tile(i - 1, j - 1, tile_coordinate.layer))
			{
				bottom_left = map.get_tile(i - 1, j - 1, tile_coordinate.layer).get_max_elevation();
			}
			if (map.has_tile(i + 1, j - 1, tile_coordinate.layer))
			{
				bottom_right = map.get_tile(i + 1, j - 1, tile_coordinate.layer).get_max_elevation();
			}
			if (map.has_tile(i - 1, j + 1, tile_coordinate.layer))
			{
				top_left = map.get_tile(i - 1, j + 1, tile_coordinate.layer).get_max_elevation();
			}
			if (map.has_tile(i + 1, j + 1, tile_coordinate.layer))
			{
				top_right = map.get_tile(i + 1, j + 1, tile_coordinate.layer).get_max_elevation();
			}

			if (center == HUGE_VALF)
			{
				center = -HUGE_VALF;
			}

			if (top_right == HUGE_VALF)
			{
				top_right = center;
			}

			if (top_left == HUGE_VALF)
			{
				top_left = center;
			}

			if (bottom_right == HUGE_VALF)
			{
				bottom_right = center;
			}

			if (bottom_left == HUGE_VALF)
			{
				bottom_left = center;
			}

			auto a = glm::mix(top_left, center, 0.5f);
			auto b = glm::mix(top_right, center, 0.5f);
			auto c = glm::mix(bottom_left, center, 0.5f);
			auto d = glm::mix(bottom_right, center, 0.5f);

			object->add_point(i - 0.0f, j + 1.0f, a);
			object->add_point(i + 1.0f, j + 1.0f, b);
			object->add_point(i - 0.0f, j - 0.0f, c);
			object->add_point(i + 1.0f, j - 0.0f, d);
			object->add_point(i - 0.0f, j + 1.0f, a);
			object->add_point(i - 0.0f, j - 0.0f, c);
			object->add_point(i + 1.0f, j + 1.0f, b);
			object->add_point(i + 1.0f, j - 0.0f, d);
		}
	}

	add_draw(object);
}

void autonomaus::WorldOverlay::start_draw(int id, float age, float lifespan)
{
	float alpha;
	if (age > lifespan)
	{
		alpha = 1.0f - glm::min((age - lifespan) / get_fade_duration(), 1.0f);
	}
	else
	{
		alpha = 1.0f;
	}

	get_renderer().push_style();
	get_renderer().unset_scissor();
	get_renderer().set_blend_mode(Renderer::blend_alpha);

	this->program->use();
	this->program->set("maus_Alpha", alpha);
	this->program->set("maus_View", get_draw_state().get_camera_view_matrix());
	this->program->set("maus_Projection", get_draw_state().get_camera_projection_matrix());
}

void autonomaus::WorldOverlay::stop_draw()
{
	get_renderer().pop_style();
}

autonomaus::WorldOverlay::MeshDrawObject::MeshDrawObject(WorldOverlay* overlay)
{
	this->overlay = overlay;
}

void autonomaus::WorldOverlay::MeshDrawObject::draw()
{
	this->overlay->get_renderer().enable_depth_testing();

	this->overlay->program->use();
	this->overlay->program->set("maus_Model", this->model_matrix);
	this->overlay->program->set("maus_Color", this->color);
	this->overlay->program->set("maus_UseTexture", 0.0f);

	this->overlay->meshes[this->mesh]->draw(this->primitive);
}

autonomaus::WorldOverlay::GridDrawObject::GridDrawObject(WorldOverlay* overlay)
{
	this->overlay = overlay;
}

void autonomaus::WorldOverlay::GridDrawObject::draw()
{
	Mesh mesh(this->overlay->get_renderer());
	{
		mesh.set_num_vertices(this->points.size());

		std::vector<glm::vec4> colors;
		colors.resize(this->points.size(), this->color);

		mesh.set_positions(&this->points[0]);
		mesh.set_colors(&colors[0]);
	}

	this->overlay->get_renderer().push_style();
	this->overlay->get_renderer().set_line_thickness(this->line_thickness);

	this->overlay->program->use();
	this->overlay->program->set("maus_Model", glm::translate(glm::mat4(1.0f), this->position));
	this->overlay->program->set("maus_Color", this->color);
	this->overlay->program->set("maus_UseTexture", 0.0f);
	mesh.draw(Mesh::primitive_lines, 0, mesh.get_num_vertices());

	this->overlay->get_renderer().pop_style();
}

void autonomaus::WorldOverlay::GridDrawObject::add_point(
	float tile_x, float tile_y, float elevation)
{
	glm::vec3 point;
	point.x = tile_x * thoreau::TILE_SIZE;
	point.y = elevation;
	point.z = tile_y * thoreau::TILE_SIZE;

	this->points.push_back(point);
}
