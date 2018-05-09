// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_RENDER_PROFILES_V1_PROFILE_HPP
#define KVNCER_RENDER_PROFILES_V1_PROFILE_HPP

#include "kvncer/render/bufferStore.hpp"
#include "kvncer/render/profile.hpp"

namespace kvncer
{
	class BufferStore;

	// This profile was valid as of April 10 2017.
	class V1Profile : public Profile
	{
	public:
		V1Profile(BufferStore& buffer_store);

		void make_model(Mesh& mesh, const kompvter::ModelDrawEvent& event);
		void make_terrain(Mesh& mesh, const kompvter::ModelDrawEvent& event);
		void make_water(Mesh& mesh, const kompvter::ModelDrawEvent& event);

	private:
		BufferStore& buffer_store;
	};
}

#endif
