// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_CANDIDATE_MANAGER_HPP
#define CONTINENT1_CANDIDATE_MANAGER_HPP

#include <string>
#include <boost/container/flat_map.hpp>
#include "continent1/candidate.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "principle/resources/batchedModelManager.hpp"
#include "thoreau/map.hpp"

namespace continent1
{
	class CandidateManager : public chomsky::WorldUpdatePass
	{
	public:
		typedef int Tag;

		CandidateManager(
			const principle::BatchedModelManager* batched_model_manager,
			const thoreau::Map* map);
		~CandidateManager() = default;

		const glm::vec3& get_token_size() const;
		void set_token_size(const glm::vec3& value);

		float get_token_elevation() const;
		void set_token_elevation(float value);

		std::size_t count() const;
		bool empty() const;

		void reset();

		const Candidate* get(Tag candidate_tag) const;

		void save_candidate_properties(const std::string& filename) const;
		void save_tile_properties(const std::string& filename) const;

		void draw_model(
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void invalidate_model(const kvre::Model* model) override;

	private:
		static std::string get_template(const std::string& filename);

		const principle::BatchedModelManager* batched_model_manager;
		const thoreau::Map* map;

		glm::vec3 token_size = glm::vec3(512.0f, 768.0f, 512.0f);
		float token_elevation = 448.0f;

		typedef int ModelID;
		ModelID current_model_id = 0;
		std::map<ModelID, kvre::Model> models;
		std::map<const kvre::Model*, ModelID> current_models;
		std::map<Tag /* candidate */, ModelID> candidate_models;

		Tag current_candidate_tag = 0;
		typedef std::map<Tag, Candidate> Candidates;
		Candidates candidates;
		typedef boost::container::flat_multimap<thoreau::TileCoordinate, Tag> CandidatesSpatialHash;
		CandidatesSpatialHash candidates_by_tile;

	public:
		typedef Candidates::const_iterator const_iterator;
		typedef CandidatesSpatialHash::const_iterator const_space_iterator;

		const_iterator begin() const;
		const_iterator end() const;

		const_space_iterator space_begin() const;
		const_space_iterator space_begin(const thoreau::TileCoordinate& tile_coordinate) const;
		const_space_iterator space_end() const;
		const_space_iterator space_end(const thoreau::TileCoordinate& tile_coordinate) const;
	};
}

#endif
