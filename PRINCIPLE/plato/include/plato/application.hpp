// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_APPLICATION_HPP
#define PLATO_APPLICATION_HPP

#include <map>
#include <memory>
#include <set>
#include <vector>
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/visibleTileWorldUpdatePass.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvncer/graphics/shader.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/game.hpp"
#include "plato/tools/previewTool.hpp"
#include "plato/tools/toolbox.hpp"
#include "plato/tools/toolRenderer.hpp"
#include "plato/tools/toolFactory.hpp"
#include "plato/gui/brochureWidgetManager.hpp"
#include "principle/application.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/scene.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"
#include "twoflower/brochure.hpp"

namespace plato
{
	class Importer;

	class Application : public principle::Application
	{
	public:
		Application();
		~Application();

	protected:
		void update(float delta) override;
		void draw() override;
		void update_gui() override;

		void on_mouse_move(const principle::MouseEvent& event) override;
		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_mouse_button_up(const principle::MouseEvent& event) override;
		void on_key_down(const principle::KeyboardEvent& event) override;
		void on_key_up(const principle::KeyboardEvent& event) override;
		void on_start_trace() override;
		void on_finish_trace() override;

	private:
		class PlatoWindow : public principle::Window
		{
		public:
			PlatoWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			void import_map();
			void load_database(bool open);
		};
		PlatoWindow main_window;

		class ImportDATWindow : public principle::Window
		{
		public:
			ImportDATWindow(Application* application);
			~ImportDATWindow() = default;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			std::string action_type;
			std::string action_name;
			std::string action_skill;

			std::vector<std::string> results;

			void import(const std::string& filename);
		};
		ImportDATWindow import_dat_window;

		class ResourcesWindow : public principle::Window
		{
		public:
			ResourcesWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			void show_locations();
			void show_skills();
			void show_items();
			void show_currencies();
			void show_shops();
			void show_abilities();
			void show_quests();
			void show_locations(const mapp::Location& parent);
			void show_resources(
				const std::string& type,
				const std::function<void()>& add,
				const std::function<void()>& remove);
			void import(const std::string& type, Importer& importer);

			std::size_t previous_tab = 1;
			std::size_t current_tab = 0;
			std::vector<std::pair<std::string, std::function<void()>>> tabs;

			std::string filter;
			int limit = 100;
			twoflower::Resource pending_resource;
			std::string pending_name;

			std::vector<twoflower::Resource> current_resources;
			void populate_resources(const std::string& type);

			mapp::Locations current_locations;
			void populate_locations();
		};
		ResourcesWindow resources_window;

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

		class FindLocationWindow : public principle::Window
		{
		public:
			FindLocationWindow(Application* application);

			void open(const std::function<void(mapp::Location)>& callback);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			std::function<void(mapp::Location)> callback;
			std::string address;
		};
		FindLocationWindow find_location_window;

		class FindResourceWindow : public principle::Window
		{
		public:
			FindResourceWindow(Application* application);

			void open(
				const std::string& tile, 
				const std::function<void(twoflower::Resource)>& callback);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			std::string type;
			std::function<void(twoflower::Resource)> callback;
			std::string filter;
		};
		FindResourceWindow find_resource_window;

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

		class EditResourceWindow : public principle::Window
		{
		public:
			EditResourceWindow(Application* application);
			EditResourceWindow(
				const twoflower::Resource& resource,
				Application* application);
			~EditResourceWindow() = default;

			void add(const twoflower::Resource& resource);

			void close() override;
			void draw() override;

		protected:
			int get_flags() const override;
			std::string get_title() const override;
			std::string get_id() const override;
			void update_gui() override;

		private:
			Application* application;

			int id;
			std::vector<twoflower::Resource> resources;
		};

		class ViewResourceWindow : public principle::Window
		{
		public:
			ViewResourceWindow(Application* application);
			ViewResourceWindow(
				const twoflower::Resource& resource,
				Application* application);
			~ViewResourceWindow() = default;

			void add(const twoflower::Resource& resource);

			void close() override;
			void draw() override;

		protected:
			int get_flags() const override;
			std::string get_title() const override;
			std::string get_id() const override;
			void update_gui() override;

		private:
			Application* application;

			int id;
			std::vector<twoflower::Resource> resources;
		};

		class EditActionWindow : public principle::Window
		{
		public:
			EditActionWindow(Application* application);
			EditActionWindow(
				const twoflower::Action& action,
				Application* application);
			~EditActionWindow() = default;

			void add(const twoflower::Action& action);

			void close() override;
			void draw() override;

		protected:
			int get_flags() const override;
			std::string get_title() const override;
			std::string get_id() const override;
			void update_gui() override;

		private:
			Application* application;

			int id;
			std::vector<twoflower::Action> actions;
		};

		class ViewActionWindow : public principle::Window
		{
		public:
			ViewActionWindow(Application* application);
			ViewActionWindow(
				const twoflower::Action& action,
				Application* application);
			~ViewActionWindow() = default;

			void add(const twoflower::Action& action);

			void close() override;
			void draw() override;

		protected:
			int get_flags() const override;
			std::string get_title() const override;
			std::string get_id() const override;
			void update_gui() override;

		private:
			Application* application;

			int id;
			std::vector<twoflower::Action> actions;
		};

		class ApplicationToolRenderer : public ToolRenderer
		{
		public:
			ApplicationToolRenderer(Application* application);
			~ApplicationToolRenderer() = default;

			glm::vec2 get_camera_offset() const override;
			float get_camera_scale() const override;
			int get_viewport_width() const override;
			int get_viewport_height() const override;
			glm::mat4 get_view_matrix() const override;
			glm::mat4 get_projection_matrix() const override;

		protected:
			void render(ImDrawList& draw_list) override;

		private:
			Application* application;
		};
		ApplicationToolRenderer tool_renderer;

		class ApplicationToolFactory : public ToolFactory
		{
		public:
			ApplicationToolFactory(Application* application);

			void add_new_location() override;
			void add_new_child_location() override;
			void add_new_child_location(const mapp::Location& parent) override;
			void add_new_waypoint(
				const mapp::Location& parent,
				const thoreau::TileCoordinate& tile_coordinate) override;
			void add_new_link(const mapp::Location& from) override;
			void find_location(const std::function<void(mapp::Location)>& callback) override;
			void find_resource(
				const std::string& name,
				const std::function<void(twoflower::Resource)>& callback) override;

			void view(const twoflower::Resource& resource) override;
			void view(const twoflower::Action& action) override;
			void edit(const twoflower::Resource& resource) override;
			void edit(const twoflower::Action& action) override;

		private:
			Application* application;
		};
		ApplicationToolFactory tool_factory;

		void remove_sector(const thoreau::SectorCoordinate& sector_coordinate);

		int current_window_id = 0;
		std::set<principle::Window*> windows;
		std::set<principle::Window*> pending_windows;
		void add_window(principle::Window* window);
		void remove_window(principle::Window* window);

		int current_layer = 0;
		void change_layer(int layer);

		int current_mouse_x = 0, current_mouse_y = 0;
		thoreau::WorldCoordinate mouse_world_position;
		thoreau::WorldCoordinate current_mouse_tile_position;
		thoreau::WorldCoordinate previous_mouse_tile_position;
		glm::vec2 camera_offset = glm::vec2(0.0f);
		float camera_scale = 16.0f;
		bool is_camera_dragging = false;
		bool is_camera_zooming = false;
		int is_multiselection = 0;
		bool show_current_tile_info = 0;
		glm::mat4 get_projection_matrix();
		glm::mat4 get_view_matrix();

		typedef std::unique_ptr<kvncer::Texture> TexturePointer;

		void load_tiles();

		std::map<thoreau::SectorCoordinate, TexturePointer> sector_textures;
		void load_tile(const thoreau::TileCoordinate& tile_coordinate);
		void clear_tiles();

		std::map<thoreau::SectorCoordinate, TexturePointer> sector_elevation_textures;
		std::map<thoreau::SectorCoordinate, TexturePointer> sector_wall_textures;
		std::set<thoreau::SectorCoordinate> dirty_sectors;
		std::set<thoreau::TileCoordinate> modified_tiles;
		void notify_map_update();
		void update_sector_elevations_texture(
			const thoreau::SectorCoordinate& sector_coordinate);
		void update_sector_walls_texture(
			const thoreau::SectorCoordinate& sector_coordinate);

		std::set<thoreau::SectorCoordinate> visible_sectors;
		chomsky::VisibleTileWorldUpdatePass::VisibleTileState visible_tile_state;
		chomsky::VisibleTileWorldUpdatePass visible_tile_pass;

		kvlc::Bounds bounds;

		kvncer::Mesh quad;
		kvncer::MeshBuffer quad_vertex_buffer;
		kvncer::Shader* quad_shader;
		void draw_sector(
			const thoreau::SectorCoordinate& sector_coordinate,
			const glm::mat4& view,
			const glm::mat4& projection);

		bool draw_walls = true;
		bool draw_elevations = false;
		bool draw_atlas = true;
		void draw_sector_elevations(
			const thoreau::SectorCoordinate& sector_coordinate,
			const glm::mat4& view,
			const glm::mat4& projection);
		void draw_sector_walls(const thoreau::SectorCoordinate& sector_coordinate,
			const glm::mat4& view,
			const glm::mat4& projection);

		std::unique_ptr<twoflower::Brochure> brochure;
		std::unique_ptr<mapp::Atlas> atlas;
		std::unique_ptr<mapp::Game> game;
		void load_brochure(const std::string& filename);
		void load_map(const std::string& filename);

		std::unique_ptr<Toolbox> toolbox;
		std::unique_ptr<PreviewTool> preview_tool;
		std::unique_ptr<BrochureWidgetManager> brochure_widget_manager;

		std::vector<std::pair<std::string, std::function<void()>>> menu;
		bool menu_open = false;
		void add_menu_option(const std::string& label, const std::function<void()>& callback);
		void clear_menu();
	};
}

#endif
