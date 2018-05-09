// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include "thoreau/tileCoordinate.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/resources/draw.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/profile/profile.hpp"
#include "zeitgeist/application.hpp"

zeitgeist::Application::ActorDrawListWindow::ActorDrawListWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	open();
}

void zeitgeist::Application::ActorDrawListWindow::clear()
{
	this->thumbnails.clear();
	this->thumbnail_times.clear();
}

void zeitgeist::Application::ActorDrawListWindow::update(float delta)
{
	auto iter = this->thumbnail_times.find(current_thumbnail);
	if (iter != this->thumbnail_times.end())
	{
		iter->second += delta;
	}
}

std::string zeitgeist::Application::ActorDrawListWindow::get_title() const
{
	return "Actor List";
}

static int count_bones(const chomsky::ActorDraw& draw)
{
	auto begin = draw.models_begin();
	auto end = draw.models_end();

	typedef std::tuple<const kvre::Model*, int> ModelBone;
	std::map<ModelBone, glm::vec3> vertices;
	std::map<ModelBone, glm::mat4> bones;
	while (begin != end)
	{
		auto model = *begin;
		auto count = model->get_num_vertices();
		for (std::size_t i = 0; i < count; ++i)
		{
			auto bone = model->get_bone(i);
			auto key = std::make_tuple(model, bone);
			auto position = model->get_position(i);
			auto iter = vertices.find(key);
			if (iter == vertices.end())
			{
				vertices[key] = position;
			}
			else
			{
				iter->second += position;
				iter->second /= 2.0f;
			}
			bones.emplace(key, draw.get_bone(model, bone));
		}
		++begin;
	}

	static std::set<const chomsky::ActorDraw*> dumped;
	if (!dumped.count(&draw))
	{
		if (dumped.size() == 7)
		{
			std::printf("OBJ %lu\n", dumped.size());
			for (auto i: vertices)
			{
				auto m = bones[i.first];
				//glm::ivec3 v = i.second;
				glm::ivec4 v = m * glm::vec4(glm::vec3(0), 1.0f);
				std::printf("v %d %d %d\n", v.x, v.y, v.z);
				//std::printf("v %d %d %d\n", std::get<0>(i), std::get<1>(i), std::get<2>(i));
			}
		}

		dumped.insert(&draw);
	}

	int count = 0;
	for (auto i = bones.begin(); i != bones.end(); ++i)
	{
		bool unique = true;

		auto next = i;
		for (auto j = ++next; j != bones.end(); ++j)
		{
			if (i->second == j->second)
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			++count;
		}
	}

	return bones.size();
}

void zeitgeist::Application::ActorDrawListWindow::update_gui()
{
	auto& actor_bounds_state = this->application->get_game_manager().get_actor_bounds_state();
	auto& actor_materials_state = this->application->get_game_manager().get_actor_materials_state();
	auto& actor_draw_state = this->application->get_game_manager().get_actor_draw_state();
	auto& draw_manager = this->application->get_game_manager().get_draw_manager();
	auto& model_manager = this->application->get_game_manager().get_model_manager();

	ImGui::Checkbox("Show Draw Details", &this->show_details);

	this->current_thumbnail = thoreau::WorldCoordinate();

	int actor_draw = 1;
	ImGui::BeginGroup();
	for (auto& i: actor_draw_state)
	{
		ImGui::PushID(actor_draw);

		show_thumbnail(i.first, i.second);

		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			if (ImGui::Button("Focus"))
			{
				auto tag = *i.second.tags_begin();
				if (draw_manager.has(tag))
				{
					auto& draw = draw_manager.get(tag);
					this->application->camera.focus(draw.get_translation());
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Select"))
			{
				for (auto j = i.second.tags_begin(); j != i.second.tags_end(); ++j)
				{
					this->application->select(*j);
				}
			}

			if (ImGui::Button("View Materials") && actor_materials_state.has(i.first))
			{
				this->application->materials_set_window.open(
					actor_materials_state.get(i.first));
			}

			ImGui::SameLine();
			if (ImGui::Button("Materials Query") && actor_materials_state.has(i.first))
			{
				this->application->material_set_match_builder_window.add(i.first);
			}

			if (ImGui::Button("Point Cloud Query"))
			{
				this->application->point_cloud_match_builder_window.add(i.first);
			}

			if (ImGui::Button("Animation Query"))
			{
				this->application->animation_builder_window.add(i.first);
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			auto tile = thoreau::tile_from_world(i.first);
			ImGui::Text("actor draw %4d at (%d, %d; %d)", actor_draw, tile.x, tile.y, tile.layer);
			if (show_details)
			{
				auto size = actor_bounds_state.get(i.first).compute_size();

				ImGui::Text("Dimensions: (%.0f, %.0f, %.0f)\n", size.x, size.y, size.z);
				ImGui::Text("World Position: (%d, %d)", i.first.x, i.first.y);

				std::size_t num_vertices = 0;
				for (auto j = i.second.tags_begin(); j != i.second.tags_end(); ++j)
				{
					kompvter::VertexIndexMapper index_mapper;
					if (model_manager.tag_has_vertex_index_mapper(*j))
					{
						index_mapper = model_manager.get_tag_vertex_index_mapper(*j);
					}

					auto& draw = draw_manager.get(*j);
					auto begin = kvre::IndexedVertexIterator::begin(
						index_mapper, draw.get_draw_parameters());
					auto end = kvre::IndexedVertexIterator::end(
						index_mapper, draw.get_draw_parameters());
					ImGui::BulletText(
						"tag %d (%lu vertices; %lu indices; index buffer %d)\n",
						*j, end - begin,
						index_mapper.get_num_indices(),
						index_mapper.get_buffer().get_name());

					num_vertices += end - begin;
				}

				ImGui::Text("Vertex Total: %lu", num_vertices);
				ImGui::Text("Bones Total: %d\n", count_bones(i.second));
			}

			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::EndGroup();

		ImGui::PopID();

		++actor_draw;
	}
	ImGui::EndGroup();
}

void zeitgeist::Application::ActorDrawListWindow::update_thumbnail(
	const thoreau::WorldCoordinate& location,
	const chomsky::ActorDraw& actor_draw)
{
	if (this->thumbnails.count(location) != 0)
	{
		return;
	}

	auto& draw_manager = this->application->get_game_manager().get_draw_manager();
	auto& model_manager = this->application->get_game_manager().get_model_manager();

	auto& thumbnail = this->thumbnails[location] = std::make_shared<principle::Thumbnail>(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
	this->thumbnail_times[location] = 0.0f;

	for (auto i = actor_draw.tags_begin(); i != actor_draw.tags_end(); ++i)
	{
		if (!draw_manager.has(*i) || !model_manager.has(*i))
		{
			continue;
		}

		auto& draw = draw_manager.get(*i);
		auto model = model_manager.get(*i);

		if (model_manager.tag_has_vertex_index_mapper(*i))
		{
			thumbnail->add_model(
				model,
				draw.get_model_matrix(),
				model_manager.get_tag_vertex_index_mapper(*i),
				draw.get_draw_parameters());
		}
		else
		{
			thumbnail->add_model(
				model,
				draw.get_model_matrix(),
				kompvter::VertexIndexMapper(),
				draw.get_draw_parameters());
		}
	}
}

void zeitgeist::Application::ActorDrawListWindow::show_thumbnail(
	thoreau::TileCoordinate location,
	const chomsky::ActorDraw& actor_draw)
{
	update_thumbnail(location, actor_draw);

	auto m = glm::rotate(
		glm::mat4(1.0f),
		(float)(this->thumbnail_times[location] * M_PI / 2.0),
		glm::vec3(0.0f, 1.0f, 0.0f));
	auto image = this->thumbnails[location]->draw(m);
	ImGui::Image(
		image,
		ImVec2(image->get_width(), image->get_height()));
	if (ImGui::IsItemHovered())
	{
		this->current_thumbnail = location;
	}
}
