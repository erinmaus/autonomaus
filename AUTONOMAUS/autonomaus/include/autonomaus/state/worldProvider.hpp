// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_WORLD_PROVIDER_HPP
#define AUTONOMAUS_STATE_WORLD_PROVIDER_HPP

#include <future>
#include <memory>
#include "autonomaus/state/world.hpp"
#include "autonomaus/state/gameStateProvider.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/blocks/utilityWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "glooper/map/mapWorldUpdateBlock.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvlc/geometry/ray.hpp"
#include "kvlc/collision/query.hpp"
#include "rosalind/common/updater.hpp"

namespace autonomaus
{
	class WorldProvider : public GameStateProvider<World, WorldProvider, GameStateIndependentChannel>
	{
	public:
		typedef GameStateProvider<World, WorldProvider, GameStateIndependentChannel> Base;
		using Base::Slot;

		WorldProvider() = default;
		~WorldProvider() = default;

		chomsky::WorldUpdateManager* get_world_update_manager();
		const chomsky::UtilityWorldUpdateBlock* get_utility_block() const;
		const glooper::MapWorldUpdateBlock* get_map_block() const;
		const chomsky::StaticWorldUpdateBlock* get_static_block() const;
		const chomsky::AnimatedWorldUpdateBlock* get_animated_block() const;

		const chomsky::StaticWorldUpdateBlock::DirtyTileState* get_dirty_tile_state() const;

		void attach(GameStateManager& manager, Slot& slot) override;
		void detach() override;

		void update() override;

		static std::future<kvlc::Query> test_collision(
			GameStateManager& game_state,
			const View& view,
			const glm::vec3& center, float radius);
		static std::future<kvlc::Query> test_collision(
			GameStateManager& game_state,
			const View& view,
			const kvlc::Bounds& bounds);
		static std::future<kvlc::Query> test_collision(
			GameStateManager& game_state,
			const View& view,
			const kvlc::Ray& ray, float length);
		static std::future<kvlc::Query> test_spray(
			GameStateManager& game_state,
			const View& view,
			const glm::vec3& center,
			float radius,
			int count);

	private:
		class DrawTypeWorldUpdatePass : public chomsky::WorldUpdatePass
		{
		public:
			typedef std::map<int, int> TagDrawTypeState;
			DrawTypeWorldUpdatePass(TagDrawTypeState* tag_draw_type_state);
			~DrawTypeWorldUpdatePass() = default;

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
			TagDrawTypeState* draws;
		};

		struct WorldUpdater
		{
			WorldUpdater(GameStateManager& manager);
			~WorldUpdater();

			rosalind::Updater& updater;
			chomsky::WorldUpdateManager world_update_manager;
			chomsky::UtilityWorldUpdateBlock utility_block;
			glooper::MapWorldUpdateBlock map_block;
			chomsky::StaticWorldUpdateBlock::DirtyTileState dirty_tile_state;
			chomsky::StaticWorldUpdateBlock static_block;
			chomsky::AnimatedWorldUpdateBlock animated_block;
			DrawTypeWorldUpdatePass::TagDrawTypeState tag_draw_type_state;
			DrawTypeWorldUpdatePass draw_type_pass;
		};
		std::unique_ptr<WorldUpdater> world_updater;
		void make_world_updater();
	};
}

#endif
