// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_RENDER_PROFILE_HPP
#define KVNCER_RENDER_PROFILE_HPP

#include "kompvter/events/drawEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"

namespace kvncer
{
	class Mesh;

	class Profile
	{
	public:
		virtual ~Profile() = default;

		virtual void make_model(
			Mesh& mesh, const kompvter::ModelDrawEvent& event) = 0;
		virtual void make_terrain(
			Mesh& mesh, const kompvter::ModelDrawEvent& event) = 0;
		virtual void make_water(
			Mesh& mesh, const kompvter::ModelDrawEvent& event) = 0;
	};
}

#endif
