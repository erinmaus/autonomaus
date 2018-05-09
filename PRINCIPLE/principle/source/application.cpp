// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <allegro5/allegro_native_dialog.h>
#include "principle/application.hpp"
#include "principle/common/jaquesFrameReader.hpp"
#include "principle/common/window.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/graphics/guiManager.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"

principle::Application::Application(const std::string& name) :
	trace_window(this),
	select_frame_window(this)
{
	this->name = name;

	initialize_allegro();
	load_window_settings();

	this->timer = al_create_timer(1 / 60.0f);
	if (this->timer == nullptr)
	{
		throw std::runtime_error("couldn't create timer");
	}

	this->event_queue = al_create_event_queue();
	if (this->event_queue == nullptr)
	{
		throw std::runtime_error("couldn't create event queue");
	}

	this->display = std::make_unique<Display>(
		this->event_queue,
		this->new_window_x, this->new_window_y,
		this->new_window_width, this->new_window_height);
	this->gui_manager = std::make_unique<GUIManager>(this->display.get());
	this->game_manager = std::make_unique<GameManager>();

	al_register_event_source(
		this->event_queue, al_get_timer_event_source(this->timer));
	al_register_event_source(this->event_queue, al_get_keyboard_event_source());
	al_register_event_source(this->event_queue, al_get_mouse_event_source());

	trace_window.open();
}

principle::Application::~Application()
{
	al_destroy_timer(this->timer);
	al_destroy_event_queue(this->event_queue);
}

void principle::Application::run()
{
	assert(!this->is_running);

	this->is_running = true;
	al_start_timer(this->timer);

	ALLEGRO_EVENT e;
	bool can_draw = false;
	float last_gui_timestamp = al_get_time();
	while (this->is_running)
	{
		al_wait_for_event(this->event_queue, &e);
		process(e);

		if (e.type == ALLEGRO_EVENT_TIMER && (ALLEGRO_TIMER*)e.any.source == this->timer)
		{
			can_draw = true;
		}

		if (can_draw && al_is_event_queue_empty(this->event_queue))
		{
			draw();

			float current_gui_timestamp = al_get_time();
			this->gui_manager->start(current_gui_timestamp - last_gui_timestamp);
			update_gui();
			this->gui_manager->stop();
			this->gui_manager->render();

			this->display->flip();
			can_draw = false;

			last_gui_timestamp = current_gui_timestamp;
		}
	}

	save_window_settings();
}

void principle::Application::stop()
{
	assert(this->is_running);

	this->is_running = false;
}

void principle::Application::add_window(Window* window)
{
	this->windows.insert(window);
}

void principle::Application::remove_window(Window* window)
{
	this->windows.erase(window);
}

std::string principle::Application::get_name() const
{
	return this->name;
}

void principle::Application::process(ALLEGRO_EVENT& event)
{
	auto& imgui = ImGui::GetIO();
	switch (event.type)
	{
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (!imgui.WantCaptureMouse)
			{
				MouseEvent e;
				e.set_position(glm::vec2(event.mouse.x, event.mouse.y));
				e.set_button(event.mouse.button);

				this->pressed_mouse_buttons.insert(event.mouse.button);

				Application::on_mouse_button_down(e);
				on_mouse_button_down(e);
			}
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (!imgui.WantCaptureMouse && this->pressed_mouse_buttons.count(event.mouse.button) != 0)
			{
				MouseEvent e;
				e.set_position(glm::vec2(event.mouse.x, event.mouse.y));
				e.set_button(event.mouse.button);

				this->pressed_mouse_buttons.erase(event.mouse.button);

				Application::on_mouse_button_up(e);
				on_mouse_button_up(e);
			}
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			if (!imgui.WantCaptureMouse)
			{
				if (event.mouse.dx != 0 || event.mouse.dy != 0)
				{
					MouseEvent e;
					e.set_position(glm::vec2(event.mouse.x, event.mouse.y));
					e.set_difference(glm::vec2(event.mouse.dx, event.mouse.dy));

					Application::on_mouse_move(e);
					on_mouse_move(e);
				}

				if (event.mouse.dz != 0)
				{
					MouseEvent e;
					e.set_wheel(event.mouse.z);

					Application::on_mouse_scroll(e);
					on_mouse_scroll(e);
				}
			}
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			if (!imgui.WantCaptureKeyboard)
			{
				KeyboardEvent e;
				e.set_modifiers(event.keyboard.modifiers);
				e.set_key_code(event.keyboard.keycode);

				this->pressed_keys.insert(event.keyboard.keycode);

				Application::on_key_down(e);
				on_key_down(e);
			}
			break;
		case ALLEGRO_EVENT_KEY_UP:
			if (!imgui.WantCaptureKeyboard && this->pressed_keys.count(event.keyboard.keycode) != 0)
			{
				KeyboardEvent e;
				e.set_modifiers(event.keyboard.modifiers);
				e.set_key_code(event.keyboard.keycode);

				this->pressed_keys.erase(event.keyboard.keycode);

				Application::on_key_up(e);
				on_key_up(e);
			}
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			if (imgui.WantTextInput)
			{
				this->gui_manager->send_text_input(event.keyboard.unichar);
			}
			else
			{
				KeyboardEvent e;
				e.set_character(event.keyboard.unichar);
				e.set_modifiers(event.keyboard.modifiers);
				e.set_key_code(event.keyboard.keycode);

				Application::on_text_input(e);
				on_text_input(e);
			}
			break;
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
			get_display()->acknowledge_resize();
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			stop();
			break;
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == this->timer)
			{
				Application::update(al_get_timer_speed(this->timer));
				update(al_get_timer_speed(this->timer));
			}
			break;
	}
}

void principle::Application::update(float delta)
{
	for (auto window: this->windows)
	{
		window->update(delta);
	}
}

void principle::Application::update_gui()
{
	auto& imgui = ImGui::GetIO();
	bool capture_mouse = imgui.WantCaptureMouse;
	bool capture_keyboard = imgui.WantCaptureKeyboard;

	for (auto window: this->windows)
	{
		window->draw();
	}

	if (!capture_mouse && imgui.WantCaptureMouse)
	{
		ALLEGRO_MOUSE_STATE mouse_state;
		al_get_mouse_state(&mouse_state);

		for (auto button: this->pressed_mouse_buttons)
		{
			MouseEvent event;
			event.set_position(glm::vec2(mouse_state.x, mouse_state.y));
			event.set_button(button);

			Application::on_mouse_button_up(event);
			on_mouse_button_up(event);
		}

		this->pressed_mouse_buttons.clear();
	}

	if (!capture_keyboard && imgui.WantCaptureKeyboard)
	{
		for (auto key: this->pressed_keys)
		{
			KeyboardEvent event;
			event.set_key_code(key);

			Application::on_key_up(event);
			on_key_up(event);
		}

		this->pressed_keys.clear();
	}
}

void principle::Application::draw()
{
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, get_display()->get_width(), get_display()->get_height());

	glClearColor(0.30f, 0.40f, 0.46f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void principle::Application::on_mouse_button_down(const MouseEvent& event)
{
	// Nada.
}

void principle::Application::on_mouse_button_up(const MouseEvent& event)
{
	// Nada.
}

void principle::Application::on_mouse_move(const MouseEvent& event)
{
	// Nada.
}

void principle::Application::on_mouse_scroll(const MouseEvent& event)
{
	// Nada.
}

void principle::Application::on_key_down(const KeyboardEvent& event)
{
	if (event.get_key_code() == ALLEGRO_KEY_F1)
	{
		trace_window.open();
	}
}

void principle::Application::on_key_up(const KeyboardEvent& event)
{
	// Nada.
}

void principle::Application::on_text_input(const KeyboardEvent& event)
{
	// Nada.
}

void principle::Application::on_start_trace()
{
	// Nada.
}

void principle::Application::on_finish_trace()
{
	// Nada.
}

std::string principle::Application::get_input_filename() const
{
	return get_input_filename(get_current_directory());
}

std::string principle::Application::get_input_filename(
	const std::string& path) const
{
	auto dialog = al_create_native_file_dialog(
		path.c_str(), "Open", nullptr, 0);

	if (dialog == nullptr)
	{
		return "";
	}

	std::string result;
	if (al_show_native_file_dialog(al_get_current_display(), dialog) &&
		al_get_native_file_dialog_count(dialog) > 0)
	{
		result = al_get_native_file_dialog_path(dialog, 0);
	}

	al_destroy_native_file_dialog(dialog);

	return result;
}

std::string principle::Application::get_output_filename() const
{
	return get_output_filename(get_current_directory());
}

std::string principle::Application::get_output_filename(
	const std::string& path) const
{
	auto dialog = al_create_native_file_dialog(
		path.c_str(), "Save", nullptr, ALLEGRO_FILECHOOSER_SAVE);

	if (dialog == nullptr)
	{
		return "";
	}

	std::string result;
	if (al_show_native_file_dialog(al_get_current_display(), dialog) &&
		al_get_native_file_dialog_count(dialog) > 0)
	{
		result = al_get_native_file_dialog_path(dialog, 0);
	}

	al_destroy_native_file_dialog(dialog);

	return result;
}

principle::Display* principle::Application::get_display()
{
	return this->display.get();
}

ALLEGRO_EVENT_QUEUE* principle::Application::get_event_queue()
{
	return this->event_queue;
}

principle::GameManager& principle::Application::get_game_manager()
{
	return *this->game_manager.get();
}

principle::GUIManager& principle::Application::get_gui_manager()
{
	return *this->gui_manager.get();
}

std::string principle::Application::get_trace_filename() const
{
	return this->trace_filename;
}

std::size_t principle::Application::get_current_frame() const
{
	return this->current_frame;
}

std::size_t principle::Application::get_num_frames() const
{
	if (this->trace_reader)
	{
		return this->trace_reader->count();
	}

	return 0;
}

bool principle::Application::replay_trace(
	std::size_t frame_num, std::size_t count, std::size_t step)
{
	if (this->trace_reader && count > 0 && frame_num + count - 1 <= get_num_frames())
	{
		LoadingScreen loading_screen(
			this->event_queue, *this->display,
			frame_num + count - 1, count);
		this->loading_screen = &loading_screen;

		on_start_trace();
		this->game_manager->load(
			loading_screen, *this->trace_reader,
			frame_num, count, step);

		if (loading_screen.get_quit_pending())
		{
			stop();
		}
		else
		{
			on_finish_trace();
		}

		this->loading_screen = nullptr;
		this->current_frame = frame_num + count - 1;

		return true;
	}

	return false;
}

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

void principle::Application::initialize_allegro()
{
	if (!al_init())
	{
		throw std::runtime_error("couldn't initialize Allegro");
	}

	if (!al_install_keyboard())
	{
		throw std::runtime_error("couldn't initialize keyboard");
	}

	if (!al_install_mouse())
	{
		throw std::runtime_error("couldn't initialize mouse");
	}

	al_init_image_addon();

	al_init_font_addon();
	al_init_ttf_addon();
}

std::string principle::Application::get_application_data_directory() const
{
	std::string root;
	const char* home = std::getenv("HOME");
	if (home == nullptr)
	{
		home = ".";
	}

	root = home;
	root += "/.hdt/principle";

	return root;
}

void principle::Application::load_window_settings()
{
	std::string path = get_application_data_directory() + "/settings.ini";
	std::string x, y, w, h;

	auto config = al_load_config_file(path.c_str());
	if (config != nullptr)
	{
		auto get_config_value = [config](const std::string& section, const std::string& key)
		{
			const char* c = al_get_config_value(config, section.c_str(), key.c_str());
			if (c == nullptr)
			{
				return "";
			}
			return c;
		};

		x = get_config_value(this->name, "window_x");
		y = get_config_value(this->name, "window_y");
		w = get_config_value(this->name, "window_width");
		h = get_config_value(this->name, "window_height");

		auto t = al_get_config_value(config, "principle", "trace_filename");
		auto f = get_config_value("principle", "frame");
		if (t != nullptr && f != nullptr)
		{
			load_trace(t);

			auto v = std::atoll(f);
			if (this->trace_reader && v >= 0 && v < this->trace_reader->count())
			{
				this->current_frame = v;
			}
		}
	}

	if (x.empty() || y.empty())
	{
		al_get_new_window_position(
			&this->new_window_x,
			&this->new_window_y);
	}
	else
	{
		this->new_window_x = std::atoi(x.c_str());
		this->new_window_y = std::atoi(y.c_str());
	}

	if (w.empty() || h.empty())
	{
		this->new_window_width = Display::DEFAULT_WIDTH;
		this->new_window_height = Display::DEFAULT_HEIGHT;
	}
	else
	{
		this->new_window_width = std::atoi(w.c_str());
		this->new_window_height = std::atoi(h.c_str());
	}

	al_destroy_config(config);
}

void principle::Application::save_window_settings()
{
	std::string root = get_application_data_directory();
	al_make_directory(root.c_str());

	std::string path = root + "/settings.ini";
	auto config = al_load_config_file(path.c_str());
	if (config == nullptr)
	{
		config = al_create_config();
	}

	auto set_config_int = [config](const std::string& section, const std::string& key, int value)
	{
		std::string v = std::to_string(value);
		al_set_config_value(config, section.c_str(), key.c_str(), v.c_str());
	};

	set_config_int(this->name, "window_x", this->display->get_x());
	set_config_int(this->name, "window_y", this->display->get_y());
	set_config_int(this->name, "window_width", this->display->get_width());
	set_config_int(this->name, "window_height", this->display->get_height());

	if (!this->trace_filename.empty())
	{
		al_set_config_value(config, "principle", "trace_filename", this->trace_filename.c_str());
		set_config_int("principle", "frame", this->current_frame);
	}

	al_save_config_file(path.c_str(), config);
	al_destroy_config(config);
}

principle::LoadingScreen* principle::Application::get_loading_screen() const
{
	return this->loading_screen;
}

std::string principle::Application::get_current_directory() const
{
	std::string result;
	auto hdt_path = std::getenv("HDT_LOCAL_ROOT");
	if (hdt_path == nullptr)
	{
		char* current_directory = al_get_current_directory();

		result = current_directory;
		if (result.back() != ALLEGRO_NATIVE_PATH_SEP)
		{
			result += ALLEGRO_NATIVE_PATH_SEP;
		}

		al_free(current_directory);
	}
	else
	{
		result = hdt_path;
		if (result.back() != ALLEGRO_NATIVE_PATH_SEP)
		{
			result += ALLEGRO_NATIVE_PATH_SEP;
		}

		result += "..";
		result += ALLEGRO_NATIVE_PATH_SEP;
		result += "NECRONOMICON";
		result += ALLEGRO_NATIVE_PATH_SEP;
	}

	return result;
}

void principle::Application::load_trace(const std::string& filename)
{
	if (!filename.empty())
	{
		this->trace_reader = std::make_unique<JaquesFrameReader>(filename);

		if (this->trace_reader->count() > 0)
		{
			this->trace_filename = filename;
		}

		this->current_frame = 0;
	}
}

principle::Application::TraceWindow::TraceWindow(Application* application) :
	Window(application)
{
	this->application = application;
}

std::string principle::Application::TraceWindow::get_title() const
{
	return "PRINCIPLE";
}

void principle::Application::TraceWindow::update_gui()
{
	if (ImGui::Button("Open Trace") && this->application->select_frame_window.get_is_closed())
	{
		std::string filename;
		if (!this->application->trace_filename.empty())
		{
			ALLEGRO_PATH* path = al_create_path(this->application->trace_filename.c_str());
			al_set_path_filename(path, nullptr);

			std::string directory = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
			al_destroy_path(path);

			filename = this->application->get_input_filename(directory);
		}
		else
		{
			filename = this->application->get_input_filename();
		}
		this->application->load_trace(filename);
	}
	if (ImGui::IsItemHovered() && this->application->select_frame_window.get_is_open())
	{
		ImGui::SetTooltip("Please close the frame picker tool before opening a trace.");
	}

	ImGui::SameLine();
	if (ImGui::Button("Pick Frame"))
	{
		if (this->application->trace_reader && this->application->trace_reader->count() > 0)
		{
			this->application->select_frame_window.open();
		}
	}

	if (ImGui::IsItemHovered() && !this->application->trace_reader)
	{
		ImGui::SetTooltip("No trace loaded; can't pick frame.");
	}

	if (!this->application->trace_reader)
	{
		ImGui::Text("Trace: (none)");
	}
	else
	{
		static const std::size_t MAX_LENGTH = 24;

		std::string filename;
		if (this->application->trace_filename.size() > MAX_LENGTH)
		{
			auto length = this->application->trace_filename.size();
			auto sub = this->application->trace_filename.substr(length - MAX_LENGTH);
			filename = "..." + sub;
		}
		else
		{
			filename = this->application->trace_filename;
		}

		ImGui::Text("Trace: %s", filename.c_str());

		ImGui::SameLine();
		ImGui::Text("(frames: %lu)", this->application->trace_reader->count());
	}

	ImGui::Text("Frame: %6lu", this->application->current_frame);

	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		this->application->replay_trace(this->application->current_frame);
	}
}

principle::Application::SelectFrameWindow::SelectFrameWindow(
	Application* application) :
		Window(application)
{
	this->application = application;
}

void principle::Application::SelectFrameWindow::open()
{
	if (this->application->trace_reader)
	{
		Window::open();
		this->current_frame = this->application->current_frame;
		this->min_frame = this->current_frame;
		this->max_frame = this->application->trace_reader->count();

		show(this->current_frame);
	}
}

void principle::Application::SelectFrameWindow::update(float delta)
{
	if (this->is_playing)
	{
		if (!this->application->trace_reader || this->application->trace_reader->count() < 1)
		{
			this->is_playing = false;
			return;
		}

		this->current_time += delta;
		bool frame_dirty = false;
		while (this->current_time > (1.0f / fps))
		{
			++this->current_frame;

			if (this->current_frame >= this->max_frame)
			{
				this->current_frame = this->min_frame;
			}

			frame_dirty = true;
			this->current_time -= (1.0f / fps);
		}

		if (frame_dirty)
		{
			show(this->current_frame);
		}
	}
}

std::string principle::Application::SelectFrameWindow::get_title() const
{
	return "Select Frame Preview";
}

void principle::Application::SelectFrameWindow::update_gui()
{
	if (!this->application->trace_reader ||
		this->application->trace_reader->count() < 1)
	{
		close();
		return;
	}

	ImGui::SetWindowSize(ImVec2(480, 320), ImGuiSetCond_FirstUseEver);

	if (this->is_playing)
	{
		if (ImGui::Button("Pause"))
		{
			this->is_playing = false;
			this->current_time = 0.0f;
		}
	}
	else
	{
		if (ImGui::Button("Play"))
		{
			this->is_playing = true;
			this->current_time = 0.0f;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Select"))
	{
		this->application->replay_trace(this->current_frame);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Load current frame.");
	}

	ImGui::SameLine();
	if (ImGui::Button("Select Loop"))
	{
		ImGui::OpenPopup(get_id("confirm_loop").c_str());
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Load frames from min/max loop points.");
	}

	ImGui::PushItemWidth(128);
	{
		ImGui::Text("FPS");
		ImGui::SameLine();
		ImGui::InputInt("##FPS", &this->fps);

		ImGui::SameLine();
		ImGui::Text("Frame");
		ImGui::SameLine();
		int v = this->current_frame;
		bool modified = ImGui::InputInt("##Frame", &v);
		if (v >= 0 && v < this->application->trace_reader->count())
		{
			this->current_frame = v;
			if (modified)
			{
				show(this->current_frame);
			}
		}

		ImGui::SameLine();
		ImGui::Text("Min Frame");
		ImGui::SameLine();
		int min = this->min_frame;
		ImGui::InputInt("##Min Frame", &min);
		if (min >= 0 && min < this->application->trace_reader->count())
		{
			this->min_frame = min;
		}

		ImGui::SameLine();
		ImGui::Text("Max Frame");
		ImGui::SameLine();
		int max = this->max_frame;
		ImGui::InputInt("##Max Frame", &max);
		if (max >= min && max < this->application->trace_reader->count())
		{
			this->max_frame = max;
		}

		ImGui::SameLine();
		ImGui::Text("Step");
		ImGui::SameLine();
		int step = this->step;
		ImGui::InputInt("##Step", &step);
		if (step >= 1)
		{
			this->step = step;
		}
	}
	ImGui::PopItemWidth();

	if (this->current_screenshot)
	{
		auto size = ImGui::GetWindowSize();
		float height_width_ratio = (float)this->current_screenshot->get_height() / (float)this->current_screenshot->get_width();
		float width = size.x - 16;
		float height = height_width_ratio * width;
		ImGui::Image(this->current_screenshot.get(), ImVec2(width, height));
	}

	if (ImGui::BeginPopup(get_id("confirm_loop").c_str()))
	{
		ImGui::Text("Are you sure you wish to replay %lu frames?",
			this->max_frame - this->min_frame + 1);
		ImGui::Text(
			"The process may appear unresponsive"
			" for a second or more per frame.");

		if (ImGui::Button("Yes"))
		{
			this->application->replay_trace(
				this->min_frame, this->max_frame - this->min_frame + 1,
				this->step);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void principle::Application::SelectFrameWindow::show(std::size_t frame)
{
	current_screenshot = std::unique_ptr<kvncer::Texture>(
		this->application->trace_reader->load_screenshot(this->current_frame));
}
