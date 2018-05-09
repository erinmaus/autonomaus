// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_COMPONENT_MANAGER_HPP
#define AUTONOMAUS_COMPONENTS_COMPONENT_MANAGER_HPP

#include <cassert>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "autonomaus/components/component.hpp"

namespace autonomaus
{
	class Autonomaus;

	class ComponentManager
	{
	public:
		explicit ComponentManager(Autonomaus& autonomaus);
		~ComponentManager();

		void show();
		void hide();
		void toggle();
		void reload();
		void update();

		template <typename Component>
		Component* add();

		template <typename Component>
		Component* get();

		template <typename Component>
		void remove();

	private:
		void draw();

		Autonomaus* autonomaus;

		std::vector<Component*> components;
		std::unordered_map<const int*, Component*> tags;
		std::vector<Component*> pending_add;
		std::unordered_set<Component*> pending_remove;

		template <typename Component>
		struct Tag
		{
			static const int VALUE = 0;
		};

#ifdef NDEBUG
		bool visible = false;
#else
		bool visible = true;
#endif
	};
}

template <typename Component>
const int autonomaus::ComponentManager::Tag<Component>::VALUE;

template <typename Component>
Component* autonomaus::ComponentManager::add()
{
	auto tag = &Tag<Component>::VALUE;
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		return (Component*)iter->second;
	}

	auto component = new Component(*autonomaus);
	this->pending_add.push_back(component);
	this->tags.emplace(tag, component);

	return component;
}

template <typename Component>
Component* autonomaus::ComponentManager::get()
{
	auto tag = &Tag<Component>::VALUE;
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		return (Component*)iter->second;
	}

	return nullptr;
}

template <typename Component>
void autonomaus::ComponentManager::remove()
{
	auto tag = &Tag<Component>::VALUE;
	auto iter = this->tags.find(tag);
	if (iter == this->tags.end())
	{
		return;
	}

	delete iter->second;
	this->pending_remove.insert(iter->second);
	this->tags.erase(iter);
}

#endif
