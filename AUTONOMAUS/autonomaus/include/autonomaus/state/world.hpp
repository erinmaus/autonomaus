// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_WORLD_HPP
#define AUTONOMAUS_STATE_WORLD_HPP

#include <cmath>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include "kvlc/geometry/bounds.hpp"
#include "chomsky/world/actorAnimation.hpp"
#include "chomsky/world/worldState.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace autonomaus
{
	class World
	{
	public:
		friend class WorldProvider;

		World() = default;
		~World() = default;

		enum
		{
			draw_type_none = 0,
			draw_type_terrain,
			draw_type_water,
			draw_type_particles,
			draw_type_scenery,
			draw_type_prop,
			draw_type_actor,
			draw_type_unclassified,
			draw_type_max
		};
		int get_draw_type(int tag) const;

		struct Animated
		{
			glm::mat4 model_matrix;
			kvlc::Bounds bounds;
			chomsky::ActorAnimation animation;
		};
		typedef chomsky::WorldState<thoreau::WorldCoordinate, Animated> AnimatedState;
		const AnimatedState& get_animated_state() const;

		struct Metric
		{
			bool valid = false;
			float timestamp = 0.0f;
			int count = 0;
			float min = 0.0f;
			float median = 0.0f;
			float max = 0.0f;
		};
		Metric get_second_metric() const;
		Metric get_minute_metric() const;
		Metric get_hour_metric() const;

	private:
		std::map<int /* tag */, int /* draw_type */> draw_types;

		AnimatedState animated_state;

		void update(float timestamp, float frame_time);

		std::vector<float> second_updates;
		float last_second_update = HUGE_VALF;
		void bubble_second(float timestamp);

		std::vector<Metric> minute_updates;
		float last_minute_update = HUGE_VALF;
		void bubble_minute(float timestamp);

		float last_hour_update = HUGE_VALF;
		void bubble_hour(float timestamp);

		static Metric compute_metric(std::vector<Metric> values);

		Metric second_metric;
		Metric minute_metric;
		Metric hour_metric;
	};
}

#endif
