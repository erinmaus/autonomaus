// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_ANIMATION_HPP
#define CHOMSKY_WORLD_ACTOR_ANIMATION_HPP

#include <vector>
#include <glm/glm.hpp>
#include "chomsky/io/world/actorAnimation.hpp"
#include "chomsky/world/actorDraw.hpp"

namespace chomsky
{
	class ActorAnimation
	{
	public:
		friend class mantorok::DATSerializer<ActorAnimation>;
		friend class mantorok::DATDeserializer<ActorAnimation>;

		static const float UNIT;

		ActorAnimation() = default;
		~ActorAnimation() = default;

		float get_hit_ratio_threshold() const;
		void set_hit_ratio_threshold(float value);

		float get_distance_threshold() const;
		void set_distance_threshold(float value);

		void extract(const chomsky::ActorDraw& draw);

		bool match(const ActorAnimation& other) const;

		std::size_t get_num_frames() const;

		std::size_t clean();

		void clear();

	private:
		typedef std::vector<glm::vec3> Frame;
		bool match(
			const Frame& query,
			const Frame& frame) const;

		std::vector<Frame> frames;
		float hits = 0.5f;
		float distance = 1.0f;
	};
}

#endif
