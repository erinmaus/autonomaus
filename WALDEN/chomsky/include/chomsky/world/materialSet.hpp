// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_MATERIAL_SET_HPP
#define CHOMSKY_WORLD_MATERIAL_SET_HPP

#include <set>
#include <glm/glm.hpp>
#include <boost/container/flat_map.hpp>

namespace chomsky
{
	class MaterialSet
	{
	public:
		MaterialSet() = default;
		~MaterialSet() = default;

		void add(
			int tag,
			int texture_name, int texture_region,
			const glm::vec3& color);
		bool has(int tag) const;
		void remove(int tag);

		std::size_t count() const;
		std::size_t count(int tag) const;
		bool empty() const;

	private:
		struct MaterialState
		{
			int texture_name, texture_region;
			float weight;
			glm::vec3 color_sum;
		};

		typedef boost::container::flat_multimap<int, MaterialState> MaterialDictionary;
		MaterialDictionary materials;

	public:
		static const std::size_t TAG = 0;
		static const std::size_t TEXTURE = 1;
		static const std::size_t REGION = 2;
		static const std::size_t COLOR = 3;
		static const std::size_t WEIGHT = 4;
		typedef std::tuple<
			int /* tag */,
			int /* texture */,
			int /* region */,
			glm::vec3 /* color */,
			float /* weight */>
		Material;

		struct const_iterator;
		const_iterator begin() const;
		const_iterator begin(int tag) const;
		const_iterator end() const;
		const_iterator end(int tag) const;

		struct const_iterator : public MaterialDictionary::const_iterator
		{
			typedef Material value_type;

			const_iterator(const MaterialDictionary::const_iterator& source);

			value_type operator *() const;
		};
	};
}

#endif
