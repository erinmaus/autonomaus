// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/textureRegionSerializer.hpp"

void mantorok::DATSerializer<kompvter::TextureRegion>::serialize(
	const kompvter::TextureRegion& region, DATWriter& writer)
{
	writer.begin_section("kompvter.TextureRegion::root");

	writer.write<std::uint32_t>(region.get_width(), "width");
	writer.write<std::uint32_t>(region.get_height(), "height");
	writer.write<std::int32_t>(region.get_is_compressed(), "is_compressed");
	writer.write<std::int32_t>(region.get_pixel_format(), "pixel_format");
	writer.write<std::int32_t>(region.get_pixel_type(), "pixel_type");
	writer.write(region.get_pixels_data(), region.get_pixels_size(), "pixels");

	writer.end_section();
}
