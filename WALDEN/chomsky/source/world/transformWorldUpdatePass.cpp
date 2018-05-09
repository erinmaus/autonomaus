// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::TransformWorldUpdatePass::TransformWorldUpdatePass(
	TransformState* state)
{
	this->state = state;
}

void chomsky::TransformWorldUpdatePass::stop()
{
	for (auto& i: this->pending)
	{
		i.wait();
	}

	this->pending.clear();
}

void chomsky::TransformWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (resources.count(model) == 0)
	{
		if (!is_scenery_draw_event(event) && !is_prop_draw_event(event))
		{
			return;
		}

		auto& points = this->resources[model];
		auto& model_matrix = event.get_model_matrix();
		auto result = std::async(
			std::launch::async,
			[this, &points, model, &model_matrix]
			{
				auto num_points = model->get_num_vertices();
				for (std::size_t i = 0; i < num_points; ++i)
				{
					auto position = model->get_position(i);
					auto transformed_point = model_matrix * glm::vec4(position, 1.0f);
					points.push_back(glm::vec3(transformed_point));
				}
			});
		this->pending.emplace_back(std::move(result));
	}
	else if (!is_scenery_draw_event(event) && !is_prop_draw_event(event))
	{
		invalidate_tag(tag);
		return;
	}

	this->state->add(tag) = this->resources[model];
}

void chomsky::TransformWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (resources.count(terrain) == 0)
	{
		auto& points = this->resources[terrain];
		auto& model_matrix = event.get_model_matrix();
		auto result = std::async(
			std::launch::async,
			[this, &points, terrain, &model_matrix]
			{
				auto num_points = terrain->get_num_vertices();
				for (std::size_t i = 0; i < num_points; ++i)
				{
					auto position = terrain->get_position(i);
					auto transformed_point = model_matrix * glm::vec4(position, 1.0f);
					points.push_back(glm::vec3(transformed_point));
				}
			});
		this->pending.emplace_back(std::move(result));
	}

	this->state->add(tag) = this->resources[terrain];
}

void chomsky::TransformWorldUpdatePass::draw_water(
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	if (resources.count(water) == 0)
	{
		auto& points = this->resources[water];
		auto& model_matrix = event.get_model_matrix();
		auto result = std::async(
			std::launch::async,
			[this, &points, water, &model_matrix]
			{
				auto num_points = water->get_num_vertices();
				for (std::size_t i = 0; i < num_points; ++i)
				{
					auto position = water->get_position(i);
					auto transformed_point = model_matrix * glm::vec4(position, 1.0f);
					points.push_back(glm::vec3(transformed_point));
				}
			});
		this->pending.emplace_back(std::move(result));
	}

	this->state->add(tag) = this->resources[water];
}

void chomsky::TransformWorldUpdatePass::invalidate_tag(int tag)
{
	this->state->remove(tag);
}

void chomsky::TransformWorldUpdatePass::invalidate_model(const kvre::Model* model)
{
	this->resources.erase(model);
}

void chomsky::TransformWorldUpdatePass::invalidate_terrain(const kvre::Terrain* terrain)
{
	this->resources.erase(terrain);
}

void chomsky::TransformWorldUpdatePass::invalidate_water(const kvre::Terrain* terrain)
{
	this->resources.erase(terrain);
}

chomsky::TransformWorldUpdatePass::PointCollection::PointCollection(const Points& points)
{
	this->points = &points;
}

const chomsky::TransformWorldUpdatePass::Points
chomsky::TransformWorldUpdatePass::PointCollection::EMPTY;

const chomsky::TransformWorldUpdatePass::Points*
chomsky::TransformWorldUpdatePass::PointCollection::operator ->() const
{
	if (this->points == nullptr)
	{
		return &EMPTY;
	}

	return points;
}

const chomsky::TransformWorldUpdatePass::Points&
chomsky::TransformWorldUpdatePass::PointCollection::operator *() const
{
	if (this->points == nullptr)
	{
		return EMPTY;
	}

	return *points;
}

glm::vec3 chomsky::TransformWorldUpdatePass::PointCollection::operator [](std::size_t index) const
{
	if (this->points == nullptr)
	{
		return glm::vec3(0.0f);
	}

	return (*this->points).at(index);
}
