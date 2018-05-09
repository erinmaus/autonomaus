// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_KEYBOARD_KEY_SEQUENCE_HPP
#define EIZNEBAHN_KEYBOARD_KEY_SEQUENCE_HPP

#include <set>
#include <tuple>
#include <vector>

namespace eiznebahn
{
	class KeySequence
	{
	public:
		KeySequence() = default;
		~KeySequence() = default;

		enum
		{
			key_code_a = 0,
			key_code_b,
			key_code_c,
			key_code_d,
			key_code_e,
			key_code_f,
			key_code_g,
			key_code_h,
			key_code_i,
			key_code_j,
			key_code_k,
			key_code_l,
			key_code_m,
			key_code_n,
			key_code_o,
			key_code_p,
			key_code_q,
			key_code_r,
			key_code_s,
			key_code_t,
			key_code_u,
			key_code_v,
			key_code_w,
			key_code_x,
			key_code_y,
			key_code_z,

			key_code_0,
			key_code_1,
			key_code_2,
			key_code_3,
			key_code_4,
			key_code_5,
			key_code_6,
			key_code_7,
			key_code_8,
			key_code_9,

			key_code_escape,
			key_code_tilde,
			key_code_minus,
			key_code_equals,
			key_code_backspace,
			key_code_tab,
			key_code_open_bracket,
			key_code_close_bracket,
			key_code_return,
			key_code_semicolon,
			key_code_apostrophe,
			key_code_backslash,
			key_code_comma,
			key_code_full_stop,
			key_code_slash,
			key_code_space,
			key_code_insert,
			key_code_delete,
			key_code_home,
			key_code_end,
			key_code_page_up,
			key_code_page_down,
			key_code_left,
			key_code_right,
			key_code_up,
			key_code_down,

			key_code_f1,
			key_code_f2,
			key_code_f3,
			key_code_f4,
			key_code_f5,
			key_code_f6,
			key_code_f7,
			key_code_f8,
			key_code_f9,
			key_code_f10,
			key_code_f11,
			key_code_f12,

			key_code_left_ctrl,
			key_code_right_ctrl,
			key_code_left_alt,
			key_code_right_alt,
			key_code_left_shift,
			key_code_right_shift,

			key_code_max,
			key_code_min = key_code_a
		};

		enum
		{
			key_press = 1,
			key_release
		};

		static const std::size_t KEY = 0;
		static const std::size_t TYPE = 1;
		static const std::size_t DELAY = 2;
		typedef std::tuple<int, int, float> Key;

		void press(int key, float delay);
		void release(int key, float delay);

		Key get(std::size_t index) const;

		bool empty() const;
		std::size_t count() const;

		// False if there are unpaired press/releases. Examples:
		//
		// sequence.press(key_code_a, 1)
		// sequence.press(key_code_a, 1)
		// sequence.is_safe() == false
		//
		// or...
		//
		// sequence.press(key_code_a, 1)
		// sequence.is_safe() == false
		//
		// or...
		//
		// sequence.release(key_code_a, 1)
		// sequence.is_safe() == false
		//
		// but...
		//
		// sequence.press(key_code_a, 1)
		// sequence.release(key_code_a, 1)
		// sequence.is_safe() == true
		bool get_is_safe() const;

	private:
		typedef std::vector<Key> KeyCollection;
		KeyCollection keys;

		std::set<int> current_keys;
		bool is_safe = true;

	public:
		typedef KeyCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
