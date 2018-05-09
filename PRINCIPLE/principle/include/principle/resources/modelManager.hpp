// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_MODEL_MANAGER_HPP
#define PRINCIPLE_RESOURCES_MODEL_MANAGER_HPP

#include "chomsky/world/worldUpdatePass.hpp"
#include "kvre/model/model.hpp"

namespace principle
{
	class ModelManager : public chomsky::WorldUpdatePass
	{
	public:
		ModelManager() = default;
		~ModelManager() = default;

		bool has(int tag);
		const kvre::Model* get(int tag);

		bool tag_has_vertex_index_mapper(int tag) const;
		const kompvter::VertexIndexMapper& get_tag_vertex_index_mapper(
			int tag) const;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;
		void invalidate_model(const kvre::Model* model) override;

	private:
		std::unordered_map<int, kompvter::VertexIndexMapper> index_mappers;
		std::unordered_map<int, const kvre::Model*> tags;
		std::unordered_map<const kvre::Model*, kvre::Model> models;
	};
}

#endif
