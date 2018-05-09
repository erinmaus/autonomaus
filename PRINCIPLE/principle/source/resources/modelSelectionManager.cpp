// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "chomsky/resources/modelVertexCloud.hpp"
#include "kvncer/render/gl.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "principle/graphics/utility.hpp"
#include "principle/resources/modelSelectionManager.hpp"
#include "principle/scene/modelNode.hpp"

principle::ModelSelectionManager::ModelSelectionManager()
{
	this->selected_node_shader = principle::Material::load_shader_from_file(
		"model.vert.glsl", "untextured_tinted_model.frag.glsl");
	this->selected_node_material.set_shader(selected_node_shader);
}

principle::ModelSelectionManager::~ModelSelectionManager()
{
	delete this->selected_node_shader;
}

void principle::ModelSelectionManager::draw(
	const Camera& camera,
	const glm::vec4& tint_color)
{
	for (auto& draw_data: this->selection_draw_data)
	{
		draw_data->node->override_mesh_draw_parameters(
			kvncer::gl::marshal_index_mapper_format_from_kompvter(draw_data->selection->get_index_mapper().get_format()),
			draw_data->selection->get_index_offset_start(),
			draw_data->selection->get_index_offset_stop());
		draw_data->node->set_model_matrix(draw_data->selection->get_model_matrix());
	}

	this->selected_node_shader->use();
	this->selected_node_shader->set("principle_Tint", tint_color);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	this->scene.set_camera(&camera);
	this->scene.draw();

	this->scene.set_camera(nullptr);
}

principle::ModelSelection* principle::ModelSelectionManager::select(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	this->selections.emplace_back(new ModelSelection(model, index_mapper));
	auto selection = this->selections.back().get();

	auto m = std::make_unique<ModelSelectionDrawData>();
	{
		std::shared_ptr<kvncer::MeshBuffer> vertex_buffer;
		{
			auto iter = this->vertex_buffers.find(model);
			if (iter != this->vertex_buffers.end())
			{
				vertex_buffer = iter->second.lock();
			}

			if (!vertex_buffer)
			{
				vertex_buffer = std::make_shared<kvncer::MeshBuffer>();
				this->vertex_buffers[model] = vertex_buffer;
			}
		}
		generate_untextured_model_vertex_data(
			model, m->mesh, *vertex_buffer.get());
		m->vertex_buffer = vertex_buffer;

		if (index_mapper)
		{
			kompvter::DrawParameters draw_parameters(
				kompvter::DrawParameters::primitive_triangles,
				0, index_mapper.get_num_indices() * index_mapper.get_index_size());
			generate_index_data(
				index_mapper, draw_parameters,
				m->mesh, m->index_buffer);
		}
	}
	{
		m->node = this->scene.add_node<principle::ModelNode>(&m->mesh);
		m->node->set_material(&this->selected_node_material);
	}
	m->selection = selection;
	this->selection_draw_data.push_back(std::move(m));

	return selection;
}

void principle::ModelSelectionManager::unselect(ModelSelection* selection)
{
	for(auto i = this->selections.begin(); i != this->selections.end(); ++i)
	{
		if ((*i).get() == selection)
		{
			this->selections.erase(i);
			break;
		}
	}

	for (auto i = this->selection_draw_data.begin(); i != this->selection_draw_data.end(); ++i)
	{
		if ((*i)->selection == selection)
		{
			this->scene.remove_node((*i)->node);
			this->selection_draw_data.erase(i);
			break;
		}
	}
}

void principle::ModelSelectionManager::unselect_all()
{
	this->selections.clear();
	this->vertex_buffers.clear();
	this->selection_draw_data.clear();
	this->scene.reset();
}

std::size_t principle::ModelSelectionManager::count() const
{
	return this->selections.size();
}

bool principle::ModelSelectionManager::empty() const
{
	return this->selections.empty();
}

principle::ModelSelection* principle::ModelSelectionManager::at(std::size_t index)
{
	return this->selections.at(index).get();
}

const principle::ModelSelection* principle::ModelSelectionManager::at(
	std::size_t index) const
{
	return this->selections.at(index).get();
}

principle::ModelSelectionManager::iterator principle::ModelSelectionManager::begin()
{
	return iterator(this->selections.begin());
}

principle::ModelSelectionManager::const_iterator
principle::ModelSelectionManager::begin() const
{
	return const_iterator(this->selections.begin());
}

principle::ModelSelectionManager::iterator principle::ModelSelectionManager::end()
{
	return iterator(this->selections.end());
}

principle::ModelSelectionManager::const_iterator
principle::ModelSelectionManager::end() const
{
	return const_iterator(this->selections.end());
}

principle::ModelSelectionManager::iterator::iterator(
	ModelSelectionCollection::iterator source) :
		ModelSelectionCollection::iterator(source)
{
	// NOTHING.
}

principle::ModelSelectionManager::iterator::reference_type
principle::ModelSelectionManager::iterator::operator *() const
{
	auto& orig = ModelSelectionCollection::iterator::operator *();

	return *orig.get();
}

principle::ModelSelectionManager::iterator::pointer_type
principle::ModelSelectionManager::iterator::operator ->() const
{
	auto orig = ModelSelectionCollection::iterator::operator ->();

	return orig->get();
}

principle::ModelSelectionManager::const_iterator::const_iterator(
	ModelSelectionCollection::const_iterator source) :
		ModelSelectionCollection::const_iterator(source)
{
	// NOTHING.
}

principle::ModelSelectionManager::const_iterator::reference_type
principle::ModelSelectionManager::const_iterator::operator *() const
{
	auto& orig = ModelSelectionCollection::const_iterator::operator *();

	return *orig.get();
}

principle::ModelSelectionManager::const_iterator::pointer_type
principle::ModelSelectionManager::const_iterator::operator ->() const
{
	auto orig = ModelSelectionCollection::const_iterator::operator ->();

	return orig->get();
}
