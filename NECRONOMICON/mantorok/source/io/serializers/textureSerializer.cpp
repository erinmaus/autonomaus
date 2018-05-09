// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/textureSerializer.hpp"
#include "mantorok/io/serializers/textureRegionSerializer.hpp"

void mantorok::DATSerializer<kompvter::Texture>::serialize(
	const kompvter::Texture& texture, DATWriter& writer)
{
	writer.begin_section("kompvter.Texture::root");

	writer.write<std::int32_t>(texture.get_name(), "name");
	writer.write<std::uint32_t>(texture.get_width(), "width");
	writer.write<std::uint32_t>(texture.get_height(), "height");
	writer.write<std::int32_t>(texture.get_is_compressed(), "is_compressed");
	writer.write<std::int32_t>(texture.get_pixel_format(), "pixel_format");
	writer.write<std::int32_t>(texture.get_pixel_type(), "pixel_type");

	writer.begin_section("kompvter.Texture::regions");
	for (auto& region: texture)
	{
		writer.begin_section("kompvter.Texture::region");
		writer.write(region.get_x(), "x");
		writer.write(region.get_y(), "y");
		DATSerializer<kompvter::TextureRegion>::serialize(region, writer);
		writer.end_section();
	}
	writer.end_section();

	writer.end_section();
}
