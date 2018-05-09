// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_DRAW_MANAGER_HPP
#define PRINCIPLE_RESOURCES_DRAW_MANAGER_HPP

#include <unordered_map>
#include "chomsky/world/worldUpdatePass.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "kvre/particles/particles.hpp"
#include "principle/resources/draw.hpp"

namespace principle
{
	class DrawManager : public chomsky::WorldUpdatePass
	{
	public:
		DrawManager() = default;
		~DrawManager() = default;

		bool has(int tag) const;
		const Draw& get(int tag) const;

		std::size_t count() const;

		int get_draw_call_tag(std::size_t call_num) const;
		const Draw& get_draw_call(std::size_t call_num) const;

		void reset();

		void start() override;

		void draw_model(
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void draw_terrain(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;
		void draw_water(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;
		void draw_particles(
			int tag, const kvre::Particles* particles,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		std::size_t current_call = 0;

		typedef std::unordered_map<int, Draw> DrawCollection;
		DrawCollection tags;

		std::vector<int> draws;

	public:
		typedef DrawCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
