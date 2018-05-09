// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_WORLD_TAG_MANAGER_HPP
#define CHOMSKY_WORLD_WORLD_TAG_MANAGER_HPP

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <glm/glm.hpp>
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"

namespace chomsky
{
	struct TagManagerDrawInvocation;
}

namespace std
{
	template <>
	struct hash<chomsky::TagManagerDrawInvocation>
	{
		std::size_t operator()(const chomsky::TagManagerDrawInvocation& value) const;
	};

	template <>
	struct equal_to<chomsky::TagManagerDrawInvocation>
	{
		std::size_t operator()(
			const chomsky::TagManagerDrawInvocation& a,
			const chomsky::TagManagerDrawInvocation& b) const;
	};
}

namespace chomsky
{
	class WorldUpdateManager;

	// XXX: There may be the most minute chance of a collision.
	//
	// Imagine...
	//
	// A draw is made with whatever state, 'Foo'. The buffer representing Foo is
	// modified. Another draw, 'Bar', is made with otherwise the same
	// parameters, including the world position. Collision! (Assuming all of
	// these changes happened within a single frame).
	//
	// In honesty, it's unlikely buffers are ever modified when drawing the
	// world. But if they are... What are the chances of a collision?
	// Astronomical, definitely, but still possible!
	struct TagManagerDrawInvocation
	{
		TagManagerDrawInvocation() = default;
		~TagManagerDrawInvocation() = default;

		void from_draw_model(const kompvter::ModelDrawEvent& event);
		void from_draw_terrain(const kompvter::ModelDrawEvent& event);
		void from_draw_water(const kompvter::ModelDrawEvent& event);
		void from_draw_particle(const kompvter::ModelDrawEvent& event);

		enum
		{
			draw_event_none = 0,
			draw_event_model,
			draw_event_terrain,
			draw_event_water,
			draw_event_particle
		};

		int type = draw_event_none;
		int position_buffer_name = 0;
		int index_buffer_name = 0;
		std::size_t index_buffer_offset_start = 0, index_buffer_offset_stop = 0;
		glm::ivec3 world_position = glm::ivec3(0);

		std::size_t hash() const;
		bool operator ==(const TagManagerDrawInvocation& other) const;
	};

	class TagManager
	{
	public:
		TagManager() = default;
		~TagManager() = default;

		int tag_model(const kompvter::ModelDrawEvent& event);
		int tag_terrain(const kompvter::ModelDrawEvent& event);
		int tag_water(const kompvter::ModelDrawEvent& event);
		int tag_particle(const kompvter::ModelDrawEvent& event);

		void for_each_invalid_tag(std::function<void(int)> on_invalidate);

		void invalidate_all();

	private:
		int get_tag(const TagManagerDrawInvocation& invocation);

		int next = 1;
		std::unordered_map<int, TagManagerDrawInvocation> used;
		std::unordered_map<TagManagerDrawInvocation, int> tags;
		std::unordered_set<int> active;
		std::queue<int> free;
	};
}

inline std::size_t std::hash<chomsky::TagManagerDrawInvocation>::operator ()(
	const chomsky::TagManagerDrawInvocation& value) const
{
	return value.hash();
}

inline std::size_t std::equal_to<chomsky::TagManagerDrawInvocation>::operator ()(
	const chomsky::TagManagerDrawInvocation& a,
	const chomsky::TagManagerDrawInvocation& b) const
{
	return a == b;
}

#endif
