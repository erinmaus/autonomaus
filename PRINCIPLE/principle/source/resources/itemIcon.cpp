#include "principle/resources/itemIcon.hpp"

std::size_t principle::ItemIcon::get_num_draws() const
{
	return this->draws.size();
}

const kompvter::ModelDrawEvent& principle::ItemIcon::get_draw(
	std::size_t index) const
{
	return this->draws.at(index);
}

const chomsky::ItemIconSilhouette& principle::ItemIcon::get_silhouette() const
{
	return this->silhouette;
}

const chomsky::TileSpacePointCloudMatch& principle::ItemIcon::get_point_cloud_match() const
{
	return this->point_cloud_match;
}

kvncer::Texture* principle::ItemIcon::get_texture() const
{
	return this->texture.get();
}

void principle::ItemIcon::to_thumbnail(Thumbnail& thumbnail) const
{
	for (auto& draw: this->draws)
	{
		kvre::Model model(draw.get_vertex_attributes());
		thumbnail.add_model(
			&model, glm::mat4(1.0f),
			draw.get_vertex_index_mapper(),
			draw.get_draw_parameters());
	}
}
