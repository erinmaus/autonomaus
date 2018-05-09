-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Log = require "maus.common.Log"
Native = require "eiznebahn.detail.Native"

KeyMap = {
	['a']: Native.rv_key_a,
	['b']: Native.rv_key_b,
	['c']: Native.rv_key_c,
	['d']: Native.rv_key_d,
	['e']: Native.rv_key_e,
	['f']: Native.rv_key_f,
	['g']: Native.rv_key_g,
	['h']: Native.rv_key_h,
	['i']: Native.rv_key_i,
	['j']: Native.rv_key_j,
	['k']: Native.rv_key_k,
	['l']: Native.rv_key_l,
	['m']: Native.rv_key_m,
	['n']: Native.rv_key_n,
	['o']: Native.rv_key_o,
	['p']: Native.rv_key_p,
	['q']: Native.rv_key_q,
	['r']: Native.rv_key_r,
	['s']: Native.rv_key_s,
	['t']: Native.rv_key_t,
	['u']: Native.rv_key_u,
	['v']: Native.rv_key_v,
	['w']: Native.rv_key_w,
	['x']: Native.rv_key_x,
	['y']: Native.rv_key_y,
	['z']: Native.rv_key_z,

	['0']: Native.rv_key_0,
	['1']: Native.rv_key_1,
	['2']: Native.rv_key_2,
	['3']: Native.rv_key_3,
	['4']: Native.rv_key_4,
	['5']: Native.rv_key_5,
	['6']: Native.rv_key_6,
	['7']: Native.rv_key_7,
	['8']: Native.rv_key_8,
	['9']: Native.rv_key_9,

	['esc']: Native.rv_key_escape,
	['`']: Native.rv_key_tilde,
	['-']: Native.rv_key_minus,
	['=']: Native.rv_key_equals,
	['\b']: Native.rv_key_backspace,
	['\t']: Native.rv_key_tab,
	['[']: Native.rv_key_open_bracket,
	[']']: Native.rv_key_close_bracket,
	['\n']: Native.rv_key_return,
	[';']: Native.rv_key_semicolon,
	['\'']: Native.rv_key_apostrophe,
	['\\']: Native.rv_key_backslash,
	[',']: Native.rv_key_comma,
	['.']: Native.rv_key_full_stop,
	['/']: Native.rv_key_slash,
	[' ']: Native.rv_key_space,
	['insert']: Native.rv_key_insert,
	['delete']: Native.rv_key_delete,
	['home']: Native.rv_key_home,
	['end']: Native.rv_key_end,
	['page_up']: Native.rv_key_page_up,
	['page_down']: Native.rv_key_page_down,
	['left']: Native.rv_key_left,
	['right']: Native.rv_key_right,
	['up']: Native.rv_key_up,
	['down']: Native.rv_key_down
}

KeyModifiers = {
	['alt']: Native.rv_key_alt
	['shift']: Native.rv_key_shift
	['ctrl']: Native.rv_key_ctrl
}

roll = (min, max, rolls) ->
	duration = 0
	for i = 1, rolls
		duration += math.random(min, max)
	return duration

class KeySequence
	new: (token) =>
		@keys = {}
		@token = token

	add_key: (key, modifiers) =>
		if KeyMap[key]
			table.insert(@keys, { key: KeyMap[key], modifiers: modifiers })
		else
			Log\warn('high', "couldn't find key '%s'", key)

	get_delay: =>
		return roll(10, 15, 5)

	replay: (input_state) =>
		modifiers = {
			shift: false
			ctrl: false
			alt: false
		}

		Native.rv_begin_event_sequence(input_state)
		for _, key in ipairs(@keys)
			for modifier, active in pairs(modifiers)
				if active and not key.modifiers[modifier]
					Native.rv_send_key_up(input_state, KeyModifiers[modifier], @\get_delay!)
			for modifier in pairs(key.modifiers)
				if not modifiers[modifier]
					Native.rv_send_key_down(input_state, KeyModifiers[modifier], @\get_delay!)
					modifiers[modifier] = true
			Native.rv_send_key_down(input_state, key.key, @\get_delay!)
			Native.rv_send_key_up(input_state, key.key, @\get_delay!)

		for modifier, active in pairs(modifiers)
			if active
				Native.rv_send_key_up(input_state, KeyModifiers[modifier], @\get_delay!)
		Native.rv_end_event_sequence(input_state)

return KeySequence
