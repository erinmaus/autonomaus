// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TILE_SPACE_POINT_CLOUD_HPP
#define CHOMSKY_RESOURCES_TILE_SPACE_POINT_CLOUD_HPP

#include <cstdint>
#include <iostream>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <glm/glm.hpp>
#include "chomsky/io/resources/tileSpacePointCloud.hpp"

namespace chomsky
{
	class TileSpacePointCloud
	{
	public:
		friend class mantorok::DATSerializer<TileSpacePointCloud>;
		friend class mantorok::DATDeserializer<TileSpacePointCloud>;

		static const glm::uvec3 GRID_SIZE;
		static const glm::uvec3 COLOR_SIZE;

		struct const_iterator;

		typedef std::uint32_t Tag;
		static const Tag TAG_NONE = 0;

		TileSpacePointCloud() = default;
		~TileSpacePointCloud() = default;

		bool add(
			const glm::vec3& position, const glm::vec3& color,
			Tag tag = TAG_NONE);
		void remove_all(Tag tag);

		bool has(const glm::vec3& position, const glm::vec3& color) const;

		bool contains(const TileSpacePointCloud& other) const;
		boost::container::flat_set<Tag> match(const TileSpacePointCloud& other) const;

		void merge(const TileSpacePointCloud& other);

		std::size_t count() const;
		std::size_t count(const TileSpacePointCloud& other) const;

		const_iterator begin(Tag tag = TAG_NONE) const;
		const_iterator end() const;

	private:
		static const glm::uvec3 GRID_INPUT_COMPONENT_BITS;
		static const glm::uvec3 COLOR_INPUT_COMPONENT_BITS;
		static const glm::uvec3 GRID_COMPONENT_BITS;
		static const glm::uvec3 GRID_COMPONENT_MASKS;
		static const glm::uvec3 COLOR_COMPONENT_BITS;
		static const glm::uvec3 COLOR_COMPONENT_MASKS;
		static const std::uint32_t POSITION_BITS;
		static const std::uint32_t COLOR_BITS;

		typedef std::uint32_t PackedPoint;
		static bool try_pack(
			const glm::vec3& position, const glm::vec3& color,
			PackedPoint& result);
		static void unpack(
			PackedPoint point,
			glm::vec3& result_position, glm::vec3& result_color);

		typedef boost::container::flat_set<PackedPoint> PointCollection;
		PointCollection points;

		typedef boost::container::flat_map<PackedPoint, boost::container::flat_set<Tag>> TagCollection;
		TagCollection tags;

	public:
		struct Point
		{
			glm::vec3 position;
			glm::vec3 color;
		};

		struct const_iterator : std::iterator<std::forward_iterator_tag, Point>
		{
		public:
			friend TileSpacePointCloud;

			const_iterator() = default;
			~const_iterator() = default;

			const_iterator& operator ++();
			const_iterator operator ++(int);

			bool operator ==(const const_iterator& other) const;
			bool operator !=(const const_iterator& other) const;

			value_type operator *() const;

		private:
			const_iterator(
				const PointCollection::const_iterator& begin,
				const PointCollection::const_iterator& end,
				const TileSpacePointCloud* point_cloud,
				Tag tag);

			PointCollection::const_iterator current;
			PointCollection::const_iterator end;
			const TileSpacePointCloud* point_cloud = nullptr;
			Tag tag = TAG_NONE;
		};
	};
}

#endif
