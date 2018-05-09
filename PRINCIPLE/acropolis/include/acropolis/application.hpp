// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ACROPOLIS_APPLICATION_HPP
#define ACROPOLIS_APPLICATION_HPP

#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "continent1/candidate.hpp"
#include "continent1/candidateManager.hpp"
#include "chomsky/world/visibleTileWorldUpdatePass.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "principle/application.hpp"
#include "principle/common/view.hpp"
#include "principle/common/viewManager.hpp"
#include "principle/graphics/thumbnail.hpp"
#include "principle/resources/batchedModelManager.hpp"
#include "principle/scene/dynamicCamera.hpp"
#include "principle/scene/dynamicCameraController.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/scene.hpp"

namespace acropolis
{
	class Application : public principle::Application
	{
	public:
		Application();
		~Application() = default;

	protected:
		void update(float delta) override;
		void draw() override;

		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_mouse_button_up(const principle::MouseEvent& event) override;
		void on_mouse_move(const principle::MouseEvent& event) override;
		void on_mouse_scroll(const principle::MouseEvent& event) override;
		void on_key_down(const principle::KeyboardEvent& event) override;
		void on_key_up(const principle::KeyboardEvent& event) override;
		void on_start_trace() override;
		void on_finish_trace() override;

	private:
		kvlc::Bounds scene_bounds;

		bool draw_models_scene = true;
		principle::Scene models_scene;
		bool draw_terrain_scene = true;
		principle::Scene terrain_scene;
		kvncer::Shader* scene_shader;
		principle::ModelMaterial scene_material;
		bool is_camera_focus_dirty = false;

		class AcropolisWindow : public principle::Window
		{
		public:
			AcropolisWindow(Application* application);
			~AcropolisWindow() = default;

		private:
			std::string get_title() const override;
			void update_gui() override;

			Application* application;
			int tile_x = 0, tile_y = 0, tile_layer = 0;
		};
		AcropolisWindow acropolis_window;

		class CandidateQueryWindow : public principle::Window
		{
		public:
			CandidateQueryWindow(Application* application);
			~CandidateQueryWindow() = default;

			void update(float delta) override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			principle::Thumbnail thumbnail;
			int thumbnail_candidate = 0;
			thoreau::TileCoordinate thumbnail_tile_coordinate;
			float thumbnail_time = 0.0f;
			void preview_candidate(int tag);
			void preview_candidate(int tag, const thoreau::TileCoordinate& tile_coordinate);
			void show_candidate();
		};
		CandidateQueryWindow candidate_query_window;

		class PassableTileQuestionWindow : public principle::Window
		{
		public:
			PassableTileQuestionWindow(Application* application);
			~PassableTileQuestionWindow() = default;

			void open() override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			void update_views();

			Application* application;

			typedef std::tuple<int, int, int> LayerXYTuple;
			std::vector<LayerXYTuple> tiles;
			std::size_t current_index;
			bool is_dirty = false;
		};
		PassableTileQuestionWindow passable_tile_question_window;

		class SceneView : public principle::View
		{
		public:
			SceneView(Application* application);
			~SceneView() = default;

			void focus(const thoreau::TileCoordinate& tile_coordinate);

			void update(float delta) override;
			void draw() override;

			void on_mouse_button_down(const principle::MouseEvent& event) override;
			void on_mouse_button_up(const principle::MouseEvent& event) override;
			void on_mouse_move(const principle::MouseEvent& event) override;
			void on_mouse_scroll(const principle::MouseEvent& event) override;
			void on_key_down(const principle::KeyboardEvent& event) override;
			void on_key_up(const principle::KeyboardEvent& event) override;

		private:
			glm::vec3 light_position = glm::vec3(0.0f);
			glm::vec3 light_color = glm::vec3(1.0f);
			float light_intensity = 1.0f;
			float light_ambience = 0.5f;

			kvncer::Mesh box_mesh;
			kvncer::MeshBuffer box_vertex_buffer;
			kvncer::MeshBuffer box_index_buffer;
			glm::vec3 box_position = glm::vec3(0.0f);
			bool box_visible = true;
			void make_box();
			void draw_box(const glm::vec3& position);
			void draw_box();

			Application* application;
			principle::DynamicCamera camera;
			principle::DynamicCameraController camera_controller;
		};
		SceneView* scene_view;

		class TileView : public principle::View
		{
		public:
			TileView(Application* application);
			~TileView() = default;

			void update(float delta) override;
			void draw() override;

			void show(const thoreau::TileCoordinate& tile_coordinate);

		private:
			Application* application;
			std::unique_ptr<principle::Thumbnail> thumbnail;
			thoreau::TileCoordinate tile_coordinate = thoreau::TileCoordinate(0, 0, 0);
			bool is_dirty = false;
			float time = 0.0f;
			glm::vec2 previous_size = glm::vec2(0.0f);
		};
		TileView* tile_view;

		principle::ViewManager view_manager;
		std::map<principle::View*, int> views;
		bool add_view(principle::View* view);
		void add_view(principle::View* view, int index);
		void remove_view(principle::View* view);
		void remove_view(int index);

		std::vector<kvncer::Mesh*> meshes;
		std::vector<kvncer::MeshBuffer*> mesh_buffers;
		kvncer::Mesh* create_mesh_for_model(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);
		kvncer::Mesh* create_mesh_for_terrain(const kvre::Terrain* terrain);
		void clear_meshes();

		void add_model(
			const principle::Draw& draw,
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper = kompvter::VertexIndexMapper());
		void add_terrain(
			const principle::Draw& draw,
			const kvre::Terrain* terrain);

		chomsky::VisibleTileWorldUpdatePass::VisibleTileState visible_tile_state;
		chomsky::VisibleTileWorldUpdatePass::VisibleTileState previous_visible_tile_state;
		chomsky::VisibleTileWorldUpdatePass visible_tile_pass;

		enum
		{
			show_focus = 0,
			show_impassable_tiles = 1,
			show_passable_tiles = 2
		};
		int show_tiles = show_focus;
		std::set<thoreau::TileCoordinate> passable_tiles;
		std::set<thoreau::TileCoordinate> impassable_tiles;
		void save_tiles(const std::string& filename);
		void apply_tile_heuristic();

		principle::BatchedModelManager batched_model_manager;
		continent1::CandidateManager candidate_manager;
	};
}

#endif
