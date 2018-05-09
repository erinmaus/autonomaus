// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef PRINCIPLE_APPLICATION_HPP
#define PRINCIPLE_APPLICATION_HPP

#include <string>
#include <unordered_set>
#include <allegro5/allegro.h>
#include "kvncer/graphics/texture.hpp"
#include "principle/common/gameManager.hpp"
#include "principle/common/window.hpp"
#include "principle/graphics/display.hpp"

namespace principle
{
	class KeyboardEvent;
	class GUIManager;
	class MouseEvent;

	class Application
	{
	public:
		Application(const std::string& name);
		~Application();

		void run();
		void stop();

		void add_window(Window* window);
		void remove_window(Window* window);

		std::string get_name() const;

	protected:
		virtual void process(ALLEGRO_EVENT& event);
		virtual void update(float delta);
		virtual void update_gui();
		virtual void draw();

		virtual void on_mouse_button_down(const MouseEvent& event);
		virtual void on_mouse_button_up(const MouseEvent& event);
		virtual void on_mouse_move(const MouseEvent& event);
		virtual void on_mouse_scroll(const MouseEvent& event);
		virtual void on_key_down(const KeyboardEvent& event);
		virtual void on_key_up(const KeyboardEvent& event);
		virtual void on_text_input(const KeyboardEvent& event);
		virtual void on_start_trace();
		virtual void on_finish_trace();

		std::string get_input_filename() const;
		std::string get_input_filename(const std::string& path) const;
		std::string get_output_filename() const;
		std::string get_output_filename(const std::string& path) const;

		Display* get_display();
		ALLEGRO_EVENT_QUEUE* get_event_queue();

		GameManager& get_game_manager();
		GUIManager& get_gui_manager();

		std::string get_trace_filename() const;
		std::size_t get_current_frame() const;
		std::size_t get_num_frames() const;
		bool replay_trace(std::size_t frame_num, std::size_t count = 1, std::size_t step = 1);

		LoadingScreen* get_loading_screen() const;

	private:
		void initialize_allegro();

		std::string get_application_data_directory() const;
		void save_window_settings();
		void load_window_settings();

		std::string get_current_directory() const;

		std::string name;

		int new_window_width, new_window_height;
		int new_window_x, new_window_y;

		bool is_running = false;

		std::unique_ptr<Display> display;
		ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
		ALLEGRO_TIMER* timer = nullptr;

		std::unique_ptr<GUIManager> gui_manager;
		std::unordered_set<Window*> windows;
		std::set<int> pressed_keys;
		std::set<int> pressed_mouse_buttons;

		std::unique_ptr<GameManager> game_manager;

		std::string trace_filename;
		std::unique_ptr<JaquesFrameReader> trace_reader;
		std::size_t current_frame = 0;

		LoadingScreen* loading_screen = nullptr;
		void load_trace(const std::string& filename);

		class TraceWindow : public principle::Window
		{
		public:
			TraceWindow(Application* application);

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;
		};
		TraceWindow trace_window;

		class SelectFrameWindow : public principle::Window
		{
		public:
			SelectFrameWindow(Application* application);

			void open() override;
			void update(float delta) override;

		protected:
			std::string get_title() const override;
			void update_gui() override;

		private:
			Application* application;

			std::unique_ptr<kvncer::Texture> current_screenshot;
			int fps = 30;
			float current_time = 0.0f;
			std::size_t current_frame = 0;
			bool is_playing = false;

			std::size_t min_frame = 0;
			std::size_t max_frame = 0;
			std::size_t step = 1;

			void show(std::size_t frame);
		};
		SelectFrameWindow select_frame_window;
	};
}

#endif
