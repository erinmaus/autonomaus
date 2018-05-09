// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cctype>
#include <map>
#include <tuple>
#include "autonomaus/core/rng.hpp"
#include "autonomaus/input/gestures/typeGesture.hpp"
#include "eiznebahn/keyboard/keySequence.hpp"
#include "eiznebahn/keyboard/typingProvider.hpp"

const int autonomaus::TypeGesture::MIN = 17;
const int autonomaus::TypeGesture::MAX = 48;
const char* autonomaus::TypeGesture::ESCAPE = "\x1b";

autonomaus::TypeGesture::TypeGesture(
	InputManager& input_manager,
	const std::string& text,
	bool ignore_case) :
		Gesture(input_manager)
{
	this->text = text;
	this->ignore_case = ignore_case;
}

static std::tuple<int, bool> character_to_key_code(char c)
{
	using namespace eiznebahn;

	static const std::map<char, std::tuple<int, bool>> KEY_CODE_MAP =
	{
		{ 'a', std::make_tuple<int, int>(KeySequence::key_code_a, false) },
		{ 'A', std::make_tuple<int, int>(KeySequence::key_code_a, true) },
		{ 'b', std::make_tuple<int, int>(KeySequence::key_code_b, false) },
		{ 'B', std::make_tuple<int, int>(KeySequence::key_code_b, true) },
		{ 'c', std::make_tuple<int, int>(KeySequence::key_code_c, false) },
		{ 'C', std::make_tuple<int, int>(KeySequence::key_code_c, true) },
		{ 'd', std::make_tuple<int, int>(KeySequence::key_code_d, false) },
		{ 'D', std::make_tuple<int, int>(KeySequence::key_code_d, true) },
		{ 'e', std::make_tuple<int, int>(KeySequence::key_code_e, false) },
		{ 'E', std::make_tuple<int, int>(KeySequence::key_code_e, true) },
		{ 'f', std::make_tuple<int, int>(KeySequence::key_code_f, false) },
		{ 'F', std::make_tuple<int, int>(KeySequence::key_code_f, true) },
		{ 'g', std::make_tuple<int, int>(KeySequence::key_code_g, false) },
		{ 'G', std::make_tuple<int, int>(KeySequence::key_code_g, true) },
		{ 'h', std::make_tuple<int, int>(KeySequence::key_code_h, false) },
		{ 'H', std::make_tuple<int, int>(KeySequence::key_code_h, true) },
		{ 'i', std::make_tuple<int, int>(KeySequence::key_code_i, false) },
		{ 'I', std::make_tuple<int, int>(KeySequence::key_code_i, true) },
		{ 'j', std::make_tuple<int, int>(KeySequence::key_code_j, false) },
		{ 'J', std::make_tuple<int, int>(KeySequence::key_code_j, true) },
		{ 'k', std::make_tuple<int, int>(KeySequence::key_code_k, false) },
		{ 'K', std::make_tuple<int, int>(KeySequence::key_code_k, true) },
		{ 'l', std::make_tuple<int, int>(KeySequence::key_code_l, false) },
		{ 'L', std::make_tuple<int, int>(KeySequence::key_code_l, true) },
		{ 'm', std::make_tuple<int, int>(KeySequence::key_code_m, false) },
		{ 'M', std::make_tuple<int, int>(KeySequence::key_code_m, true) },
		{ 'n', std::make_tuple<int, int>(KeySequence::key_code_n, false) },
		{ 'N', std::make_tuple<int, int>(KeySequence::key_code_n, true) },
		{ 'o', std::make_tuple<int, int>(KeySequence::key_code_o, false) },
		{ 'O', std::make_tuple<int, int>(KeySequence::key_code_o, true) },
		{ 'p', std::make_tuple<int, int>(KeySequence::key_code_p, false) },
		{ 'P', std::make_tuple<int, int>(KeySequence::key_code_p, true) },
		{ 'q', std::make_tuple<int, int>(KeySequence::key_code_q, false) },
		{ 'Q', std::make_tuple<int, int>(KeySequence::key_code_q, true) },
		{ 'r', std::make_tuple<int, int>(KeySequence::key_code_r, false) },
		{ 'R', std::make_tuple<int, int>(KeySequence::key_code_r, true) },
		{ 's', std::make_tuple<int, int>(KeySequence::key_code_s, false) },
		{ 'S', std::make_tuple<int, int>(KeySequence::key_code_s, true) },
		{ 't', std::make_tuple<int, int>(KeySequence::key_code_t, false) },
		{ 'T', std::make_tuple<int, int>(KeySequence::key_code_t, true) },
		{ 'u', std::make_tuple<int, int>(KeySequence::key_code_u, false) },
		{ 'U', std::make_tuple<int, int>(KeySequence::key_code_u, true) },
		{ 'v', std::make_tuple<int, int>(KeySequence::key_code_v, false) },
		{ 'V', std::make_tuple<int, int>(KeySequence::key_code_v, true) },
		{ 'w', std::make_tuple<int, int>(KeySequence::key_code_w, false) },
		{ 'W', std::make_tuple<int, int>(KeySequence::key_code_w, true) },
		{ 'x', std::make_tuple<int, int>(KeySequence::key_code_x, false) },
		{ 'X', std::make_tuple<int, int>(KeySequence::key_code_x, true) },
		{ 'y', std::make_tuple<int, int>(KeySequence::key_code_y, false) },
		{ 'Y', std::make_tuple<int, int>(KeySequence::key_code_y, true) },
		{ 'z', std::make_tuple<int, int>(KeySequence::key_code_z, false) },
		{ 'Z', std::make_tuple<int, int>(KeySequence::key_code_z, true) },
		{ '0', std::make_tuple<int, int>(KeySequence::key_code_0, false) },
		{ '1', std::make_tuple<int, int>(KeySequence::key_code_1, false) },
		{ '2', std::make_tuple<int, int>(KeySequence::key_code_2, false) },
		{ '3', std::make_tuple<int, int>(KeySequence::key_code_3, false) },
		{ '4', std::make_tuple<int, int>(KeySequence::key_code_4, false) },
		{ '5', std::make_tuple<int, int>(KeySequence::key_code_5, false) },
		{ '6', std::make_tuple<int, int>(KeySequence::key_code_6, false) },
		{ '7', std::make_tuple<int, int>(KeySequence::key_code_7, false) },
		{ '8', std::make_tuple<int, int>(KeySequence::key_code_8, false) },
		{ '9', std::make_tuple<int, int>(KeySequence::key_code_9, false) },
		{ ')', std::make_tuple<int, int>(KeySequence::key_code_0, true) },
		{ '!', std::make_tuple<int, int>(KeySequence::key_code_1, true) },
		{ '@', std::make_tuple<int, int>(KeySequence::key_code_2, true) },
		{ '#', std::make_tuple<int, int>(KeySequence::key_code_3, true) },
		{ '$', std::make_tuple<int, int>(KeySequence::key_code_4, true) },
		{ '%', std::make_tuple<int, int>(KeySequence::key_code_5, true) },
		{ '^', std::make_tuple<int, int>(KeySequence::key_code_6, true) },
		{ '&', std::make_tuple<int, int>(KeySequence::key_code_7, true) },
		{ '*', std::make_tuple<int, int>(KeySequence::key_code_8, true) },
		{ '(', std::make_tuple<int, int>(KeySequence::key_code_9, true) },
		{ '`', std::make_tuple<int, int>(KeySequence::key_code_tilde, false) },
		{ '~', std::make_tuple<int, int>(KeySequence::key_code_tilde, true) },
		{ '-', std::make_tuple<int, int>(KeySequence::key_code_minus, false) },
		{ '_', std::make_tuple<int, int>(KeySequence::key_code_minus, true) },
		{ '=', std::make_tuple<int, int>(KeySequence::key_code_equals, false) },
		{ '\b', std::make_tuple<int, int>(KeySequence::key_code_backspace, false) },
		{ '+', std::make_tuple<int, int>(KeySequence::key_code_equals, true) },
		{ '\t', std::make_tuple<int, int>(KeySequence::key_code_tab, false) },
		{ '[', std::make_tuple<int, int>(KeySequence::key_code_open_bracket, false) },
		{ '{', std::make_tuple<int, int>(KeySequence::key_code_open_bracket, true) },
		{ ']', std::make_tuple<int, int>(KeySequence::key_code_close_bracket, false) },
		{ '}', std::make_tuple<int, int>(KeySequence::key_code_close_bracket, true) },
		{ '\n', std::make_tuple<int, int>(KeySequence::key_code_return, false) },
		{ ';', std::make_tuple<int, int>(KeySequence::key_code_semicolon, false) },
		{ ':', std::make_tuple<int, int>(KeySequence::key_code_semicolon, true) },
		{ '\'', std::make_tuple<int, int>(KeySequence::key_code_apostrophe, false) },
		{ '\"', std::make_tuple<int, int>(KeySequence::key_code_apostrophe, true) },
		{ '\\', std::make_tuple<int, int>(KeySequence::key_code_backslash, false) },
		{ '|', std::make_tuple<int, int>(KeySequence::key_code_backslash, true) },
		{ ',', std::make_tuple<int, int>(KeySequence::key_code_comma, false) },
		{ '<', std::make_tuple<int, int>(KeySequence::key_code_comma, true) },
		{ '.', std::make_tuple<int, int>(KeySequence::key_code_full_stop, false) },
		{ '>', std::make_tuple<int, int>(KeySequence::key_code_full_stop, true) },
		{ '/', std::make_tuple<int, int>(KeySequence::key_code_slash, false) },
		{ '?', std::make_tuple<int, int>(KeySequence::key_code_slash, true) },
		{ ' ', std::make_tuple<int, int>(KeySequence::key_code_space, false) }
	};

	auto iter = KEY_CODE_MAP.find(c);
	if (iter == KEY_CODE_MAP.end())
	{
		return std::make_tuple<int, int>(eiznebahn::KeySequence::key_code_max, false);
	}

	return iter->second;
}

std::string autonomaus::TypeGesture::escape_sequence(int modifier, int key)
{
	std::string sequence = ESCAPE;
	sequence += (char)modifier;
	sequence += (char)key;

	return sequence;
}

std::string autonomaus::TypeGesture::escape_sequence(int modifier, char key)
{
	auto press = character_to_key_code(key);

	std::string sequence = ESCAPE;
	sequence += (char)modifier;
	sequence += (char)std::get<0>(press);

	return sequence;
}

std::string autonomaus::TypeGesture::escape_sequence(int key)
{
	std::string sequence = ESCAPE;
	sequence += (char)eiznebahn::KeySequence::key_code_max;
	sequence += (char)key;

	return sequence;
}

int autonomaus::TypeGesture::play()
{
	eiznebahn::KeySequence sequence;
	bool is_shift_down = false;
	bool is_ctrl_down = false;
	bool is_alt_down = false;

	for (std::size_t i = 0; i < text.size(); /* Nop. */)
	{
		bool requires_shift = false;
		bool requires_ctrl = false;
		bool requires_alt = false;
		auto c = text[i];
		int key;
		if (c == ESCAPE[0])
		{
			++i;
			if (i >= text.size())
			{
				return error_unbalanced;
			}

			int modifier = text[i];
			requires_shift =
				modifier == eiznebahn::KeySequence::key_code_left_shift ||
				modifier == eiznebahn::KeySequence::key_code_right_shift;
			requires_ctrl =
				modifier == eiznebahn::KeySequence::key_code_left_ctrl ||
				modifier == eiznebahn::KeySequence::key_code_right_ctrl;
			requires_alt =
				modifier == eiznebahn::KeySequence::key_code_left_alt ||
				modifier == eiznebahn::KeySequence::key_code_right_alt;

			++i;
			if (i >= text.size())
			{
				return error_unbalanced;
			}

			key = text[i];
		}
		else
		{
			if (ignore_case)
			{
				c = std::tolower(c);
			}

			auto press = character_to_key_code(c);
			key = std::get<0>(press);
			requires_shift = std::get<1>(press);
		}
		++i;

		if (requires_shift && !is_shift_down)
		{
			is_shift_down = true;
			sequence.press(eiznebahn::KeySequence::key_code_left_shift, RNG::next(MIN, MAX));
		}
		else if (!requires_shift && is_shift_down)
		{
			is_shift_down = false;
			sequence.release(eiznebahn::KeySequence::key_code_left_shift, RNG::next(MIN, MAX));
		}

		if (requires_ctrl && !is_ctrl_down)
		{
			is_ctrl_down = true;
			sequence.press(eiznebahn::KeySequence::key_code_left_ctrl, RNG::next(MIN, MAX));
		}
		else if (!requires_ctrl && is_ctrl_down)
		{
			is_ctrl_down = false;
			sequence.release(eiznebahn::KeySequence::key_code_left_ctrl, RNG::next(MIN, MAX));
		}

		if (requires_alt && !is_alt_down)
		{
			is_alt_down = true;
			sequence.press(eiznebahn::KeySequence::key_code_left_alt, RNG::next(MIN, MAX));
		}
		else if (!requires_alt && is_alt_down)
		{
			is_alt_down = false;
			sequence.release(eiznebahn::KeySequence::key_code_left_alt, RNG::next(MIN, MAX));
		}

		if (key == eiznebahn::KeySequence::key_code_max)
		{
			continue;
		}

		sequence.press(key, RNG::next(MIN, MAX));
		sequence.release(key, RNG::next(MIN, MAX));
	}

	if (is_shift_down)
	{
		sequence.release(eiznebahn::KeySequence::key_code_left_shift, RNG::next(MIN, MAX));
	}

	if (is_ctrl_down)
	{
		sequence.release(eiznebahn::KeySequence::key_code_left_ctrl, RNG::next(MIN, MAX));
	}

	if (is_alt_down)
	{
		sequence.release(eiznebahn::KeySequence::key_code_left_alt, RNG::next(MIN, MAX));
	}

	if (!sequence.get_is_safe())
	{
		return error_unbalanced;
	}

	auto& queue = get_input_manager().get_queue();
	auto group = queue.start(eiznebahn::InputQueue::channel_keyboard);
	queue.push<eiznebahn::TypingProvider>(sequence);
	queue.stop();

	set_group(group);

	return success;
}
