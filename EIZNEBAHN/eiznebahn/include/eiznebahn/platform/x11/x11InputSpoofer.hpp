// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_PLATFORM_X11_INPUT_SPOOFER_HPP
#define EIZNEBAHN_PLATFORM_X11_INPUT_SPOOFER_HPP
#ifdef EIZNEBAHN_BUILD_X11

#include <functional>
#include <memory>
#include <mutex>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include "eiznebahn/input/inputSpoofer.hpp"
#include "eiznebahn/input/inputState.hpp"
#include "eiznebahn/keyboard/keySequence.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"

namespace eiznebahn
{
	class X11InputSpoofer : public InputSpoofer
	{
	public:
		X11InputSpoofer();
		~X11InputSpoofer();

		void set_target_window(Window window);

		void mouse_move(int x, int y) override;
		void mouse_scroll_down() override;
		void mouse_scroll_up() override;
		void mouse_button_press(int button) override;
		void mouse_button_release(int button) override;

		void key_press(int key_code) override;
		void key_release(int key_code) override;

		void poll() override;

		const InputState& get_input_state() const override;
		const InputState& get_real_input_state() const;

		void disable_real_events();
		void enable_real_events();
		bool get_real_events_enabled() const;

		int get_num_pending_events() const;
		void acknowledge_pending_event();

		enum
		{
			event_mouse_move = 0,
			event_mouse_scroll_up,
			event_mouse_scroll_down,
			event_mouse_button_press,
			event_mouse_button_release
		};
		typedef std::function<bool(int type, int x, int y)> MouseEventCallback;
		void register_mouse_callback(const MouseEventCallback& callback);

		enum
		{
			event_key_press = 0,
			event_key_release
		};
		typedef std::function<bool(int type, int key_code)> KeyboardEventCallback;
		void register_keyboard_callback(const KeyboardEventCallback& callback);

		void make_current();
		static X11InputSpoofer* get_current();

		bool process_event(const XEvent& event);

	private:
		bool window_to_root(
			int input_x, int input_y,
			int& output_x, int& output_y);
		bool root_to_window(
			int input_x, int input_y,
			int& output_x, int& output_y);
		void get_current_mask(int& result_mask);
		Window get_root_window();

		mutable std::mutex mutex;

		int current_mouse_x, current_mouse_y;
		int current_button_mask = 0;
		int current_key_mask = 0;

		Atom time_prop_atom;
		Time get_current_time();
		static Bool filter_timestamp_event(
			Display *display, XEvent* event, XPointer argument);

		Display* display = nullptr;
		Window target_window = None;
		Window dummy_window = None;

		bool real_events_enabled = true;
		int num_pending_events = 0;

		std::unique_ptr<MouseEventCallback> mouse_callback;
		std::unique_ptr<KeyboardEventCallback> keyboard_callback;

		InputState input_state;
		InputState real_input_state;

		static X11InputSpoofer* current;
		static const int BUTTON_MASK;
		static const int KEY_MASK;
		static const int KEY_MAP[KeySequence::key_code_max];
		static const int KEY_MAP_MASK[KeySequence::key_code_max];
		static const int MOUSE_MAP[MouseButton::button_max];
		static const int MOUSE_MAP_MASK[MouseButton::button_max];
		bool try_marshal_key_code_from_x11(int x11_key_code, int& key_code) const;
		bool try_marshal_mouse_button_from_x11(int x11_mouse_button, int& mouse_button) const;
	};
}

#endif
#endif
