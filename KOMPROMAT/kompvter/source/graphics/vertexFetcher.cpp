// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <algorithm>
#include <limits>
#include "kompvter/graphics/buffer.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"

kompvter::VertexFetcher::VertexFetcher(
	Buffer buffer,
	std::size_t num_components,
	int format, bool normalized,
	std::size_t stride, std::size_t offset)
{
	this->buffer = buffer;
	this->data = buffer.get_data();
	this->data_size = buffer.get_data_size();

	this->num_components = num_components;
	this->element_size = num_components * get_component_size(format);

	this->format = format;
	this->normalized = normalized;

	if (stride == 0)
	{
		this->stride = this->element_size;
	}
	else
	{
		this->stride = stride;
	}
	this->offset = offset;
}

template <typename Component>
Component marshal_component(const std::uint8_t* data)
{
#if 0
	union {
		std::uint8_t d[sizeof(Component)];
		Component v;
	} result;

	for (std::size_t i = 0; i < sizeof(Component); ++i)
	{
		result.d[i] = data[i];
	}

	return result.v;
#endif
	return *(const Component*)data;
}

float marshal_half_component(const std::uint8_t* data)
{
	union {
		std::uint8_t d[sizeof(std::uint16_t)];
		std::uint16_t v;
	} input;

	input.d[0] = data[0];
	input.d[1] = data[1];

	// Credits: https://gist.github.com/martinkallman/5049614
	std::uint32_t t1 = input.v & 0x7fff;
	std::uint32_t t2 = input.v & 0x8000;
	std::uint32_t t3 = input.v & 0x7c00;

	t1 <<= 13;
	t2 <<= 16;

	t1 += 0x38000000;
	t1 = (t3 == 0 ? 0 : t1);
	t1 |= t2;

	union {
		std::uint32_t integer;
		float single;
	} result;
	result.integer = t1;

	return result.single;
}

template <typename VertexElementComponent>
void fetch_half_component(VertexElementComponent& e, const std::uint8_t* data)
{
	e = (VertexElementComponent)marshal_half_component(data);
}

template <typename VertexElementComponent, typename DataComponent>
void fetch_component(VertexElementComponent& e, const std::uint8_t* data)
{
	e = (VertexElementComponent)marshal_component<DataComponent>(data);
}

template <typename VertexElementComponent>
void fetch_element(
	std::size_t num_components,
	int format,
	const std::uint8_t* data,
	VertexElementComponent* component)
{
	for (std::size_t i = 0; i < num_components; ++i)
	{
		switch(format)
		{
			case kompvter::VertexFetcher::format_byte:
				fetch_component<VertexElementComponent, std::int8_t>(
					component[i], data);
				data += sizeof(std::int8_t);
				break;
			case kompvter::VertexFetcher::format_unsigned_byte:
				fetch_component<VertexElementComponent, std::uint8_t>(
					component[i], data);
				data += sizeof(std::uint8_t);
				break;
			case kompvter::VertexFetcher::format_short:
				fetch_component<VertexElementComponent, std::int16_t>(
					component[i], data);
				data += sizeof(std::int16_t);
				break;
			case kompvter::VertexFetcher::format_unsigned_short:
				fetch_component<VertexElementComponent, std::uint16_t>(
					component[i], data);
				data += sizeof(std::uint16_t);
				break;
			case kompvter::VertexFetcher::format_integer:
				fetch_component<VertexElementComponent, std::int32_t>(
					component[i], data);
				data += sizeof(std::int32_t);
				break;
			case kompvter::VertexFetcher::format_unsigned_integer:
				fetch_component<VertexElementComponent, std::uint32_t>(
					component[i], data);
				data += sizeof(std::uint32_t);
				break;
			case kompvter::VertexFetcher::format_float:
				fetch_component<VertexElementComponent, float>(
					component[i], data);
				data += sizeof(float);
				break;
			case kompvter::VertexFetcher::format_half_float:
				fetch_half_component<VertexElementComponent>(component[i], data);
				data += sizeof(std::uint16_t);
				break;
			default:
				assert(false);
			case kompvter::VertexFetcher::format_none:
				break;
		}
	}
}

template <typename VertexElementComponent>
void fetch_element(
	std::size_t num_components,
	int format,
	const std::uint8_t* data,
	kompvter::VertexElement<VertexElementComponent>& element)
{
	fetch_element<VertexElementComponent>(num_components, format, data, &element.x);
}

bool kompvter::VertexFetcher::fetch(
	std::size_t index, IntegralVertexElement& element) const
{
	auto d = advance(index);
	if (d == nullptr)
	{
		return false;
	}

	fetch_element<int>(this->num_components, this->format, d, element);
	return true;
}

bool kompvter::VertexFetcher::fetch(
	std::size_t index, UnsignedVertexElement& element) const
{
	auto d = advance(index);
	if (d == nullptr)
	{
		return false;
	}

	fetch_element<unsigned int>(this->num_components, this->format, d, element);
	return true;
}

template <typename Component>
void normalize_component(float& component)
{
	static const float MIN = std::numeric_limits<Component>::min();
	static const float MAX = std::numeric_limits<Component>::max();
	static const float INVERSE_RANGE = 1.0f / (MAX - MIN);

	component = (component - MIN) * INVERSE_RANGE;
}

void normalize(std::size_t num_components, int format, float* components)
{
	for (std::size_t i = 0; i < num_components; ++i)
	{
		switch (format)
		{
			case kompvter::VertexFetcher::format_byte:
				normalize_component<std::int8_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_unsigned_byte:
				normalize_component<std::uint8_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_short:
				normalize_component<std::int16_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_unsigned_short:
				normalize_component<std::uint16_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_integer:
				normalize_component<std::int32_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_unsigned_integer:
				normalize_component<std::uint32_t>(components[i]);
				break;
			case kompvter::VertexFetcher::format_float:
				break;
			default:
				assert(false);
		}
	}
}

void normalize(
	std::size_t num_components, int format,
	kompvter::FloatingPointVertexElement& element)
{
	normalize(num_components, format, &element.x);
}

bool kompvter::VertexFetcher::fetch(
	std::size_t index, FloatingPointVertexElement& element) const
{
	auto d = advance(index);
	if (d == nullptr)
	{
		return false;
	}

	fetch_element<float>(this->num_components, this->format, d, element);

	if (this->normalized)
	{
		normalize(this->num_components, this->format, element);
	}

	return true;
}

bool kompvter::VertexFetcher::extract(
	std::size_t index, std::size_t count,
	int* output, std::size_t num_components, std::size_t stride) const
{
	if ((index + count) * this->element_size > this->data_size)
	{
		return false;
	}
	num_components = std::min(num_components, this->num_components);

	for (std::size_t i = 0; i < count; ++i)
	{
		auto current = advance(index + i);
		fetch_element<int>(num_components, this->format, current, output);

		output = (int*)(((std::uint8_t*)output) + stride);
	}

	return true;
}

bool kompvter::VertexFetcher::extract(
	std::size_t index, std::size_t count,
	unsigned int* output, std::size_t num_components, std::size_t stride) const
{
	if ((index + count) * this->element_size > this->data_size)
	{
		return false;
	}
	num_components = std::min(num_components, this->num_components);

	for (std::size_t i = 0; i < count; ++i)
	{
		auto current = advance(index + i);
		fetch_element<unsigned int>(num_components, this->format, current, output);

		output = (unsigned int*)(((std::uint8_t*)output) + stride);
	}

	return true;
}

bool kompvter::VertexFetcher::extract(
	std::size_t index, std::size_t count,
	float* output, std::size_t num_components, std::size_t stride) const
{
	if ((index + count) * this->element_size > this->data_size)
	{
		return false;
	}
	num_components = std::min(num_components, this->num_components);

	for (std::size_t i = 0; i < count; ++i)
	{
		auto current = advance(index + i);
		for (std::size_t j = 0; j < num_components; ++j)
		{
			switch (this->format)
			{
				case format_byte:
					output[j] = ((const std::int8_t*)current)[j];
					break;
				case format_unsigned_byte:
					output[j] = ((const std::uint8_t*)current)[j];
					break;
				case format_short:
					output[j] = ((const std::int16_t*)current)[j];
					break;
				case format_unsigned_short:
					output[j] = ((const std::uint16_t*)current)[j];
					break;
				case format_integer:
					output[j] = ((const std::int32_t*)current)[j];
					break;
				case format_unsigned_integer:
					output[j] = ((const std::uint32_t*)current)[j];
					break;
				case format_float:
					output[j] = ((const float*)current)[j];
					break;
				case kompvter::VertexFetcher::format_half_float:
					fetch_half_component<float>(output[j], current + j * sizeof(std::uint16_t));
					break;
			}
		}

		if (this->normalized)
		{
			normalize(num_components, this->format, output);
		}

		output = (float*)(((std::uint8_t*)output) + stride);
	}

	return true;
}

std::size_t kompvter::VertexFetcher::get_num_vertices() const
{
	return this->data_size / this->stride;
}

const kompvter::Buffer& kompvter::VertexFetcher::get_buffer() const
{
	return this->buffer;
}

int kompvter::VertexFetcher::get_num_components() const
{
	return this->num_components;
}

int kompvter::VertexFetcher::get_format() const
{
	return this->format;
}

bool kompvter::VertexFetcher::get_normalized() const
{
	return this->normalized;
}

std::size_t kompvter::VertexFetcher::get_stride() const
{
	return this->stride;
}

std::size_t kompvter::VertexFetcher::get_offset() const
{
	return this->offset;
}

const std::uint8_t* kompvter::VertexFetcher::advance(std::size_t index) const
{
	assert(this->data != nullptr);

	auto o = this->offset + index * this->stride;
	if (o >= this->data_size)
	{
		return nullptr;
	}

	return this->data + o;
}

std::size_t kompvter::VertexFetcher::get_component_size(int format)
{
	switch (format)
	{
		case format_none:
			return 0;
		case format_byte:
		case format_unsigned_byte:
			return 1;
		case format_short:
		case format_unsigned_short:
			return 2;
		case format_integer:
		case format_unsigned_integer:
			return 4;
		case format_float:
			return 4;
		case format_half_float:
			return 2;
		default:
			assert(false);
	}

	return 0;
}
