// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_MULTI_TILE_ELEVATION_EXTENTS_HPP
#define CHOMSKY_WORLD_MULTI_TILE_ELEVATION_EXTENTS_HPP

#include <cmath>
#include <iterator>
#include <map>
#include <utility>
#include <boost/container/flat_set.hpp>

namespace chomsky
{
	class MultiTileElevationExtents
	{
	public:
		MultiTileElevationExtents() = default;
		~MultiTileElevationExtents() = default;

		void test(int tag, float y);

		std::size_t count() const;
		std::size_t count(int tag) const;

		bool has(int tag) const;

		float get_min(int tag) const;
		float get_min_above(int tag, float threshold) const;
		float get_max(int tag) const;
		float get_max_below(int tag, float threshold) const;

		bool try_find_tag(float y, int& result) const;

		void remove_tag(int tag);
		void clear();

	private:
		typedef boost::container::flat_set<float> MinMaxY;
		typedef std::map<int, MinMaxY> ExtentCollection;
		ExtentCollection extents;

	public:
		static const int MIN_Y = 0;
		static const int MAX_Y = 1;
		static const int TAG = 2;
		typedef std::tuple<float, float, int> LayerExtents;

		struct const_iterator : public ExtentCollection::const_iterator
		{
			typedef LayerExtents value_type;

			const_iterator(const ExtentCollection::const_iterator& source);

			value_type operator *() const;
		};

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
