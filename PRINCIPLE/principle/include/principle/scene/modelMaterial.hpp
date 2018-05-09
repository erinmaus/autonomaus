// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_MODEL_MATERIAL_HPP
#define PRINCIPLE_SCENE_MODEL_MATERIAL_HPP

#include "principle/scene/material.hpp"

namespace principle
{
	class ModelMaterial : public Material
	{
	public:
		ModelMaterial() = default;
		~ModelMaterial() = default;

	protected:
		void build() override;
	};
}

#endif
