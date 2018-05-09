// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ZEITGEIST_THUMBNAIL_HPP
#define ZEITGEIST_THUMBNAIL_HPP

#include <tuple>
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvncer/graphics/framebuffer.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/graphics/texture.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/scene.hpp"
#include "principle/scene/staticCamera.hpp"

namespace principle
{
	class Thumbnail
	{
	public:
		Thumbnail(int width, int height);
		~Thumbnail() = default;

		int get_width() const;
		int get_height() const;

		void add_model(
			const kvre::Model* model,
			const glm::mat4& model_matrix,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters);
		void add_terrain(
			const kvre::Terrain* terrain,
			const glm::mat4& model_matrix,
			const kompvter::DrawParameters& draw_parameters);
		void clear();

		kvncer::Shader* get_shader();
		void load_shader_from_file(
			const std::string& vertex_filename,
			const std::string& fragment_filename);

		glm::vec4 get_background_color() const;
		void set_background_color(const glm::vec4& value);

		kvncer::Texture* draw(const glm::mat4& model);
		void start_draw(bool use_framebuffer = true);
		void draw_clipped_to_tile(int x, int y, const glm::mat4& model);
		kvncer::Texture* stop_draw();

		kvlc::Bounds get_bounds() const;
		void set_bounds(const kvlc::Bounds& bounds);
		void unset_bounds();

	private:
		StaticCamera camera;
		Scene scene;
		ModelMaterial model_material;

		glm::vec4 background_color = glm::vec4(0.0f);

		void compute_projection_view_model(
			const glm::mat4& input_model,
			glm::mat4& result_projection,
			glm::mat4& result_view,
			glm::mat4& result_model) const;

		std::map<const kvre::Model*, kvncer::MeshBuffer> model_vertex_buffers;
		std::map<const kvre::Terrain*, kvncer::MeshBuffer> terrain_vertex_buffers;

		struct ModelData
		{
			kvncer::Mesh mesh;
			kvncer::MeshBuffer index_buffer;
		};
		typedef std::unique_ptr<ModelData> ModelDataPointer;
		std::vector<ModelDataPointer> model_data;

		std::unique_ptr<kvncer::Shader> shader;
		std::unique_ptr<kvncer::Framebuffer> framebuffer;
		std::unique_ptr<kvncer::Texture> color_attachment;

		kvlc::Bounds bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
		kvlc::Bounds custom_bounds;
		bool custom_bounds_set = false;
	};
}

#endif
