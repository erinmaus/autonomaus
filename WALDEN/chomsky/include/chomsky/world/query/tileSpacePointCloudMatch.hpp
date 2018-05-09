// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_MATERIAL_MATCH_HPP
#define CHOMSKY_WORLD_QUERY_MATERIAL_MATCH_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class TileSpacePointCloudMatch
	{
	public:
		TileSpacePointCloudMatch() = default;
		~TileSpacePointCloudMatch() = default;

		bool has(const thoreau::TileCoordinate& offset) const;
		const TileSpacePointCloud& get(const thoreau::TileCoordinate& offset) const;
		void set(const thoreau::TileCoordinate& offset, const TileSpacePointCloud& value);

		int get_width() const;
		int get_height() const;
		int get_depth() const;

		bool empty() const;

	private:
		typedef boost::container::flat_map<thoreau::TileCoordinate, TileSpacePointCloud> CloudCollection;
		CloudCollection clouds;

		int min_x = 0, max_x = 0;
		int min_y = 0, max_y = 0;
		int min_layer = 0, max_layer = 0;

	public:
		typedef CloudCollection::const_iterator const_iterator;
		typedef CloudCollection::iterator iterator;

		const_iterator begin() const;
		iterator begin();
		const_iterator end() const;
		iterator end();
	};
}

#endif
