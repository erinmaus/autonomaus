// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_ITEM_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_ITEM_CORTEX_HPP

#include <string>
#include <vector>
#include "autonomaus/state/memory/cortex.hpp"
#include "mapp/game/item.hpp"

namespace autonomaus
{
	class ItemCortex : public Cortex
	{
	public:
		ItemCortex(Autonomaus& autonomaus);
		~ItemCortex() = default;

		void remember_item(const std::string& runtime_key);
		void add_name(
			const std::string& key,
			const std::string& name,
			int object_id = mapp::Item::NULL_OBJECT_ID);

		bool has_name(const std::string& key) const;
		bool is_ambiguous(const std::string& key) const;
		std::string get_name(const std::string& key) const;
		std::vector<std::string> get_names(const std::string& key) const;
	};
}

#endif
