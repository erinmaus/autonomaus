// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_PROP_PROP_HPP
#define GLOOPER_PROP_PROP_HPP

#include <string>
#include "thoreau/tileCoordinate.hpp"

namespace glooper
{
	class Prop
	{
	public:
		Prop() = default;
		~Prop() = default;

		const std::string& get_name() const;
		void set_name(const std::string& value);

		const thoreau::TileCoordinate& get_tile_coordinate() const;
		void set_tile_coordinate(const thoreau::TileCoordinate& value);

		bool operator <(const Prop& other) const;
		bool operator <=(const Prop& other) const;
		bool operator >(const Prop& other) const;
		bool operator >=(const Prop& other) const;
		bool operator ==(const Prop& other) const;
		bool operator !=(const Prop& other) const;

	private:
		std::string name;
		thoreau::TileCoordinate tile_coordinate;
	};
}

#endif
