// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include "chomsky/io/world/actorAnimation.hpp"
#include "chomsky/world/actorAnimation.hpp"

void mantorok::DATSerializer<chomsky::ActorAnimation>::serialize(
	const chomsky::ActorAnimation& animation,
	DATWriter& writer)
{
	writer.begin_section("chomsky.ActorAnimation::root", DATWriter::flag_clean);

	int num_frames = animation.get_num_frames();
	writer.write(num_frames, "num_frames");

	float hit_ratio = animation.hits;
	writer.write(hit_ratio, "hit_ratio");

	float distance = animation.distance;
	writer.write(distance, "distance");

	for (auto& frame: animation.frames)
	{
		writer.begin_section("chomsky.ActorAnimation::frame",  DATWriter::flag_clean);

		int num_bones = frame.size();
		writer.write(num_bones, "num_bones");
		writer.write((const std::uint8_t*)&frame[0].x, num_bones * sizeof(glm::vec3), "bones");

		writer.end_section();
	}
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::ActorAnimation>::deserialize(
	chomsky::ActorAnimation& animation,
	DATReader& reader)
{
	if (reader.match_section("chomsky.ActorAnimation::root"))
	{
		animation = chomsky::ActorAnimation();

		int num_frames;
		reader.read(num_frames);
		animation.frames.resize(num_frames);

		reader.read(animation.hits);
		reader.read(animation.distance);

		for (int i = 0; i < num_frames; ++i)
		{
			if (!reader.match_section("chomsky.ActorAnimation::frame"))
			{
				break;
			}

			int num_bones;
			reader.read(num_bones);

			animation.frames[i].resize(num_bones);

			std::unique_ptr<std::uint8_t[]> data;
			std::size_t size;
			reader.read(data, size);

			if (size == num_bones * sizeof(glm::vec3))
			{
				std::memcpy(&animation.frames[i][0].x, data.get(), size);
			}

			reader.leave_section();
		}

		reader.leave_section();
	}
}
