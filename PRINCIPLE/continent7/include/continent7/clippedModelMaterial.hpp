// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT7_SCENE_CLIPPED_MODEL_MATERIAL_HPP
#define CONTINENT7_SCENE_CLIPPED_MODEL_MATERIAL_HPP

#include <glm/glm.hpp>
#include "kvlc/geometry/bounds.hpp"
#include "principle/scene/modelMaterial.hpp"

namespace continent7
{
	class ClippedModelMaterial : public principle::ModelMaterial
	{
	public:
		ClippedModelMaterial() = default;
		~ClippedModelMaterial() = default;

		const kvlc::Bounds& get_bounds() const;
		void set_bounds(const kvlc::Bounds& bounds);

		void show();
		void hide();

		void use() override;
		bool get_is_enabled() const override;

	private:
		bool is_visible = true;
		kvlc::Bounds bounds = kvlc::Bounds(glm::vec3(-HUGE_VALF), glm::vec3(HUGE_VALF));
	};
}

#endif
