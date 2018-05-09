// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/color_space_YCoCg.hpp>
#include "chomsky/resources/textureFingerprint.hpp"

chomsky::TextureFingerprint::TextureFingerprint(
	int width, int height,
	int texture_width, int texture_height)
{
	assert(width > 0);
	assert(height > 0);
	assert(texture_width > 0);
	assert(texture_height > 0);

	this->width = width;
	this->height = height;
	this->texture_width = texture_width;
	this->texture_height = texture_height;
	this->samples = std::make_unique<glm::vec3[]>(this->width * this->height);
}

chomsky::TextureFingerprint::TextureFingerprint(const TextureFingerprint& other)
{
	*this = other;
}

int chomsky::TextureFingerprint::get_width() const
{
	return this->width;
}

int chomsky::TextureFingerprint::get_height() const
{
	return this->height;
}

int chomsky::TextureFingerprint::get_texture_width() const
{
	return this->texture_width;
}

int chomsky::TextureFingerprint::get_texture_height() const
{
	return this->texture_height;
}

glm::vec3 chomsky::TextureFingerprint::get_sample(int x, int y) const
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return this->samples[y * this->width + x];
}

void chomsky::TextureFingerprint::set_sample(
	int x, int y, const glm::vec3& value)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	this->samples[y * this->width + x] = value;
}

glm::vec4 chomsky::TextureFingerprint::compare(
	const TextureFingerprint& a, const TextureFingerprint& b,
	const glm::vec3& max_differences)
{
	if (a.width != b.width)
	{
		return glm::vec4(HUGE_VALF);
	}

	if (a.height != b.height)
	{
		return glm::vec4(HUGE_VALF);
	}

	int num_samples = a.width * a.height;
	auto a_samples = a.samples.get();
	auto b_samples = b.samples.get();

	glm::vec3 differences = glm::vec3(0.0f);
	float max_difference = 0.0f;
	for (auto i = 0; i < num_samples; ++i)
	{
		auto difference = glm::abs(*(a_samples++) - *(b_samples++));

		if (difference.x > max_differences.x)
		{
			return glm::vec4(HUGE_VALF);
		}

		if (difference.y > max_differences.y)
		{
			return glm::vec4(HUGE_VALF);
		}

		if (difference.z > max_differences.z)
		{
			return glm::vec4(HUGE_VALF);
		}

		differences += difference;
		max_difference = glm::max(max_difference, difference.x);
		max_difference = glm::max(max_difference, difference.y);
		max_difference = glm::max(max_difference, difference.z);
	}

	return glm::vec4(differences, max_difference);
}

chomsky::TextureFingerprint chomsky::TextureFingerprint::generate(
	const kompvter::TextureRegion& region,
	int block_width, int block_height,
	const glm::vec3& color_range)
{
	auto block = std::make_unique<glm::u8vec4[]>(block_width * block_height);

	int remainder_width = block_width - (region.get_width() % block_width);
	int fingerprint_width = (region.get_width() + remainder_width) / block_width;
	int remainder_height = block_height - (region.get_height() % block_height);
	int fingerprint_height = (region.get_height() + remainder_height) / block_height;
	TextureFingerprint fingerprint(
		fingerprint_width, fingerprint_height,
		region.get_width(), region.get_height());

	for (int y = 0; y < region.get_height(); y += block_height)
	{
		for (int x = 0; x < region.get_width(); x += block_height)
		{
			int width = glm::min(block_width, region.get_width() - x);
			int height = glm::min(block_height, region.get_height() - y);
			region.get_pixels(x, y, width, height, (std::uint8_t*)block.get());

			glm::vec3 sum;
			auto current = block.get();
			for (int j = 0; j < height; ++j)
			{
				for (int i = 0; i < width; ++i)
				{
					glm::vec3 rgb = glm::vec3(*(current++)) * (1.0f / 255.0f);
					glm::vec3 sample = glm::rgb2YCoCg(rgb);
					sum += sample;
				}
			}

			glm::vec3 result = sum / (float)(width * height);
			result.x = glm::roundEven(result.x * color_range.x);
			result.y = glm::roundEven(result.y * (2.0f * color_range.y) + color_range.y);
			result.z = glm::roundEven(result.z * (2.0f * color_range.z) + color_range.z);

			fingerprint.set_sample(x / block_width, y / block_height, result);
		}
	}

	return fingerprint;
}

chomsky::TextureFingerprint& chomsky::TextureFingerprint::operator =(
	const TextureFingerprint& other)
{
	this->width = other.width;
	this->height = other.height;
	this->texture_width = other.texture_width;
	this->texture_height = other.texture_height;

	this->samples.reset();
	if (other.samples)
	{
		assert(this->width != 0);
		assert(this->height != 0);
		int num_samples = this->width * this->height;
		this->samples = std::make_unique<glm::vec3[]>(num_samples);

		auto output = this->samples.get();
		auto input = other.samples.get();
		for (int i = 0; i < num_samples; ++i)
		{
			*(output++) = *(input++);
		}
	}
	else
	{
		assert(this->width == 0);
		assert(this->height == 0);
	}

	return *this;
}
