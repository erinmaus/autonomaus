// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT7_APPLICATION_HPP
#define CONTINENT7_APPLICATION_HPP

#include <map>
#include <vector>
#include "continent1/candidateManager.hpp"
#include "continent1/mapSnapshot.hpp"
#include "continent1/tileSnapshot.hpp"
#include "continent7/clippedModelMaterial.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/visibleTileWorldUpdatePass.hpp"
#include "chomsky/world/sceneryElevationWorldUpdatePass.hpp"
#include "kvlc/collision/scene.hpp"
#include "principle/application.hpp"
#include "principle/common/view.hpp"
#include "principle/graphics/thumbnail.hpp"
#include "principle/scene/dynamicCamera.hpp"
#include "principle/scene/dynamicCameraController.hpp"
#include "principle/scene/material.hpp"
#include "principle/scene/scene.hpp"
#include "thoreau/map.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace continent7
{
	class Application : public principle::Application
	{
	public:
		Application();
		~Application();

	protected:
		void update(float delta) override;
		void draw() override;
		void update_gui() override;

		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_mouse_button_up(const principle::MouseEvent& event) override;
		void on_mouse_move(const principle::MouseEvent& event) override;
		void on_mouse_scroll(const principle::MouseEvent& event) override;
		void on_key_down(const principle::KeyboardEvent& event) override;
		void on_key_up(const principle::KeyboardEvent& event) override;
		void on_start_trace() override;
		void on_finish_trace() override;

	private:
		class ProxyPass : public chomsky::WorldUpdatePass
		{
		public:
			ProxyPass(Application* application);
			~ProxyPass() = default;

			void start() override;
			void stop() override;

			void draw_model(
				int tag, const kvre::Model* model,
				const kompvter::ModelDrawEvent& event) override;
			void draw_terrain(
				int tag, const kvre::Terrain* terrain,
				const kompvter::ModelDrawEvent& event) override;
			void draw_water(
				int tag, const kvre::Terrain* terrain,
				const kompvter::ModelDrawEvent& event) override;

		private:
			std::vector<glm::vec3> transform_unskinned_model(
				const kvre::Model* model,
				const kompvter::ModelDrawEvent& event);
			std::vector<glm::vec3> transform_terrain(
				const kvre::Terrain* terrain,
				const kompvter::ModelDrawEvent& event);
			void add_model(
				int tag,
				int draw_type,
				const std::vector<glm::vec3>& points,
				const kompvter::ModelDrawEvent& event);
			Application* application;
		};
		ProxyPass proxy_pass;

		class Continent7Window : public principle::Window
		{
		public:
			Continent7Window(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			void import_map();
		};
		Continent7Window main_window;

		class ExportWindow : public principle::Window
		{
		public:
			ExportWindow(Application* application);

			void open() override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			bool export_elevation = true;
			bool export_flags = true;
			enum
			{
				mode_merge = 0,
				mode_replace = 1
			};
			int export_mode = mode_merge;
			std::string destination;
			bool only_modified = true;

			void export_map();
		};
		ExportWindow export_window;

		class EditTileWindow : public principle::Window
		{
		public:
			EditTileWindow(Application* application);
			~EditTileWindow() = default;

			void close() override;

			void set_tile(const thoreau::TileCoordinate& tile_coordinate);
			void add_tile(const thoreau::TileCoordinate& tile_coordinate);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			void set_value(const std::function<void(thoreau::Tile&)>& callback);

			std::set<thoreau::TileCoordinate> tiles;

			float current_min_elevation = 0.0f;
			float current_max_elevation = 0.0f;
			float current_height = 0.0f;
			int current_flags = thoreau::Tile::flag_none;

		};
		EditTileWindow edit_tile_window;

		class CandidateQueryWindow : public principle::Window
		{
		public:
			CandidateQueryWindow(Application* application);
			~CandidateQueryWindow() = default;

			void open(const thoreau::TileCoordinate& tile_coordinate);
			void update(float delta) override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			thoreau::TileCoordinate tile_coordinate;
			principle::Thumbnail thumbnail;
			int thumbnail_candidate = 0;
			thoreau::TileCoordinate thumbnail_tile_coordinate;
			float thumbnail_time = 0.0f;
			void preview_candidate(int tag);
			void preview_candidate(int tag, const thoreau::TileCoordinate& tile_coordinate);
			void show_candidate();
		};
		CandidateQueryWindow candidate_query_window;

		class Toolbox
		{
		public:
			Toolbox(Application* application);
			~Toolbox() = default;

			void neighbor_elevations_from_ray(
				const thoreau::TileCoordinate& tile_coordinate,
				float elevation_hint, float neighbor_offset,
				bool make_passable);
			void elevation_from_ray(
				const thoreau::TileCoordinate& tile_coordinate,
				float elevation_hint,
				bool make_passable);

			void mark_impassable(
				const thoreau::TileCoordinate& tile_coordinate);
			void mark_passable(
				const thoreau::TileCoordinate& tile_coordinate);

			void undo();
			void push_undo_stack();
			void clear_undo_stack();
			std::size_t get_undo_stack_size() const;

		private:
			void do_neighbor_elevations_from_ray(
				const thoreau::TileCoordinate& tile_coordinate,
				float elevation_hint, float neighbor_offset,
				bool make_passable, int tag,
				std::set<thoreau::TileCoordinate>& visited);
			float test_elevation_from_ray(
				const thoreau::TileCoordinate& tile_coordinate,
				float elevation_hint, int& result_tag);
			void set_elevation(
				const thoreau::TileCoordinate& tile_coordinate,
				float value);
			void mark_dirty(const thoreau::TileCoordinate& tile_coordinate);

			std::stack<thoreau::Map> undo_stack;

			Application* application;
		};

		bool automatic_enabled = false;

		int current_layer = 0;
		bool override_layer = false;
		float max_elevation_deviation = 192.0f;
		float tile_elevation_range = 512.0f;
		bool make_passable = true;
		bool restrain_tag = true;
		bool player_sector_only = true;
		bool include_adjacent = true;
		glm::vec3 token_size = glm::vec3(128.0f, 256.0f, 128.0f);
		float token_elevation = 512.0f;
		Toolbox toolbox;

		int is_multiselection = 0;

		std::vector<kvncer::Mesh*> meshes;
		std::vector<kvncer::MeshBuffer*> mesh_buffers;
		kvncer::Mesh* create_mesh_for_model(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);
		kvncer::Mesh* create_mesh_for_terrain(const kvre::Terrain* terrain);
		void clear_meshes();

		bool draw_scenes = true;
		kvncer::Shader* scene_shader;
		principle::ModelMaterial scene_material;
		std::vector<std::unique_ptr<principle::Scene>> scenes;
		void populate_scene();
		void add_model(
			const principle::Draw& draw,
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper = kompvter::VertexIndexMapper());
		void add_terrain(
			const principle::Draw& draw,
			const kvre::Terrain* terrain);

		typedef std::unique_ptr<kvncer::Texture> TexturePointer;
		std::map<thoreau::SectorCoordinate, TexturePointer> sector_elevation_textures;
		std::map<thoreau::SectorCoordinate, TexturePointer> sector_wall_textures;
		std::set<thoreau::SectorCoordinate> dirty_sectors;
		std::set<thoreau::TileCoordinate> modified_tiles;
		void notify_map_update();
		void update_sector_elevations_texture(
			const thoreau::SectorCoordinate& sector_coordinate);
		void update_sector_walls_texture(
			const thoreau::SectorCoordinate& sector_coordinate);
		void apply_tile_heuristic();

		std::map<thoreau::SectorCoordinate, bool> visible_sectors;
		std::set<thoreau::SectorCoordinate> player_sectors;
		std::set<thoreau::TileCoordinate> visited_tiles;

		chomsky::VisibleTileWorldUpdatePass::VisibleTileState visible_tile_state;
		chomsky::VisibleTileWorldUpdatePass visible_tile_pass;

		chomsky::SceneryElevationWorldUpdatePass::SceneryElevationState scenery_elevation_state;
		chomsky::SceneryElevationWorldUpdatePass::SceneryElevationState relative_scenery_elevation_state;
		chomsky::SceneryElevationWorldUpdatePass scenery_elevation_pass;

		principle::DynamicCamera camera;
		principle::DynamicCameraController camera_controller;

		glm::vec2 mouse_position;
		bool current_tile_dirty = false;
		bool show_current_tile_info = false;
		thoreau::TileCoordinate current_tile_coordinate = thoreau::TileCoordinate(0, 0);
		float current_tile_elevation = -HUGE_VALF;
		void update_current_tile();
		thoreau::TileCoordinate get_current_tile_coordinate(bool override_layer);
		float get_current_tile_elevation();

		int current_frame = 0;
		kvlc::Bounds bounds;
		kvlc::Scene map_collision_scene;
		std::map<int /* scene tag */, int /* draw type */> map_collision_scene_types;
		std::map<int /* scene tag */, std::pair<int /* frame */, int /* tag */>> map_collision_scene_tags;

		principle::BatchedModelManager batched_model_manager;
		continent1::CandidateManager candidate_manager;

		kvncer::Mesh box_mesh;
		kvncer::MeshBuffer box_vertex_buffer;
		kvncer::MeshBuffer box_index_buffer;
		void draw_box(const glm::vec3& position);
	};
}

#endif
