// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_WORLD_UPDATE_MANAGER_HPP
#define CHOMSKY_WORLD_WORLD_UPDATE_MANAGER_HPP

#include <functional>
#include <set>
#include <queue>
#include <unordered_map>
#include <vector>
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/detail/drawResourceCollection.hpp"
#include "chomsky/world/detail/tagManager.hpp"
#include "kompvter/events/bufferEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/listeners/bufferListener.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "kompvter/listeners/modelListener.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "kvre/particles/particles.hpp"

namespace chomsky
{
	class WorldUpdateManager :
		public kompvter::BufferListener,
		public kompvter::ModelListener,
		public kompvter::FrameListener
	{
	public:
		WorldUpdateManager() = default;
		virtual ~WorldUpdateManager() = default;

		void add_pass(WorldUpdatePass* pass);
		void remove_pass(WorldUpdatePass* pass);

		void begin_update();
		void end_update();
		void update_pass(WorldUpdatePass* pass);
		void update();

		void reset();

		void on_buffer_create(const kompvter::BufferEvent& event) override;
		void on_buffer_delete(const kompvter::BufferEvent& event) override;
		void on_buffer_allocate(const kompvter::BufferEvent& event) override;
		void on_buffer_clear(const kompvter::BufferEvent& event) override;
		void on_buffer_update(const kompvter::BufferEvent& event) override;

		void on_draw_enabled(float timestamp) override;
		void on_draw_disabled(float timestamp) override;

		void on_draw_model(const kompvter::ModelDrawEvent& event) override;
		void on_draw_terrain(const kompvter::ModelDrawEvent& event) override;
		void on_draw_water(const kompvter::ModelDrawEvent& event) override;
		void on_draw_particle(const kompvter::ModelDrawEvent& event) override;

	protected:
		virtual void after_draw_terrain(
			WorldUpdatePass* pass,
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event);
		virtual void after_draw_model(
			WorldUpdatePass* pass,
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event);
		virtual void after_draw_water(
			WorldUpdatePass* pass,
			int tag, const kvre::Terrain* water,
			const kompvter::ModelDrawEvent& event);
		virtual void after_draw_particles(
			WorldUpdatePass* pass,
			int tag, const kvre::Particles* particles,
			const kompvter::ModelDrawEvent& event);
		virtual void after_start(WorldUpdatePass* pass);
		virtual void after_stop(WorldUpdatePass* pass);

	private:
		TagManager tag_manager;
		DrawResourceCollection<kvre::Model> models;
		DrawResourceCollection<kvre::Terrain> terrains;
		DrawResourceCollection<kvre::Terrain> waters;
		DrawResourceCollection<kvre::Particles> particles;

		void invalidate_buffer(int buffer);

		typedef std::pair<kompvter::ModelDrawEvent, kvre::Model*> ModelDraw;
		std::vector<ModelDraw> model_draws;

		typedef std::pair<kompvter::ModelDrawEvent, kvre::Terrain*> TerrainDraw;
		std::vector<TerrainDraw> terrain_draws;

		typedef std::pair<kompvter::ModelDrawEvent, kvre::Terrain*> WaterDraw;
		std::vector<WaterDraw> water_draws;

		typedef std::pair<kompvter::ModelDrawEvent, kvre::Particles*> ParticleDraw;
		std::vector<ParticleDraw> particle_draws;

		bool is_updating = false;
		std::vector<WorldUpdatePass*> passes;
	};
}

#endif
