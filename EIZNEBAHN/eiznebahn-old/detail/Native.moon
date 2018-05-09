-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2016 [bk]door.maus

ffi = require "ffi"

-- rv_input.h
ffi.cdef [[
	struct rv_window;
	typedef struct rv_input_event_state rv_input_event_state;

	rv_input_event_state* rv_create_input_event_state(struct rv_window* window);
	void rv_destroy_input_event_state(rv_input_event_state* event_state);

	void rv_begin_event_sequence(rv_input_event_state* event_state);
	void rv_end_event_sequence(rv_input_event_state* event_state);

	void rv_send_key_down(rv_input_event_state* window, int key, int delay);
	void rv_send_key_up(rv_input_event_state* window, int key, int delay);

	int rv_get_mouse_x(rv_input_event_state* event_state);
	int rv_get_mouse_y(rv_input_event_state* event_state);
	unsigned int rv_get_mouse_buttons(rv_input_event_state* event_state);

	void rv_send_mouse_move(rv_input_event_state* window, int x, int y, int delay);
	void rv_send_mouse_button_down(rv_input_event_state* window, int button, int delay);
	void rv_send_mouse_button_up(rv_input_event_state* window, int button, int delay);
]]

-- rv_input_keys.h
ffi.cdef [[
	enum
	{
		rv_key_a = 0,
		rv_key_b,
		rv_key_c,
		rv_key_d,
		rv_key_e,
		rv_key_f,
		rv_key_g,
		rv_key_h,
		rv_key_i,
		rv_key_j,
		rv_key_k,
		rv_key_l,
		rv_key_m,
		rv_key_n,
		rv_key_o,
		rv_key_p,
		rv_key_q,
		rv_key_r,
		rv_key_s,
		rv_key_t,
		rv_key_u,
		rv_key_v,
		rv_key_w,
		rv_key_x,
		rv_key_y,
		rv_key_z,

		rv_key_0,
		rv_key_1,
		rv_key_2,
		rv_key_3,
		rv_key_4,
		rv_key_5,
		rv_key_6,
		rv_key_7,
		rv_key_8,
		rv_key_9,

		rv_key_escape,
		rv_key_tilde,
		rv_key_minus,
		rv_key_equals,
		rv_key_backspace,
		rv_key_tab,
		rv_key_open_bracket,
		rv_key_close_bracket,
		rv_key_return,
		rv_key_semicolon,
		rv_key_apostrophe,
		rv_key_backslash,
		rv_key_comma,
		rv_key_full_stop,
		rv_key_slash,
		rv_key_space,
		rv_key_insert,
		rv_key_delete,
		rv_key_home,
		rv_key_end,
		rv_key_page_up,
		rv_key_page_down,
		rv_key_left,
		rv_key_right,
		rv_key_up,
		rv_key_down,

		rv_key_ctrl,
		rv_key_alt,
		rv_key_shift,

		rv_key_max
	};
]]

-- rv_time.h
ffi.cdef [[
	long long rv_get_time();
	void rv_sleep(long long period);
]]

-- rv_window.h
ffi.cdef [[
	struct rv_image;
	typedef struct rv_window rv_window;

	rv_window* rv_select_window();
	rv_window* rv_window_from_handle(size_t handle);
	void rv_destroy_window(rv_window* window);

	int rv_get_window_x(rv_window* window);
	int rv_get_window_y(rv_window* window);
	int rv_get_window_width(rv_window* window);
	int rv_get_window_height(rv_window* window);

	void rv_resize_window(rv_window* window, int width, int height);
]]

return ffi.load(os.getenv("EIZNEBAHN_RV_LIBRARY") or "rv")
