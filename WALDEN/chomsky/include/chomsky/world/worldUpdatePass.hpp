// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_WORLD_UPDATE_PASS_HPP

#include "kompvter/events/modelDrawEvent.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "kvre/particles/particles.hpp"

namespace chomsky
{
	class WorldUpdatePass
	{
	public:
		virtual ~WorldUpdatePass() = default;

		virtual void start();
		virtual void stop();

		virtual void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event);
		virtual void draw_terrain(
			int tag,
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event);
		virtual void draw_water(
			int tag,
			const kvre::Terrain* water,
			const kompvter::ModelDrawEvent& event);
		virtual void draw_particles(
			int tag,
			const kvre::Particles* particles,
			const kompvter::ModelDrawEvent& event);

		virtual void invalidate_tag(int tag);
		virtual void invalidate_model(const kvre::Model* model);
		virtual void invalidate_terrain(const kvre::Terrain* terrain);
		virtual void invalidate_water(const kvre::Terrain* water);
		virtual void invalidate_particles(const kvre::Particles* particles);
	};
}

#endif
