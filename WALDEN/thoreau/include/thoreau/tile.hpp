// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_TILE_HPP
#define THOREAU_TILE_HPP

#include "thoreau/tileCoordinate.hpp"

namespace thoreau
{
	class Tile
	{
	public:
		enum
		{
			flag_none = 0,

			// 1 .. 10 bits (passable bits)
			flag_impassable = 1 << 0,
			flag_passable = 1 << 1,
			flag_water = 1 << 2,

			// 11 bits .. 20 bits (walls/doors)
			flag_wall_left = 1 << 10,
			flag_wall_right = 1 << 11,
			flag_wall_top = 1 << 12,
			flag_wall_bottom = 1 << 13,
			flag_wall_diagonal = 1 << 14,
			flag_wall = flag_wall_left | flag_wall_right | flag_wall_top | flag_wall_bottom | flag_wall_diagonal,

			flag_door_left = 1 << 15,
			flag_door_right = 1 << 16,
			flag_door_top = 1 << 17,
			flag_door_bottom = 1 << 18,
			flag_door = flag_door_left | flag_door_right | flag_door_top | flag_door_bottom,

			// 21 bits .. 30 bits (misc)
			flag_portal = 1 << 20,
			flag_wall_diagonal_left = 1 << 21, // i.e., '/'
			flag_wall_diagonal_right = 1 << 22, // i.e., '\'
			flag_portal_up = 1 << 23,
			flag_portal_down = 1 << 24,

			// seen bit
			flag_seen = 1 << 31
		};

		Tile() = default;
		~Tile() = default;

		float get_elevation() const;
		float get_min_elevation() const;
		float get_max_elevation() const;
		void set_elevation(float min_value, float max_value);
		bool has_elevation() const;

		float get_height() const;
		void set_height(float value);

		int get_flags() const;
		void set_flags(int value);
		void append_flags(int value);
		void unset_flags(int value);

		void mark_seen();

		bool is_wall() const;
		bool is_impassable() const;
		bool is_passable() const;
		bool is_door() const;
		bool is_portal() const;
		bool was_seen() const;

		bool operator <(const Tile& other) const;
		bool operator <=(const Tile& other) const;
		bool operator >(const Tile& other) const;
		bool operator >=(const Tile& other) const;
		bool operator ==(const Tile& other) const;
		bool operator !=(const Tile& other) const;

	private:
		TileCoordinate location = TileCoordinate(0, 0, 0);
		float min_elevation = -HUGE_VALF, max_elevation = HUGE_VALF;
		int flags = flag_none;

		float height = 0.0f;
	};
}

#endif
