// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include "continent1/candidateManager.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "chomsky/world/detail/prop.hpp"

continent1::CandidateManager::CandidateManager(
	const principle::BatchedModelManager* batched_model_manager,
	const thoreau::Map* map)
{
	this->batched_model_manager = batched_model_manager;
	this->map = map;
}

const glm::vec3& continent1::CandidateManager::get_token_size() const
{
	return this->token_size;
}

void continent1::CandidateManager::set_token_size(const glm::vec3& value)
{
	this->token_size = value;
}

float continent1::CandidateManager::get_token_elevation() const
{
	return this->token_elevation;
}

void continent1::CandidateManager::set_token_elevation(float value)
{
	this->token_elevation = value;
}

std::size_t continent1::CandidateManager::count() const
{
	return this->candidates.size();
}

bool continent1::CandidateManager::empty() const
{
	return this->candidates.empty();
}

void continent1::CandidateManager::reset()
{
	this->current_model_id = 0;
	this->models.clear();
	this->current_models.clear();
	this->candidate_models.clear();

	this->current_candidate_tag = 0;
	this->candidates.clear();
	this->candidates_by_tile.clear();
}

const continent1::Candidate* continent1::CandidateManager::get(Tag candidate_tag) const
{
	auto iter = this->candidates.find(candidate_tag);
	if (iter != this->candidates.end())
	{
		return &iter->second;
	}

	return nullptr;
}

void continent1::CandidateManager::save_candidate_properties(
	const std::string& filename) const
{
	std::ofstream stream(filename);
	stream << get_template("candidate_properties") << std::endl;
	stream << std::fixed << std::setprecision(0);

	static const std::string SEP = ", ";
	for (auto& i: this->candidates)
	{
		auto tag = i.first;
		auto& candidate = i.second;

		stream << tag << SEP;
		stream << candidate.get_max_triangle_surface_area() << SEP;
		stream << candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_x) << SEP;
		stream << candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_y) << SEP;
		stream << candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_z) << SEP;
		stream << candidate.get_surface_area_sum() << SEP;
		stream << candidate.get_surface_area_sum_by_normal(Candidate::normal_x) << SEP;
		stream << candidate.get_surface_area_sum_by_normal(Candidate::normal_y) << SEP;
		stream << candidate.get_surface_area_sum_by_normal(Candidate::normal_z) << SEP;
		stream << candidate.get_bounding_box_volume() << SEP;
		stream << candidate.get_surface_area_bounding_box_volume_ratio() * 512 << SEP;
		stream << candidate.get_num_vertices() << SEP;
		stream << candidate.get_num_unique_vertices() << SEP;
		stream << candidate.get_num_triangles() << SEP;
		stream << std::endl;
	}
}

void continent1::CandidateManager::save_tile_properties(
	const std::string& filename) const
{
	std::ofstream stream(filename);
	stream << get_template("tile_properties") << std::endl;
	stream << std::fixed << std::setprecision(0);

	static const std::string SEP = ", ";
	for (auto& i: this->candidates_by_tile)
	{
		auto tile_coordinate = i.first;
		auto tag = i.second;
		auto& candidate = *get(tag);

		stream << tag << SEP;
		stream << tile_coordinate.x << SEP;
		stream << tile_coordinate.y << SEP;
		stream << tile_coordinate.layer << SEP;
		stream << candidate.get_num_vertices(tile_coordinate) << SEP;
		stream << candidate.get_num_unique_vertices(tile_coordinate) << SEP;
		stream << candidate.get_num_triangles(tile_coordinate) << SEP;
		stream << candidate.get_num_token_intersections(tile_coordinate) << SEP;
		stream << candidate.get_num_triangles_on_side(tile_coordinate, Candidate::side_left) << SEP;
		stream << candidate.get_num_triangles_on_side(tile_coordinate, Candidate::side_right) << SEP;
		stream << candidate.get_num_triangles_on_side(tile_coordinate, Candidate::side_top) << SEP;
		stream << candidate.get_num_triangles_on_side(tile_coordinate, Candidate::side_bottom) << SEP;
		stream << std::endl;
	}
}

void continent1::CandidateManager::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (!this->batched_model_manager->has(tag))
	{
		return;
	}

	if (!chomsky::is_scenery_draw_event(event) && !chomsky::is_prop_draw_event(event))
	{
		return;
	}

	ModelID model_id;
	if (this->current_models.count(model) != 0)
	{
		model_id = this->current_models[model];
	}
	else
	{
		model_id = ++this->current_model_id;
		this->models.emplace(model_id, *model);
		this->current_models.emplace(model, model_id);
	}

	auto draws = this->batched_model_manager->get(tag);
	for (auto& draw: *draws)
	{
		Candidate candidate(&this->models.find(model_id)->second, draw, event.get_model_matrix());
		candidate.populate(*this->map, this->token_size, this->token_elevation);
		candidate.set_tag(tag);

		auto candidate_tag = ++this->current_candidate_tag;
		this->candidates.emplace(candidate_tag, candidate);
		this->candidate_models.emplace(candidate_tag, model_id);

		auto min = candidate.get_min_tile();
		auto max = candidate.get_max_tile();
		for (int x = min.x; x <= max.x; ++x)
		{
			for (int y = min.y; y <= max.y; ++y)
			{
				for (int layer = min.layer; layer <= max.layer; ++layer)
				{
					auto tile_coordinate = thoreau::TileCoordinate(x, y, layer);
					this->candidates_by_tile.emplace(tile_coordinate, candidate_tag);
				}
			}
		}
	}
}

void continent1::CandidateManager::invalidate_model(const kvre::Model* model)
{
	this->current_models.erase(model);
}

continent1::CandidateManager::const_iterator
continent1::CandidateManager::begin() const
{
	return this->candidates.begin();
}

continent1::CandidateManager::const_iterator
continent1::CandidateManager::end() const
{
	return this->candidates.end();
}

continent1::CandidateManager::const_space_iterator
continent1::CandidateManager::space_begin() const
{
	return this->candidates_by_tile.begin();
}

continent1::CandidateManager::const_space_iterator
continent1::CandidateManager::space_begin(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->candidates_by_tile.lower_bound(tile_coordinate);
}

continent1::CandidateManager::const_space_iterator
continent1::CandidateManager::space_end() const
{
	return this->candidates_by_tile.end();
}

continent1::CandidateManager::const_space_iterator
continent1::CandidateManager::space_end(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->candidates_by_tile.upper_bound(tile_coordinate);
}

std::string continent1::CandidateManager::get_template(const std::string& filename)
{
	auto root_path = std::getenv("HDT_SHARE");
	if (root_path == nullptr)
	{
		return "";
	}

	std::string share_path = root_path;
	share_path += "/principle/continent1/";
	share_path += filename;
	share_path += ".csv.tpl";

	std::ifstream stream(share_path);
	auto result = std::string(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>());

	return result;
}
