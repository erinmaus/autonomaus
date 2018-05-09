// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_SILHOUETTE_HPP
#define CHOMSKY_RESOURCES_SILHOUETTE_HPP

#include <iostream>
#include <functional>
#include <memory>
#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class Silhouette
	{
	public:
		friend class mantorok::DATSerializer<Silhouette>;
		friend class mantorok::DATDeserializer<Silhouette>;

		typedef float Fragment;

		Silhouette() = default;
		Silhouette(int width, int height, int depth);
		Silhouette(const Silhouette& other);
		Silhouette(Silhouette&& other);
		~Silhouette() = default;

		int get_width() const;
		int get_height() const;
		int get_depth() const;

		Fragment get_fragment(int x, int y, int z) const;
		void set_fragment(int x, int y, int z, Fragment value);

		void merge(const Silhouette& other);
		void clear(Fragment value = 0.0f);

		typedef std::function<bool(Fragment a, Fragment b, int x, int y, int z)> CompareFunction;

		static bool match(
			const Silhouette& a, int a_x, int a_y, int a_z,
			const Silhouette& b, int b_x, int b_y, int b_z,
			int width, int height, int depth,
			const CompareFunction& compare);
		static bool match(
			const Silhouette& a, const Silhouette& b,
			const CompareFunction& compare);

		Silhouette& operator =(const Silhouette& other);
		Silhouette& operator =(Silhouette&& other);

	private:
		int width = 0, height = 0, depth = 0;
		std::unique_ptr<Fragment[]> fragments;
	};
}

#endif
