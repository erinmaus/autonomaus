// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_WORLD_OVERLAY_HPP
#define AUTONOMAUS_GRAPHICS_WORLD_OVERLAY_HPP

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/mesh.hpp"
#include "autonomaus/graphics/overlay.hpp"
#include "autonomaus/graphics/program.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"

namespace autonomaus
{
	class WorldOverlay : public Overlay
	{
	public:
		WorldOverlay(Renderer& renderer, const DrawState& draw_state);
		WorldOverlay() = default;

		void unload_resources() override;
		void load_resources() override;

		void draw_box(
			const glm::vec3& position,
			const glm::vec3& size,
			const glm::vec4& color);
		void draw_box_outline(
			const glm::vec3& position,
			const glm::vec3& size,
			const glm::vec4& color);
		void draw_sphere(
			const glm::vec3& position,
			float radius,
			const glm::vec4& color);

		void draw_box(
			const glm::mat4& model_matrix,
			const glm::vec3& size,
			const glm::vec4& color);
		void draw_box_outline(
			const glm::mat4& model_matrix,
			const glm::vec3& size,
			const glm::vec4& color);
		void draw_sphere(
			const glm::mat4& model_matrix,
			float radius,
			const glm::vec4& color);

		void draw_tile(
			const thoreau::Map& map,
			const thoreau::TileCoordinate& tile_coordinate,
			const glm::vec4& color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
			int radius = 1,
			float thickness = 4.0f,
			float offset = 16.0f);

	protected:
		void start_draw(int id, float age, float lifespan) override;
		void stop_draw() override;

	private:
		struct MeshDrawObject : public DrawObject
		{
			MeshDrawObject(WorldOverlay* overlay);
			void draw() override;

			WorldOverlay* overlay = nullptr;
			glm::mat4 model_matrix = glm::mat4(1.0f);
			glm::vec4 color = glm::vec4(1.0f);
			int primitive = Mesh::primitive_points;
			int mesh;
		};

		struct GridDrawObject : public DrawObject
		{
			GridDrawObject(WorldOverlay* overlay);
			void draw() override;

			void add_point(float tile_x, float tile_y, float elevation);

			WorldOverlay* overlay = nullptr;
			std::vector<glm::vec3> points;
			float line_thickness = 64.0f;
			glm::vec3 position = glm::vec3(0.0f);
			glm::vec4 color = glm::vec4(1.0f);
		};

		enum
		{
			mesh_box = 0,
			mesh_box_outline,
			mesh_sphere,
			mesh_max
		};
		std::unique_ptr<Mesh> meshes[mesh_max];
		std::unique_ptr<Program> program;
	};
}

#endif
