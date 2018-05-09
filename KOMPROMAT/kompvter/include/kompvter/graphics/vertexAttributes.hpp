// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_VERTEX_ATTRIBUTES_HPP
#define KOMPVTER_GRAPHICS_VERTEX_ATTRIBUTES_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include "kompvter/graphics/vertexFetcher.hpp"

namespace kompvter
{
	class VertexAttributes
	{
	public:
		typedef std::unordered_map<std::string, VertexFetcher>::const_iterator iterator;

		VertexAttributes() = default;
		virtual ~VertexAttributes() = default;

		void add(const std::string& name, const VertexFetcher& fetcher);
		bool has(const std::string& name) const;
		const VertexFetcher* get(const std::string& name) const;

		iterator begin() const;
		iterator end() const;

	private:
		std::unordered_map<std::string, VertexFetcher> attributes;
	};
}

#endif
