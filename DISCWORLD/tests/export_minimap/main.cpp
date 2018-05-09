// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp>
#include <png.h>
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/blocks/utilityWorldUpdateBlock.hpp"
#include "kommon/io/jaquesFrameReader.hpp"
#include "glooper/map/minimap.hpp"
#include "glooper/map/minimapManager.hpp"
#include "glooper/map/mapWorldUpdateBlock.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "rosalind/processors/bufferProcessor.hpp"
#include "rosalind/processors/frameProcessor.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "rosalind/processors/modelProcessor.hpp"
#include "rosalind/processors/textureProcessor.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/call.hpp"
#include "thoreau/map.hpp"

struct Image
{
	Image(int width, int height);
	~Image() = default;

	glm::vec4 get_pixel(int x, int y) const;
	void set_pixel(int x, int y, const glm::vec4& value);
	void blend_pixel(int x, int y, const glm::vec4& value);

	void draw_line(
		int x1, int y1, int x2, int y2,
		const glm::vec4& color);

	int width, height;
	std::vector<glm::u8vec4> pixels;
};

Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;
	pixels.resize(width * height, glm::u8vec4(0));
}

glm::vec4 Image::get_pixel(int x, int y) const
{
	auto& pixel = pixels[y * this->height + x];
	return glm::vec4(pixel) / 255.0f;
}

void Image::set_pixel(int x, int y, const glm::vec4& value)
{
	pixels[y * this->width + x] = glm::u8vec4(value * 255.0f);
}

void Image::blend_pixel(int x, int y, const glm::vec4& value)
{
	auto destination_rgb = glm::vec3(value);
	auto destination_alpha = value.a;
	auto source = get_pixel(x, y);
	auto source_rgb = glm::vec3(destination_rgb);
	auto source_alpha = source.a;

	auto result_rgb = source_rgb * source_alpha + destination_rgb * destination_alpha * (1.0f - source_alpha);
	auto result_alpha = source_alpha + destination_alpha * (1.0f - source_alpha);

	set_pixel(x, y, glm::vec4(result_rgb, result_alpha));
}

void Image::draw_line(
	int x1, int y1, int x2, int y2,
	const glm::vec4& color)
{
	if (x1 == x2)
	{
		int min_y = std::min(y1, y2);
		int max_y = std::max(y1, y2);
		for (int y = min_y; y <= max_y; ++y)
		{
			set_pixel(x1, y, color);
		}
	}
	else if (y1 == y2)
	{
		int min_x = std::min(x1, x2);
		int max_x = std::max(x1, x2);
		for (int x = min_x; x <= max_x; ++x)
		{
			set_pixel(x, y1, color);
		}
	}
	else
	{
		float delta_x = x2 - x1;
		float delta_y = y2 - y1;
		float delta_error = glm::abs(delta_y / delta_x);
		float error = delta_error - 0.5f;

		int y = y1;
		for (int x = x1; x <= x2; ++x)
		{
			set_pixel(x, y, color);

			error += delta_error;
			if (error >= 0.5f)
			{
				++y;
				error -= 1.0f;
			}
		}
	}
}

void save_map_png(const std::string& filename, const thoreau::Map& map, int layer)
{
	std::map<thoreau::TileCoordinate, thoreau::Tile> tiles;

	float min_elevation = HUGE_VALF;
	float max_elevation = -HUGE_VALF;
	int min_tile_x = thoreau::TileCoordinate::MAX_COORDINATE;
	int max_tile_x = thoreau::TileCoordinate::MIN_COORDINATE;
	int min_tile_y = thoreau::TileCoordinate::MAX_COORDINATE;
	int max_tile_y = thoreau::TileCoordinate::MIN_COORDINATE;

	thoreau::TileCoordinate current;
	if (map.get_first_tile(current))
	{
		do
		{
			if (current.layer != layer)
			{
				continue;
			}

			auto& tile = map.get_tile(current);
			if (!tile.has_elevation())
			{
				continue;
			}

			min_elevation = std::min(tile.get_elevation(), min_elevation);
			max_elevation = std::max(tile.get_elevation(), max_elevation);
			min_tile_x = std::min(min_tile_x, current.x);
			max_tile_x = std::max(max_tile_x, current.x);
			min_tile_y = std::min(min_tile_y, current.y);
			max_tile_y = std::max(max_tile_y, current.y);

			tiles[current] = tile;
		} while (map.get_next_tile(current, current));
	}

	int width_in_tiles = max_tile_x - min_tile_x + 1;
	int height_in_tiles = max_tile_y - min_tile_y + 1;
	const int PIXELS_PER_TILE = 4;

	Image canvas(
		width_in_tiles * PIXELS_PER_TILE,
		height_in_tiles * PIXELS_PER_TILE);
	auto get_neighbor_elevation = [&tiles](const thoreau::TileCoordinate& current, int offset_x, int offset_y)
	{
		auto neighbor = thoreau::TileCoordinate(
			current.x + offset_x, current.y + offset_y, current.layer);
		auto iter = tiles.find(neighbor);
		if (iter == tiles.end())
		{
			return tiles.find(current)->second.get_elevation();
		}

		return iter->second.get_elevation();
	};

	for (auto& i: tiles)
	{
		float center = i.second.get_elevation();
		float left = get_neighbor_elevation(i.first, -1, 0);
		float right = get_neighbor_elevation(i.first, 1, 0);
		float top = get_neighbor_elevation(i.first, 0, 1);
		float bottom = get_neighbor_elevation(i.first, 0, -1);
		for (int y = 0; y < PIXELS_PER_TILE; ++y)
		{
			for (int x = 0; x < PIXELS_PER_TILE; ++x)
			{
				float sum = 0.0f;
				float x_delta = x / (float)PIXELS_PER_TILE;
				float y_delta = y / (float)PIXELS_PER_TILE;
				float x_from, x_to;
				if (x_delta < 0.5f)
				{
					x_from = left;
					x_to = center;
					x_delta *= 2.0f;
				}
				else
				{
					x_from = center;
					x_to = right;
					x_delta *= 2.0f;
					x_delta -= 1.0f;
				}
				float y_from, y_to;
				if (y_delta < 0.5f)
				{
					y_from = bottom;
					y_to = center;
					y_delta *= 2.0f;
				}
				else
				{
					y_from = center;
					y_to = top;
					y_delta *= 2.0f;
					y_delta -= 1.0f;
				}

				sum += glm::mix(x_from, x_to, x_delta);
				sum += glm::mix(y_from, y_to, y_delta);
				sum /= 2.0f;

				float relative_elevation = (sum - min_elevation) / (max_elevation - min_elevation);
				float hue = relative_elevation * 360.0f;
				auto color = glm::rgbColor(glm::vec3(hue, 0.5f, 0.75f));

				int pixel_x = (i.first.x - min_tile_x) * PIXELS_PER_TILE + x;
				int pixel_y = (i.first.y - min_tile_y) * PIXELS_PER_TILE + y;

				canvas.set_pixel(pixel_x, pixel_y, glm::vec4(color, 1.0f));
			}
		}
	}

	for (auto& i: tiles)
	{
		auto& tile_coordinate = i.first;
		auto& tile = i.second;

		int left = (i.first.x - min_tile_x) * PIXELS_PER_TILE;
		int right = left + PIXELS_PER_TILE;
		int bottom = (i.first.y - min_tile_y) * PIXELS_PER_TILE;
		int top = bottom + PIXELS_PER_TILE;

		if (tile.is_impassable())
		{
			for (int y = bottom; y <= top; ++y)
			{
				for (int x = left; x <= right; ++x)
				{
					canvas.set_pixel(x, y, glm::vec4(1.0f, 0.0f, 0.0f, 0.75f));
				}
			}
		}

		if (tile.is_portal())
		{
			for (int y = bottom; y <= top; ++y)
			{
				for (int x = left; x <= right; ++x)
				{
					canvas.set_pixel(x, y, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}

			continue;
		}

		if (!tile.is_wall() && !tile.is_door())
		{
			continue;
		}

		const auto wall_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		const auto door_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		int flags = tile.get_flags();
		if (flags & thoreau::Tile::flag_door_left)
		{
			canvas.draw_line(left, top, left, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_left)
		{
			canvas.draw_line(left, top, left, bottom, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_right)
		{
			canvas.draw_line(right, top, right, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_right)
		{
			canvas.draw_line(right, top, right, bottom, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_top)
		{
			canvas.draw_line(left, top, right, top, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_top)
		{
			canvas.draw_line(left, top, right, top, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_bottom)
		{
			canvas.draw_line(left, bottom, right, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_bottom)
		{
			canvas.draw_line(left, bottom, right, bottom, wall_color);
		}
	}

	png_image image = { 0 };
	image.version = PNG_IMAGE_VERSION;
	image.width = canvas.width;
	image.height = canvas.height;
	image.format = PNG_FORMAT_RGBA;
	png_image_write_to_file(
		&image, filename.c_str(), false,
		&canvas.pixels[0].x, sizeof(glm::u8vec4) * canvas.width, nullptr);
	png_image_free(&image);
}

int main(int argc, const char* argv[])
{
	if (argc != 3)
	{
		std::fprintf(stderr, "%s <trace> <output.png>\n", argv[0]);

		return 1;
	}

	glooper::ResourceManager resource_manager;
	{
		auto root_path = std::getenv("NECRONOMICON_DATA_PATH");
		if (root_path != nullptr)
		{
			resource_manager.set_path(root_path);
		}
	}
	glooper::Minimap minimap;
	glooper::MinimapManager minimap_manager(resource_manager, minimap);

	std::setbuf(stdout, NULL);

	kommon::JaquesFrameReader reader(argv[1]);
	touchstone::GLTrace gl_trace;
	touchstone::Context context;

	rosalind::BufferProcessor buffer_processor(&context);
	buffer_processor.add(&resource_manager.get_buffer_manager());

	rosalind::TextureProcessor texture_processor(&context);
	texture_processor.add(&resource_manager.get_texture_manager());
	texture_processor.add(&resource_manager.get_texture_fingerprint_manager());
	texture_processor.add(&resource_manager.get_texture_key_manager());

	rosalind::GUIProcessor gui_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager());
	rosalind::ItemIconProcessor item_icon_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager());
	rosalind::MinimapProcessor minimap_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager());
	rosalind::ModelProcessor model_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager());
	rosalind::FrameProcessor frame_processor;

	texture_processor.add(&minimap_manager);
	gui_processor.add(&minimap_manager);
	minimap_processor.add(&minimap_manager);
	frame_processor.add(&minimap_manager);

	thoreau::Map map;
	thoreau::Map final_map;
	chomsky::WorldUpdateManager world_update_manager;
	chomsky::UtilityWorldUpdateBlock utility_block(world_update_manager);
	glooper::MapWorldUpdateBlock map_block(
		world_update_manager,
		&map,
		utility_block.get_transform_state(),
		utility_block.get_bounds());

	buffer_processor.add(&world_update_manager);
	model_processor.add(&world_update_manager);
	frame_processor.add(&world_update_manager);

	std::size_t current_frame = 0;
	while (reader.read(current_frame, gl_trace))
	{
		auto start = std::chrono::system_clock::now();
		frame_processor.on_frame(current_frame);
		frame_processor.enable_draw(current_frame);
		for (auto call: gl_trace)
		{
			buffer_processor.before_update(call);
			texture_processor.before_update(call);
			gui_processor.before_update(call);
			item_icon_processor.before_update(call);
			model_processor.before_update(call);
			minimap_processor.before_update(call);

			context.update(call);

			buffer_processor.after_update(call);
			texture_processor.after_update(call);
			gui_processor.after_update(call);
			item_icon_processor.after_update(call);
			model_processor.after_update(call);
			minimap_processor.after_update(call);
		}
		frame_processor.disable_draw(current_frame);

		world_update_manager.begin_update();
		utility_block.update();
		map_block.update();
		world_update_manager.end_update();

		final_map.merge(*map_block.get_runtime_map());
		if (minimap.get_is_visible())
		{
			minimap.extract(final_map, 0);
		}

		auto stop = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = stop - start;

		std::printf("finished frame %lu in %.4lf ms\n", current_frame, diff.count() * 1000);

		++current_frame;
		gl_trace.clear();
	}

	std::printf("done!\n");
	save_map_png(argv[2], final_map, 0);

	return 0;
}
