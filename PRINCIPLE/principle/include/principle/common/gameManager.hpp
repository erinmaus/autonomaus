// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_GAME_MANAGER_HPP
#define PRINCIPLE_COMMON_GAME_MANAGER_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/resources/textureDictionary.hpp"
#include "chomsky/resources/textureKeyManager.hpp"
#include "chomsky/resources/textureFingerprintManager.hpp"
#include "chomsky/gui/itemIconManager.hpp"
#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/materialSet.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/blocks/utilityWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "glooper/map/minimap.hpp"
#include "glooper/map/minimapManager.hpp"
#include "glooper/map/mapWorldUpdateBlock.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "principle/common/loadingScreen.hpp"
#include "principle/graphics/display.hpp"
#include "principle/resources/drawManager.hpp"
#include "principle/resources/itemIconManager.hpp"
#include "principle/resources/modelPicker.hpp"
#include "principle/resources/modelManager.hpp"
#include "principle/resources/terrainManager.hpp"
#include "principle/resources/textureManager.hpp"
#include "rosalind/processors/bufferProcessor.hpp"
#include "rosalind/processors/frameProcessor.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "rosalind/processors/modelProcessor.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "rosalind/processors/textureProcessor.hpp"
#include "thoreau/map.hpp"
#include "touchstone/gl/context.hpp"

namespace principle
{
	class JaquesFrameReader;

	class GameManager
	{
	public:
		typedef chomsky::TransformWorldUpdatePass::TransformState TransformState;
		typedef chomsky::WorldState<
				thoreau::TileCoordinate,
				chomsky::TileElevationExtents>
			TileElevationState;
		typedef chomsky::WorldState<
				thoreau::TileCoordinate,
				chomsky::TileSpacePointCloud>
			TilePointCloudState;
		typedef chomsky::WorldState<
				thoreau::TileCoordinate,
				chomsky::MaterialSet>
			SceneryMaterialsState;
		typedef chomsky::WorldState<
				thoreau::WorldCoordinate,
				kvlc::Bounds>
			ActorBoundsState;
		typedef chomsky::WorldState<
				thoreau::WorldCoordinate,
				chomsky::MaterialSet>
			ActorMaterialsState;
		typedef chomsky::WorldState<
				thoreau::WorldCoordinate,
				chomsky::ActorDraw>
			ActorDrawState;
		typedef chomsky::WorldState<
				thoreau::WorldCoordinate,
				std::shared_ptr<chomsky::WorldState<thoreau::TileCoordinate, chomsky::TileSpacePointCloud>>>
			ActorPointCloudState;

		GameManager();
		~GameManager();

		kompvter::BufferManager& get_buffer_manager();
		const kompvter::BufferManager& get_buffer_manager() const;
		kompvter::TextureManager& get_texture_manager();
		const kompvter::TextureManager& get_texture_manager() const;

		touchstone::Context& get_touchstone_context();
		const touchstone::Context& get_touchstone_context() const;

		rosalind::BufferProcessor& get_buffer_processor();
		const rosalind::BufferProcessor& get_buffer_processor() const;
		rosalind::TextureProcessor& get_texture_processor();
		const rosalind::TextureProcessor& get_texture_processor() const;
		rosalind::FrameProcessor& get_frame_processor();
		const rosalind::FrameProcessor& get_frame_processor() const;
		rosalind::GUIProcessor& get_gui_processor();
		const rosalind::GUIProcessor& get_gui_processor() const;
		rosalind::ItemIconProcessor& get_item_icon_processor();
		const rosalind::ItemIconProcessor& get_item_icon_processor() const;
		rosalind::MinimapProcessor& get_minimap_processor();
		const rosalind::MinimapProcessor& get_minimap_processor() const;
		rosalind::ModelProcessor& get_model_processor();
		const rosalind::ModelProcessor& get_model_processor() const;

		const TransformState& get_transform_state() const;
		const TileElevationState& get_terrain_elevation_state() const;
		const TilePointCloudState& get_static_point_cloud_state() const;
		const SceneryMaterialsState& get_scenery_materials_state() const;
		const ActorBoundsState& get_actor_bounds_state() const;
		const ActorDrawState& get_actor_draw_state() const;
		const ActorMaterialsState& get_actor_materials_state() const;
		const ActorPointCloudState& get_actor_point_cloud_state() const;

		ModelPicker& get_model_picker();
		const ModelPicker& get_model_picker() const;

		DrawManager& get_draw_manager();
		const DrawManager& get_draw_manager() const;
		ModelManager& get_model_manager();
		const ModelManager& get_model_manager() const;
		TerrainManager& get_terrain_manager();
		const TerrainManager& get_terrain_manager() const;

		TextureManager& get_texture_resource_manager();
		const TextureManager& get_texture_resource_manager() const;
		ItemIconManager& get_item_icon_resource_manager();
		const ItemIconManager& get_item_icon_resource_manager() const;

		chomsky::TextureDictionary& get_texture_dictionary();
		const chomsky::TextureDictionary& get_texture_dictionary() const;

		chomsky::TextureKeyManager& get_texture_key_manager();
		const chomsky::TextureKeyManager& get_texture_key_manager() const;
		chomsky::TextureFingerprintManager& get_texture_fingerprint_manager();
		const chomsky::TextureFingerprintManager& get_texture_fingerprint_manager() const;
		chomsky::ItemIconManager& get_item_icon_manager();
		const chomsky::ItemIconManager& get_item_icon_manager() const;

		thoreau::Map& get_map();
		const thoreau::Map& get_map() const;
		thoreau::Map& get_runtime_map();
		const thoreau::Map& get_runtime_map() const;

		glooper::ResourceManager& get_resource_manager();
		const glooper::ResourceManager& get_resource_manager() const;

		glooper::Minimap& get_minimap();
		const glooper::Minimap& get_minimap() const;

		chomsky::WorldUpdateManager& get_world_update_manager();
		const chomsky::WorldUpdateManager& get_world_update_manager() const;
		chomsky::UtilityWorldUpdateBlock& get_utility_block();
		const chomsky::UtilityWorldUpdateBlock& get_utility_block() const;
		glooper::MapWorldUpdateBlock& get_map_block();
		const glooper::MapWorldUpdateBlock& get_map_block() const;
		chomsky::StaticWorldUpdateBlock& get_static_block();
		const chomsky::StaticWorldUpdateBlock& get_static_block() const;
		chomsky::AnimatedWorldUpdateBlock& get_animated_block();
		const chomsky::AnimatedWorldUpdateBlock& get_animated_block() const;

		void load(
			LoadingScreen& loading_screen,
			JaquesFrameReader& reader,
			std::size_t frame_num, std::size_t count = 1, std::size_t step = 1);

		void load_texture_dictionary();
		void save_texture_dictionary();

	private:
		void load_special_items();

		glooper::ResourceManager resource_manager;

		touchstone::Context context;
		rosalind::BufferProcessor buffer_processor;
		rosalind::TextureProcessor texture_processor;
		rosalind::FrameProcessor frame_processor;
		rosalind::GUIProcessor gui_processor;
		rosalind::ItemIconProcessor item_icon_processor;
		rosalind::MinimapProcessor minimap_processor;
		rosalind::ModelProcessor model_processor;

		ModelPicker model_picker;

		DrawManager draw_manager;
		ModelManager model_manager;
		TerrainManager terrain_manager;

		TextureManager texture_resource_manager;

		ItemIconManager item_icon_resource_manager;

		thoreau::Map map;
		thoreau::Map runtime_map;
		glooper::Minimap minimap;
		glooper::MinimapManager minimap_manager;

		class ProxyWorldUpdateManager : public chomsky::WorldUpdateManager
		{
		public:
			ProxyWorldUpdateManager() = default;
			~ProxyWorldUpdateManager() = default;

			void set_loading_screen(LoadingScreen& value);

		protected:
			void after_draw_terrain(
				chomsky::WorldUpdatePass* pass,
				int tag, const kvre::Terrain* terrain,
				const kompvter::ModelDrawEvent& event) override;
			void after_draw_model(
				chomsky::WorldUpdatePass* pass,
				int tag, const kvre::Model* model,
				const kompvter::ModelDrawEvent& event) override;
			void after_draw_water(
				chomsky::WorldUpdatePass* pass,
				int tag, const kvre::Terrain* water,
				const kompvter::ModelDrawEvent& event) override;
			void after_draw_particles(
				chomsky::WorldUpdatePass* pass,
				int tag, const kvre::Particles* particles,
				const kompvter::ModelDrawEvent& event) override;
			void after_start(chomsky::WorldUpdatePass* pass) override;
			void after_stop(chomsky::WorldUpdatePass* pass) override;

		private:
			LoadingScreen* loading_screen = nullptr;
		};

		ProxyWorldUpdateManager world_update_manager;
		chomsky::UtilityWorldUpdateBlock utility_world_update_block;
		glooper::MapWorldUpdateBlock map_world_update_block;
		chomsky::StaticWorldUpdateBlock static_world_update_block;
		chomsky::AnimatedWorldUpdateBlock animated_world_update_block;
	};
}

#endif
