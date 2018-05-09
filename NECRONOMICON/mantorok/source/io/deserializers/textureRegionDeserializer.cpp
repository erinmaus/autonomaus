// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/textureRegionDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::TextureRegion>::deserialize(
	kompvter::TextureRegion& region, DATReader& reader)
{
	if (!reader.match_section("kompvter.TextureRegion::root"))
	{
		return;
	}

	std::uint32_t width, height;
	std::int32_t is_compressed;
	std::int32_t pixel_format, pixel_type;
	std::unique_ptr<std::uint8_t[]> pixels_data;
	std::size_t pixels_data_size;

	reader.read(width);
	reader.read(height);
	reader.read(is_compressed);
	reader.read(pixel_format);
	reader.read(pixel_type);
	reader.read(pixels_data, pixels_data_size);

	reader.leave_section();

	region = kompvter::TextureRegion(
		0,
		0, 0,
		width, height,
		pixel_format, pixel_type, is_compressed,
		pixels_data.get(), pixels_data_size);
}
