// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_DETAIL_COORDINATE_HPP
#define THOREAU_DETAIL_COORDINATE_HPP

#include <cstdint>
#include <functional>

namespace thoreau
{
	struct Coordinate
	{
	public:
		typedef std::uint64_t Packed;

		// 28 bits precision.
		const static int MIN_COORDINATE = -134217728;
		const static int MAX_COORDINATE = 134217727;

		// 8 bits precision.
		const static int MIN_LAYER = 0;
		const static int MAX_LAYER = 255;

		Coordinate(int x, int y, int layer = 0);
		Coordinate(Packed other);
		Coordinate() = default;
		~Coordinate() = default;

		Packed to_packed() const;
		static Packed to_packed(int x, int y, int layer = 0);

		void from_packed(Packed value);

		bool match_xy(const Coordinate& other) const;
		bool match_xy(int x, int y) const;
		static bool match_xy(const Coordinate& a, const Coordinate& b);

		bool operator <(const Coordinate& other) const;
		bool operator <=(const Coordinate& other) const;
		bool operator >(const Coordinate& other) const;
		bool operator >=(const Coordinate& other) const;
		bool operator ==(const Coordinate& other) const;
		bool operator !=(const Coordinate& other) const;

		std::int32_t x = 0, y = 0;
		std::int32_t layer = 0;

	private:
		const static std::uint64_t COORDINATE_MASK = 0x0fffffff;
		const static std::uint64_t COORDINATE_BITS = 28;
		const static std::uint64_t LAYER_MASK = 0xff;
		const static std::uint64_t LAYER_BITS = 8;
	};
}

namespace std
{
	template <>
	struct hash<thoreau::Coordinate>
	{
		std::size_t operator()(const thoreau::Coordinate& value) const;
	};
}

inline std::size_t std::hash<thoreau::Coordinate>::operator ()(
	const thoreau::Coordinate& value) const
{
	return value.to_packed();
}

#endif
