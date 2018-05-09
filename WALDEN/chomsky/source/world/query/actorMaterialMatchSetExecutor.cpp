// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/query/actorMaterialSetMatchExecutor.hpp"

chomsky::ActorMaterialSetMatchExecutor::ActorMaterialSetMatchExecutor(
	const AnimatedWorldUpdateBlock* animated_block,
	const TextureFingerprintManager& texture_fingerprint_manager)
{
	this->animated_block = animated_block;
	this->texture_fingerprint_manager = &texture_fingerprint_manager;
}

bool chomsky::ActorMaterialSetMatchExecutor::execute(
	const thoreau::WorldCoordinate& location,
	const MaterialSetMatch& match) const
{
	auto material_state = this->animated_block->get_actor_materials_state();
	if (!material_state->has(location))
	{
		return false;
	}

	auto& material_set = material_state->get(location);
	if (match.count() > material_set.count())
	{
		return false;
	}

	for (auto material_definition: match)
	{
		bool match = false;
		auto& query_fingerprint = material_definition.get_fingerprint();
		auto max_dissimilarity = material_definition.get_max_dissimilarity();
		auto max_color_difference = material_definition.get_max_color_difference();

		for (auto material: material_set)
		{
			auto texture_name = std::get<MaterialSet::TEXTURE>(material);
			auto region_name = std::get<MaterialSet::REGION>(material);
			auto color = std::get<MaterialSet::COLOR>(material);

			auto color_difference = glm::abs(color - material_definition.get_color());
			if (color_difference.x > max_color_difference.x ||
				color_difference.y > max_color_difference.y ||
				color_difference.z > max_color_difference.z)
			{
				continue;
			}

			auto target_fingerprint = this->texture_fingerprint_manager->get(
				texture_name, region_name);
			if (query_fingerprint.get_texture_width() != target_fingerprint.get_texture_width() ||
				query_fingerprint.get_texture_height() != target_fingerprint.get_texture_height())
			{
				continue;
			}

			auto dissimilarity = TextureFingerprint::compare(
				query_fingerprint,
				target_fingerprint);
			if (dissimilarity.x <= max_dissimilarity.x &&
				dissimilarity.y <= max_dissimilarity.y &&
				dissimilarity.z <= max_dissimilarity.z)
			{
				match = true;
				break;
			}
		}

		if (!match)
		{
			return false;
		}
	}

	return true;
}
