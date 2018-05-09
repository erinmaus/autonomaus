// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/worldUpdatePass.hpp"

void chomsky::WorldUpdatePass::start()
{
	// Nada.
}

void chomsky::WorldUpdatePass::stop()
{
	// Nada.
}

void chomsky::WorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	// Nada.
}

void chomsky::WorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	// Nada.
}

void chomsky::WorldUpdatePass::draw_water(
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	// Nada.
}

void chomsky::WorldUpdatePass::draw_particles(
	int tag, const kvre::Particles* particles,
	const kompvter::ModelDrawEvent& event)
{
	// Nada.
}

void chomsky::WorldUpdatePass::invalidate_tag(int tag)
{
	// Nada.
}

void chomsky::WorldUpdatePass::invalidate_model(const kvre::Model* model)
{
	// Nada.
}

void chomsky::WorldUpdatePass::invalidate_terrain(const kvre::Terrain* terrain)
{
	// Nada.
}

void chomsky::WorldUpdatePass::invalidate_water(const kvre::Terrain* water)
{
	// Nada.
}

void chomsky::WorldUpdatePass::invalidate_particles(const kvre::Particles* particles)
{
	// Nada.
}
