// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef POLTERGEIST_APPLICATION_HPP
#define POLTERGEIST_APPLICATION_HPP

#include <memory>
#include <vector>
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/resources/textureHash.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/viewportEvent.hpp"
#include "kompvter/graphics/textureRegion.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "principle/application.hpp"
#include "principle/common/window.hpp"

namespace poltergeist
{
	class Application :
		public principle::Application,
		public kompvter::FrameListener,
		public kompvter::GUIListener
	{
	public:
		Application();
		~Application();

		void on_draw_frame(float timestamp) override;
		void on_draw_enabled(float timestamp) override;
		void on_draw_disabled(float timestamp) override;
		void on_draw_gui(const kompvter::GUIDrawEvent& event) override;
		void on_set_viewport(const kompvter::ViewportEvent& event) override;

	protected:
		void on_mouse_move(const principle::MouseEvent& event) override;
		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_mouse_button_up(const principle::MouseEvent& event) override;
		void on_mouse_scroll(const principle::MouseEvent& event) override;
		void on_start_trace() override;
		void on_finish_trace() override;
		void update_gui() override;
		void draw() override;

	private:
		class PoltergeistWindow : public principle::Window
		{
		public:
			PoltergeistWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
		};
		PoltergeistWindow poltergeist_window;

		class GUIDrawsWindow : public principle::Window
		{
		public:
			GUIDrawsWindow(Application* Application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
			bool show_vertex_info = false;
			bool show_element_info = false;
			bool show_specialized_info = true;
		};
		GUIDrawsWindow gui_draws_window;

		class EditTextureDefinitionWindow : public principle::Window
		{
		public:
			EditTextureDefinitionWindow(Application* application);

			void add(const kompvter::TextureRegion& region);
			void add(int texture_name, int region_name);
			void add(const chomsky::ItemIconSilhouette& silhouette);
			void clear();
			void close() override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			void update_value();
			void add_tag(const std::string& tag);
			void add_data(const std::string& key, const std::string& value);

			Application* application;
			bool minimal_entries_view = true;

			struct Entry
			{
				int texture_name, region_name;
				std::string key;
			};
			std::vector<Entry> entries;

			std::string value;
			std::string pending_input;
		};
		EditTextureDefinitionWindow edit_texture_definition_window;

		class GUIStateWindow : public principle::Window
		{
		public:
			GUIStateWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

			void show_content(const chomsky::GUIContent& content);

		private:
			Application* application;
			std::string query;
			std::vector<chomsky::GUIContent> query_results;
		};
		GUIStateWindow gui_state_window;

		class DiffWindow : public principle::Window
		{
		public:
			DiffWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
		};
		DiffWindow diff_window;

		class ItemIconsWindow : public principle::Window
		{
		public:
			ItemIconsWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
		};
		ItemIconsWindow item_icons_window;

		class ItemIcon3DViewWindow : public principle::Window
		{
		public:
			ItemIcon3DViewWindow(Application* application);

			void open(const principle::ItemIcon& icon);
			using principle::Window::open;
			void close() override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			glm::vec4 background_color = glm::vec4(0.30f, 0.40f, 0.46f, 1.0f);

			float x_rotation = M_PI * 2 - M_PI / 8.0f;
			float y_rotation = M_PI + M_PI / 8.0f;
			float scale = 1.0f;

			glm::vec3 light_position = glm::vec3(-2048, 512, -2048);
			glm::vec3 light_color = glm::vec3(1.0f);
			float light_intensity = 1.0f;
			float light_ambience = 1.0f;

			std::unique_ptr<principle::Thumbnail> thumbnail;
			kvncer::Texture* update_thumbnail();
			void show_thumbnail();

			Application* application;
			const principle::ItemIcon* icon = nullptr;
		};
		ItemIcon3DViewWindow item_icon_3d_view_window;

		int current_framebuffer = 0;

		struct GUIDraw
		{
			int texture;
			kvncer::Mesh mesh;
			kvncer::MeshBuffer vertex_buffer;
			kvncer::MeshBuffer index_buffer;
		};
		typedef std::unique_ptr<GUIDraw> GUIDrawPointer;
		std::multimap<int, GUIDrawPointer> gui_draws;
		bool is_draw_enabled = false;

		std::map<int, chomsky::GUIElementStream> element_streams;
		chomsky::GUIElementStream& get_element_stream(int framebuffer);

		bool select_next = false;
		std::size_t previous_focus = 0;
		std::size_t current_focus = 0;
		bool try_get_element(std::size_t start, int x, int y, std::size_t& result);

		bool highlight_active = false;
		kvncer::Shader* highlight_shader;
		kvncer::Mesh highlight_mesh;
		kvncer::MeshBuffer highlight_vertex_buffer;
		void set_highlight(std::size_t element_index);
		void set_highlight(int left, int right, int top, int bottom);

		kvncer::Shader* gui_shader;
		float viewport_width, viewport_height;
		glm::vec2 camera_offset = glm::vec2(0.0f);
		float camera_scale = 1.0f;
		bool is_camera_dragging = false;
		float previous_scroll = 0.0f;
		glm::mat4 get_projection_matrix();
		glm::mat4 get_view_matrix();

		std::size_t current_frame = 0;

		chomsky::GUIContent gui_state;
		bool gui_state_success = true;
		std::string gui_state_error;
		bool process_gui_state();

		std::size_t element_diff_range_begin = 0;
		std::size_t element_diff_range_end = 0;
		std::vector<chomsky::GUIElement> diff_previous_elements;
		std::vector<chomsky::GUIElement> diff_current_elements;
		void update_diff();

		struct ElementDiff
		{
			ElementDiff(
				const chomsky::GUIElement& element,
				std::size_t frame,
				const kompvter::TextureRegion* region);

			chomsky::GUIElement element;
			std::size_t frame;
			std::unique_ptr<kvncer::Texture> texture;
			kompvter::TextureRegion region;
		};
		std::vector<ElementDiff> element_diffs;
		std::set<std::pair<int, int>> element_diffs_cache;
		void add_diff(const chomsky::GUIElement& element);
		void clear_diffs();
	};
}

#endif
