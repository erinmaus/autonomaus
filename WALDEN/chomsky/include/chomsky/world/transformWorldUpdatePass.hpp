// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_TRANSFORM_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_TRANSFORM_WORLD_UPDATE_PASS_HPP

#include <future>
#include <unordered_map>
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"

namespace chomsky
{
	class TransformWorldUpdatePass : public WorldUpdatePass
	{
	public:
		struct PointCollection;
		typedef WorldState<int, PointCollection> TransformState;

		TransformWorldUpdatePass(TransformState* transform_state);
		~TransformWorldUpdatePass() = default;

		void stop() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void draw_terrain(
			int tag,
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;
		void draw_water(
			int tag,
			const kvre::Terrain* water,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;
		void invalidate_model(const kvre::Model* model) override;
		void invalidate_terrain(const kvre::Terrain* terrain) override;
		void invalidate_water(const kvre::Terrain* water) override;

	private:
		TransformState* state;

		typedef std::vector<glm::vec3> Points;
		std::unordered_map<const void*, Points> resources;

		std::vector<std::future<void>> pending;

	public:
		struct PointCollection
		{
		public:
			PointCollection() = default;
			PointCollection(const Points& points);
			~PointCollection() = default;

			const Points* operator ->() const;
			const Points& operator *() const;
			glm::vec3 operator [](std::size_t index) const;

		private:
			static const Points EMPTY;
			const Points* points = nullptr;
		};
	};
}

#endif
