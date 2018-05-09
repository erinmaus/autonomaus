// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_DRAW_HPP
#define CHOMSKY_WORLD_ACTOR_DRAW_HPP

#include <glm/glm.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include "kvre/model/model.hpp"

namespace chomsky
{
	class ActorDraw
	{
	public:
		ActorDraw() = default;
		~ActorDraw() = default;

		const glm::mat4& get_model_matrix() const;
		void set_model_matrix(const glm::mat4& value);

		const glm::vec3& get_world_position() const;

		void add_model(const kvre::Model* model);
		void add_tag(int tag);
		void add_bone(const kvre::Model* model, const glm::mat4& bone);

		std::size_t get_num_bones() const;
		std::size_t get_num_bones(const kvre::Model* model) const;
		glm::mat4 get_bone(std::size_t index) const;
		glm::mat4 get_bone(const kvre::Model* model, std::size_t index) const;

	private:
		glm::mat4 model_matrix = glm::mat4(1.0f);
		glm::vec3 world_position = glm::vec3(0.0f);

		typedef std::vector<glm::mat4> Bones;
		typedef boost::container::flat_map<const kvre::Model*, Bones> BoneCollection;
		BoneCollection bones_by_model;
		Bones bones;

		typedef boost::container::flat_set<const kvre::Model*> ModelCollection;
		ModelCollection models;

		typedef boost::container::flat_set<int> TagCollection;
		TagCollection tags;

	public:
		typedef ModelCollection::const_iterator const_model_iterator;
		typedef TagCollection::const_iterator const_tag_iterator;

		const_model_iterator models_begin() const;
		const_model_iterator models_end() const;

		const_tag_iterator tags_begin() const;
		const_tag_iterator tags_end() const;
	};
}

#endif
