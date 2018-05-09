// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_VERTEX_FETCHER_HPP
#define KOMPVTER_GRAPHICS_VERTEX_FETCHER_HPP

#include <cstddef>
#include <cstdint>
#include "kompvter/graphics/buffer.hpp"

namespace kompvter
{
	class Buffer;

	template <typename Primitive>
	struct VertexElement
	{
		Primitive x, y, z, w;
	};
	typedef VertexElement<int> IntegralVertexElement;
	typedef VertexElement<unsigned int> UnsignedVertexElement;
	typedef VertexElement<float> FloatingPointVertexElement;

	class VertexFetcher
	{
	public:
		enum
		{
			format_none,
			format_byte,
			format_unsigned_byte,
			format_short,
			format_unsigned_short,
			format_integer,
			format_unsigned_integer,
			format_float,
			format_half_float
		};

		VertexFetcher() = default;
		VertexFetcher(
			Buffer buffer,
			std::size_t num_components,
			int format, bool normalized,
			std::size_t stride, std::size_t offset);
		~VertexFetcher() = default;

		bool fetch(
			std::size_t index, IntegralVertexElement& element) const;
		bool fetch(
			std::size_t index, UnsignedVertexElement& element) const;
		bool fetch(
			std::size_t index, FloatingPointVertexElement& element) const;

		bool extract(
			std::size_t index, std::size_t count,
			int* output, std::size_t num_components, std::size_t stride) const;
		bool extract(
			std::size_t index, std::size_t count,
			unsigned int* output, std::size_t num_components, std::size_t stride) const;
		bool extract(
			std::size_t index, std::size_t count,
			float* output, std::size_t num_components, std::size_t stride) const;

		std::size_t get_num_vertices() const;

		const Buffer& get_buffer() const;
		int get_num_components() const;
		int get_format() const;
		bool get_normalized() const;
		std::size_t get_stride() const;
		std::size_t get_offset() const;

	private:
		const std::uint8_t* advance(std::size_t index) const;
		static std::size_t get_component_size(int format);

		Buffer buffer;
		const std::uint8_t* data = nullptr;
		std::size_t data_size = 0;

		std::size_t num_components = 0;
		std::size_t element_size = 0;

		int format = format_float;
		bool normalized = false;

		std::size_t stride = 0;
		std::size_t offset = 0;
	};
}

#endif
