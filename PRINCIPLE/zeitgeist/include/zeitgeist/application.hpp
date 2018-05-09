// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ZEITGEIST_APPLICATION_HPP
#define ZEITGEIST_APPLICATION_HPP

#include <set>
#include <unordered_map>
#include "chomsky/resources/textureFingerprint.hpp"
#include "chomsky/world/actorAnimation.hpp"
#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/materialSet.hpp"
#include "chomsky/world/visibleTileWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "thoreau/worldCoordinate.hpp"
#include "kompvter/graphics/textureRegion.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvncer/graphics/shader.hpp"
#include "principle/application.hpp"
#include "principle/common/window.hpp"
#include "principle/graphics/thumbnail.hpp"
#include "principle/resources/draw.hpp"
#include "principle/resources/modelSelectionManager.hpp"
#include "principle/scene/dynamicCamera.hpp"
#include "principle/scene/dynamicCameraController.hpp"
#include "principle/scene/material.hpp"
#include "principle/scene/pointMaterial.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/scene.hpp"

namespace zeitgeist
{
	class Application : public principle::Application
	{
	public:
		Application();
		~Application();

	protected:
		void update(float delta) override;
		void update_gui() override;
		void draw() override;

		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_mouse_button_up(const principle::MouseEvent& event) override;
		void on_mouse_move(const principle::MouseEvent& event) override;
		void on_mouse_scroll(const principle::MouseEvent& event) override;
		void on_key_down(const principle::KeyboardEvent& event) override;
		void on_key_up(const principle::KeyboardEvent& event) override;

	private:
		class ProxyPass : public chomsky::WorldUpdatePass
		{
		public:
			ProxyPass(Application* application);
			~ProxyPass() = default;

			void start() override;
			void stop() override;
		private:
			Application* application;
		};
		ProxyPass proxy_pass;

		void prepare_snapshot();
		void finish_snapshot();

		static const int THUMBNAIL_WIDTH = 128;
		static const int THUMBNAIL_HEIGHT = 128;
		std::unique_ptr<principle::Thumbnail> model_thumbnail;
		const kvre::Model* model_thumbnail_current_model = nullptr;
		int model_thumbnail_current_index_mapper_buffer = 0;
		std::size_t model_thumbnail_current_index_offset_start = 0;
		std::size_t model_thumbnail_current_index_offset_stop = 0;
		int model_thumbnail_current_tag = 0;
		std::unique_ptr<principle::Thumbnail> selection_thumbnail;
		bool thumbnail_was_visible = false;
		float thumbnail_time = 0.0f;

		void show_model_thumbnail(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);
		void show_model_thumbnail(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters);
		void show_model_thumbnail(int tag);
		void show_selection_thumbnail();
		void show_thumbnail(principle::Thumbnail* thumbnail);
		glm::mat4 compute_thumbnail_rotation_matrix() const;

		class VisibilityFilterWindow : public principle::Window
		{
		public:
			VisibilityFilterWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
		};
		VisibilityFilterWindow visibility_filter_window;

		class ModelDrawListWindow : public principle::Window
		{
		public:
			ModelDrawListWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			void show_model_draw(
				const kvre::Model* model,
				const principle::Draw& draw);

			Application* application;

			bool show_unclassified = true;
			bool show_scenery = true;
			bool show_props = true;
			bool show_actors = true;

			enum
			{
				group_by_call = 0,
				group_by_model
			};
			int grouping = group_by_call;
		};
		ModelDrawListWindow model_draw_list_window;

		class SelectionWindow : public principle::Window
		{
		public:
			SelectionWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			void show_selection_list();

			float x_rotation = 0.0f;
			float y_rotation = 0.0f;
			float scale = 1.0f;

			std::unique_ptr<principle::Thumbnail> thumbnail;
			std::unordered_set<principle::ModelSelection*> selections;
			kvncer::Texture* update_thumbnail();
			void show_thumbnail();

			void new_point_cloud_query();
		};
		SelectionWindow selection_window;

		class ActorDrawListWindow : public principle::Window
		{
		public:
			ActorDrawListWindow(Application* application);

			void clear();

			void update(float delta) override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			bool show_details = false;

			typedef std::shared_ptr<principle::Thumbnail> ThumbnailPointer;
			std::map<thoreau::WorldCoordinate, ThumbnailPointer> thumbnails;
			std::map<thoreau::WorldCoordinate, float> thumbnail_times;
			thoreau::WorldCoordinate current_thumbnail;

			void update_thumbnail(
				const thoreau::WorldCoordinate& location,
				const chomsky::ActorDraw& actor_draw);
			void show_thumbnail(
				thoreau::WorldCoordinate location,
				const chomsky::ActorDraw& actor_draw);
		};
		ActorDrawListWindow actor_draw_list_window;

		class AnimationBuilderWindow : public principle::Window
		{
		public:
			AnimationBuilderWindow(Application* application);

			void add(const thoreau::WorldCoordinate& location);
			void on_frame();

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
			
			chomsky::TileSpacePointCloudMatch query;
			bool extract = false;
			bool dump_meta = false;

			chomsky::ActorAnimation animation;
			float distance = 1.0f;
			float hits = 0.5f;
			void find_matches();
		};
		AnimationBuilderWindow animation_builder_window;

		class PointCloudMatchBuilderWindow : public principle::Window
		{
		public:
			PointCloudMatchBuilderWindow(Application* application);

			void add(const chomsky::TileSpacePointCloudMatch& match);
			void add(const thoreau::WorldCoordinate& location);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			struct PointCloudMatch
			{
				std::string name;
				chomsky::TileSpacePointCloudMatch value;
			};
			std::vector<PointCloudMatch> matches;
		};
		PointCloudMatchBuilderWindow point_cloud_match_builder_window;

		class MaterialsSetWindow : public principle::Window
		{
		public:
			MaterialsSetWindow(Application* application);

			void open(const chomsky::MaterialSet& materials);
			using principle::Window::open;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
			chomsky::MaterialSet materials;
		};
		MaterialsSetWindow materials_set_window;

		class MaterialSetMatchBuilderWindow : public principle::Window
		{
		public:
			MaterialSetMatchBuilderWindow(Application* application);

			void add(const chomsky::MaterialSet& materials);
			void add(const thoreau::WorldCoordinate& location);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			bool create_inclusive_match();

			Application* application;

			struct MaterialSetMatch
			{
				chomsky::MaterialSetMatch value;

				typedef std::pair<int, int> MaterialsKey;
				typedef std::shared_ptr<kvncer::Texture> MaterialsValue;
				std::vector<MaterialsKey> texture_regions;
				std::map<MaterialsKey, MaterialsValue> materials;
				std::string name;
			};
			std::vector<MaterialSetMatch> matches;

			glm::vec3 default_max_difference_threshold = glm::vec3(1.0f);
			glm::vec3 default_max_dissimilarity = glm::vec3(1.0f);
			glm::vec3 default_max_color_difference = glm::vec3(16.0f);
		};
		MaterialSetMatchBuilderWindow material_set_match_builder_window;

		class QueryResultsWindow : public principle::Window
		{
		public:
			QueryResultsWindow(Application* application);

			void add_world(const thoreau::WorldCoordinate& location);
			void add_tile(const thoreau::TileCoordinate& location);
			void clear();

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

		public:
			std::set<thoreau::WorldCoordinate> world_matches;
			std::set<thoreau::TileCoordinate> tile_matches;
		};
		QueryResultsWindow query_results_window;

		chomsky::VisibleTileWorldUpdatePass::VisibleTileState visible_tile_state;
		chomsky::VisibleTileWorldUpdatePass visible_tile_pass;

		kvlc::Bounds scene_bounds;

		principle::DynamicCamera camera;
		principle::DynamicCameraController camera_controller;

		std::vector<kvncer::Mesh*> meshes;
		std::vector<kvncer::MeshBuffer*> mesh_buffers;
		kvncer::Mesh* create_mesh_for_model(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);
		kvncer::Mesh* create_mesh_for_terrain(const principle::Draw& draw);
		void clear_meshes();

		bool draw_models = true;
		principle::Scene models_scene;
		kvncer::Shader* models_shader;
		bool draw_unclassified = true;
		principle::ModelMaterial unclassified_material;
		bool draw_scenery = true;
		principle::ModelMaterial scenery_material;
		bool draw_props = true;
		principle::ModelMaterial prop_material;
		bool draw_actors = true;
		principle::ModelMaterial actor_material;
		void add_model(
			const principle::Draw& draw,
			const kvre::Model* model,
			kvncer::Texture* texture,
			const kompvter::VertexIndexMapper& index_mapper = kompvter::VertexIndexMapper());

		bool draw_terrain = true;
		principle::Scene terrain_scene;
		kvncer::Shader* terrain_shader;
		principle::ModelMaterial terrain_material;
		void add_terrain(
			const principle::Draw& draw,
			const kvre::Terrain* terrain,
			kvncer::Texture* texture);

		bool draw_min_terrain_y = false;
		principle::Scene min_terrain_y;
		kvncer::Shader* min_terrain_y_shader;
		principle::PointMaterial min_terrain_y_material;

		bool draw_max_terrain_y = false;
		principle::Scene max_terrain_y;
		kvncer::Shader* max_terrain_y_shader;
		principle::PointMaterial max_terrain_y_material;

		bool draw_min_scenery_y = false;
		principle::Scene min_scenery_y;
		kvncer::Shader* min_scenery_y_shader;
		principle::PointMaterial min_scenery_y_material;

		bool draw_max_scenery_y = false;
		principle::Scene max_scenery_y;
		kvncer::Shader* max_scenery_y_shader;
		principle::PointMaterial max_scenery_y_material;

		bool draw_tile_point_clouds = false;
		principle::Scene tile_point_clouds;
		kvncer::Shader* tile_point_clouds_shader;
		principle::PointMaterial tile_point_clouds_material;

		bool draw_selection = false;
		principle::ModelSelectionManager selection_manager;

		principle::ModelSelection* select(int tag);

		glm::vec3 world_position = glm::vec3(0.0f);
		kvncer::Mesh sphere_mesh;
		kvncer::MeshBuffer sphere_vertex_buffer, sphere_index_buffer;

		bool export_tile_snapshots = false;
		int export_tile_layer = 0;
		void export_tile_snapshot(const thoreau::TileCoordinate& tile_coordinate);
	};
}

#endif
