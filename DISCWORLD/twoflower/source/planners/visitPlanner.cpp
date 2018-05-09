// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include "visitPlanner.hpp"

twoflower::VisitPlanner::VisitPlanner(const Brochure& brochure, const Wizard& wizard) :
	Planner(brochure, wizard)
{
	// Nothing.
}

bool twoflower::VisitPlanner::visit(const Resource& from, const Resource& to)
{
	typedef std::pair<int, int> ResourceActionNode;
	std::map<ResourceActionNode, ResourceActionNode> path;
	std::map<int, float> f_score;
	std::map<ResourceActionNode, float> g_score;
	std::set<ResourceActionNode> complete;

	std::unordered_map<int, Action> actions;

	typedef std::pair<Resource, Action> ResourceActionInstance;
	std::queue<ResourceActionInstance> queue;

	typedef std::vector<std::pair<Action, Resource>> Neighbors;
	{
		Neighbors neighbors;
		wizard().neighbors(from, neighbors);

		for (auto& neighbor: neighbors)
		{
			queue.push(std::make_pair(neighbor.second, neighbor.first));

			auto key = std::make_pair(neighbor.second.get_id(), neighbor.first.get_id());
			f_score[neighbor.second.get_id()] = g_score[key] = wizard().distance(from, neighbor.second, neighbor.first);
			actions[neighbor.first.get_id()] = neighbor.first;
		}
	}

	while (!queue.empty())
	{
		auto top = queue.front();
		auto& resource = top.first;
		auto& action = top.second;

		queue.pop();

		if (resource.get_id() == to.get_id())
		{
			complete.emplace(resource.get_id(), action.get_id());
		}
		else
		{
			Neighbors neighbors;
			wizard().neighbors(resource, neighbors);

			for (auto& i: neighbors)
			{
				auto& link = i.first;
				auto& neighbor = i.second;

				float distance = wizard().distance(resource, neighbor, link);

				auto key = std::make_pair(neighbor.get_id(), link.get_id());
				auto g_iter = g_score.find(key);
				if (g_iter == g_score.end())
				{
					g_score[key] = distance;
					actions[link.get_id()] = link;
				}

				auto f_iter = f_score.find(neighbor.get_id());
				if (f_iter == f_score.end() || f_iter->second > distance + f_score[resource.get_id()])
				{
					f_score[neighbor.get_id()] = distance + f_score[resource.get_id()];
					queue.push(std::make_pair(neighbor, link));

					path[key] = std::make_pair(resource.get_id(), action.get_id());
				}
			}
		}
	}

	if (!complete.empty())
	{
		float best = HUGE_VALF;
		for (auto& i: complete)
		{
			std::vector<ResourceActionNode> result;
			result.push_back(i);

			float cost = 0.0f;
			auto current = i;
			while (path.find(current) != path.end())
			{
				auto iter = path.find(current);
				auto& node = iter->second;
				result.insert(result.begin(), node);
				cost += g_score[node];

				current = node;
			}

			if (cost < best)
			{
				plan().clear();

				for (auto& node: result)
				{
					plan().add(actions[node.second], 1, g_score[node]);
				}
			}
		}
	}

	return !complete.empty();
}
