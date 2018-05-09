// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_FINGERPRINT_HPP
#define CHOMSKY_RESOURCES_TEXTURE_FINGERPRINT_HPP

#include <cmath>
#include <memory>
#include <glm/glm.hpp>
#include "kompvter/graphics/textureRegion.hpp"
#include "mantorok/io/datDeserializer.hpp"
#include "mantorok/io/datSerializer.hpp"

namespace chomsky
{
	class TextureFingerprint
	{
	public:
		friend class mantorok::DATSerializer<TextureFingerprint>;
		friend class mantorok::DATDeserializer<TextureFingerprint>;

		TextureFingerprint() = default;
		TextureFingerprint(
			int width, int height,
			int texture_width, int texture_height);
		TextureFingerprint(const TextureFingerprint& other);
		~TextureFingerprint() = default;

		int get_width() const;
		int get_height() const;
		int get_texture_width() const;
		int get_texture_height() const;

		glm::vec3 get_sample(int x, int y) const;
		void set_sample(int x, int y, const glm::vec3& value);

		static glm::vec4 compare(
			const TextureFingerprint& a,
			const TextureFingerprint& b,
			const glm::vec3& max_difference = glm::vec3(HUGE_VALF));

		static TextureFingerprint generate(
			const kompvter::TextureRegion& region,
			int block_width, int block_height,
			const glm::vec3& color_range);

		TextureFingerprint& operator =(const TextureFingerprint& other);

	private:
		int width = 0, height = 0;
		int texture_width = 0, texture_height = 0;
		std::unique_ptr<glm::vec3[]> samples;
	};
}

#endif
