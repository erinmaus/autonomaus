// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "kvlc/geometry/ray.hpp"
#include "kvlc/geometry/mesh.hpp"
#include "kvlc/collision/query.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "principle/resources/modelPicker.hpp"

std::vector<int> principle::ModelPicker::pick(
	const glm::mat4& view, const glm::mat4& projection, const glm::vec4& viewport,
	const glm::vec2& mouse)
{
	std::map<int, glm::vec3> p;
	return pick(view, projection, viewport, mouse, p);
}

std::vector<int> principle::ModelPicker::pick(
	const glm::mat4& view, const glm::mat4& projection, const glm::vec4& viewport,
	const glm::vec2& mouse, std::map<int, glm::vec3>& positions)
{
	auto v = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale)) * view;
	glm::vec3 a = glm::unProject(glm::vec3(mouse, 0.0f), v, projection, viewport);
	glm::vec3 b = glm::unProject(glm::vec3(mouse, 0.1f), v, projection, viewport);
	glm::vec3 c = glm::unProject(glm::vec3(mouse, 1.0f), v, projection, viewport);

	kvlc::Ray ray;
	ray.position = a;
	ray.direction = glm::normalize(b - a);
	float length = glm::length(c - a);

	return pick(ray, length, positions);
}

std::vector<int> principle::ModelPicker::pick(
	const kvlc::Ray& ray, float length,
	std::map<int, glm::vec3>& positions)
{
	std::vector<int> results;
	kvlc::Query query;
	if (this->collision_scene.test(ray, length, query))
	{
		typedef std::pair<float, int> Hit;
		std::set<Hit> hits;
		for (auto hit: query)
		{
			float distance = glm::length(ray.position - hit.second);
			hits.emplace(distance, hit.first);
			positions[this->scene_tags[hit.first]] = hit.second;
		}

		for (auto hit: hits)
		{
			results.push_back(this->scene_tags[hit.second]);
		}
	}

	return results;
}

std::vector<int> principle::ModelPicker::pick(
	const kvlc::Bounds& bounds)
{
	std::vector<int> results;
	kvlc::Query query;
	if (this->collision_scene.test(bounds, query))
	{
		for (auto hit: query)
		{
			results.push_back(hit.first);
		}
	}

	return results;
}

void principle::ModelPicker::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	std::vector<glm::vec3> points;
	if (chomsky::is_actor_draw_event(event))
	{
		points = transform_skinned_model(model, event);
	}
	else
	{
		points = transform_unskinned_model(model, event);
	}

	add_model(tag, points, event);
}

void principle::ModelPicker::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	std::vector<glm::vec3> points = transform_terrain(terrain, event);
	add_model(tag, points, event);
}

void principle::ModelPicker::draw_water(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	std::vector<glm::vec3> points = transform_terrain(terrain, event);
	add_model(tag, points, event);
}

void principle::ModelPicker::invalidate_tag(int tag)
{
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		if (iter->second != 0)
		{
			this->collision_scene.remove(iter->second);
		}

		this->scene_tags.erase(iter->second);
		this->tags.erase(iter);
	}
}

std::vector<glm::vec3> principle::ModelPicker::transform_skinned_model(
	const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> result;
	auto model_matrix = event.get_model_matrix();
	for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
	{
		auto position = model->get_position(i);
		auto bone = model->get_bone(i);

		auto bone_transform = event.get_bone_transform(bone);
		auto transformed_position = bone_transform * glm::vec4(position, 1.0f);
		transformed_position = model_matrix * transformed_position;

		result.push_back(glm::vec3(transformed_position));
	}

	return result;
}

std::vector<glm::vec3> principle::ModelPicker::transform_unskinned_model(
	const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> result;
	auto model_matrix = event.get_model_matrix();
	for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
	{
		auto position = model->get_position(i);
		auto transformed_position = model_matrix * glm::vec4(position, 1.0f);
		result.push_back(glm::vec3(transformed_position));
	}

	return result;
}

std::vector<glm::vec3> principle::ModelPicker::transform_terrain(
	const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> result;
	auto model_matrix = event.get_model_matrix();
	for (std::size_t i = 0; i < terrain->get_num_vertices(); ++i)
	{
		auto position = terrain->get_position(i);
		auto transformed_position = model_matrix * glm::vec4(position, 1.0f);
		result.push_back(glm::vec3(transformed_position));
	}

	return result;
}

void principle::ModelPicker::add_model(
	int tag,
	const std::vector<glm::vec3>& points,
	const kompvter::ModelDrawEvent& event)
{
	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto num_indices = std::distance(begin, end);
	if (num_indices == 0 || num_indices % 3 != 0)
	{
		return;
	}

	kvlc::Mesh mesh;
	mesh.set_num_points(points.size());
	auto mesh_points = mesh.get_points();
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		mesh_points[i] = glm::vec4(points[i], 0.0f);
	}

	mesh.set_num_triangles(num_indices / 3);
	auto mesh_indices = mesh.get_triangles();
	for (std::size_t i = 0; i < num_indices; ++i)
	{
		auto current_index = *(begin + i);
		mesh_indices[i] = current_index;
	}

	auto scene_tag = this->collision_scene.add(mesh);
	tags[tag] = scene_tag;
	scene_tags[scene_tag] = tag;
}
