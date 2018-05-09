// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "plato/utility.hpp"
#include "mapp/atlas/link.hpp"
#include "twoflower/relationships/requirement.hpp"
#include "thoreau/pathFinder.hpp"

bool plato::EmptyLuggage::has(const twoflower::Resource& resource, float quantity) const
{
	return false;
}

float plato::EmptyLuggage::count(const twoflower::Resource& resource) const
{
	return 0.0f;
}

void plato::connect_nearest_neighbors(
	twoflower::Brochure& brochure,
	mapp::Atlas& atlas,
	const thoreau::Map& map)
{
	auto resources = brochure.resources();

	std::unordered_map<int, mapp::Location> nodes;
	std::vector<mapp::Location> detached_nodes;
	for (auto i = resources.by_type("location"); i != resources.end(); ++i)
	{
		mapp::Location location;
		if (atlas.location(*i, location))
		{
			if (location.anchor.layer != 0)
			{
				detached_nodes.push_back(location);
				continue;
			}

			if (location.resource.get_id() == atlas.root().resource.get_id())
			{
				continue;
			}

			nodes.emplace(i->get_id(), location);

			auto links = atlas.links(location);
			for (auto& link: links)
			{
				if (link.type == mapp::TravelType::walk)
				{
					atlas.remove(link);
				}
			}
		}
	}

	if (nodes.empty())
	{
		return;
	}

	typedef std::pair<thoreau::TileCoordinate, int> Vertex;
	typedef std::pair<Vertex, Vertex> Edge;
	struct Triangle
	{
		Vertex a, b, c;
		bool contains(Vertex& v)
		{
			float ab = (a.first.x * a.first.x) + (a.first.y * a.first.y);
			float cd = (b.first.x * b.first.x) + (b.first.y * b.first.y);
			float ef = (c.first.x * c.first.x) + (c.first.y * c.first.y);

			float circum_x = (ab * (c.first.y - b.first.y) + cd * (a.first.y - c.first.y) + ef * (b.first.y - a.first.y)) / (a.first.x * (c.first.y - b.first.y) + b.first.x * (a.first.y - c.first.y) + c.first.x * (b.first.y - a.first.y)) / 2.f;
			float circum_y = (ab * (c.first.x - b.first.x) + cd * (a.first.x - c.first.x) + ef * (b.first.x - a.first.x)) / (a.first.y * (c.first.x - b.first.x) + b.first.y * (a.first.x - c.first.x) + c.first.y * (b.first.x - a.first.x)) / 2.f;
			float circum_radius = std::sqrt(((a.first.x - circum_x) * (a.first.x - circum_x)) + ((a.first.y - circum_y) * (a.first.y - circum_y)));

			float distance = std::sqrt(((v.first.x - circum_x) * (v.first.x - circum_x)) + ((v.first.y - circum_y) * (v.first.y - circum_y)));
			return distance <= circum_radius;
		}
	};
	std::vector<Triangle> triangles;
	auto add_triangle = [](std::vector<Triangle>& triangles, Vertex a, Vertex b, Vertex c)
	{
		Triangle t = { a, b, c };
		triangles.push_back(t);
	};

	// Calculate super triangle.
	Triangle super_triangle;
	{
		float min_x = nodes.begin()->second.anchor.x;
		float min_y = nodes.begin()->second.anchor.y;
		float max_x = min_x;
		float max_y = min_y;

		for (auto& node: nodes)
		{
			min_x = std::min(min_x, (float)node.second.anchor.x);
			max_x = std::max(max_x, (float)node.second.anchor.x);
			min_y = std::min(min_y, (float)node.second.anchor.y);
			max_y = std::max(max_y, (float)node.second.anchor.y);
		}

		float difference_x = max_x - min_x;
		float difference_y = max_y - min_y;
		float delta_max = std::max(difference_x, difference_y);
		float mid_x = (min_x + max_x) / 2.0f;
		float mid_y = (min_y + max_y) / 2.0f;

		Vertex p1 = std::make_pair(thoreau::TileCoordinate(mid_x - 20 * delta_max, mid_y - delta_max), 0);
		Vertex p2 = std::make_pair(thoreau::TileCoordinate(mid_x, mid_y + 20 * delta_max), 0);
		Vertex p3 = std::make_pair(thoreau::TileCoordinate(mid_x + 20 * delta_max, mid_y - delta_max), 0);

		add_triangle(triangles, p1, p2, p3);
		super_triangle = triangles.front();
	}

	for (auto& i: nodes)
	{
		std::vector<Triangle> bad_triangles;
		std::vector<Edge> polygon;

		auto p = std::make_pair(i.second.anchor, i.first);
		for (auto& triangle: triangles)
		{
			if (triangle.contains(p))
			{
				bad_triangles.push_back(triangle);
				polygon.emplace_back(triangle.a, triangle.b);
				polygon.emplace_back(triangle.b, triangle.c);
				polygon.emplace_back(triangle.c, triangle.a);
			}
		}

		for (auto triangle = triangles.begin(); triangle != triangles.end(); /* Nop. */)
		{
			bool erase = false;
			for (auto& bad_triangle: bad_triangles)
			{
				bool has_a = false, has_b = false, has_c = false;
				if (bad_triangle.a == triangle->a ||
					bad_triangle.a == triangle->b ||
					bad_triangle.a == triangle->c)
				{
					has_a = true;
				}

				if (bad_triangle.b == triangle->a ||
					bad_triangle.b == triangle->b ||
					bad_triangle.b == triangle->c)
				{
					has_b = true;
				}
				
				if (bad_triangle.c == triangle->a ||
					bad_triangle.c == triangle->b ||
					bad_triangle.c == triangle->c)
				{
					has_c = true;
				}

				if (has_a && has_b && has_c)
				{
					erase = true;
					break;
				}
			}

			if (erase)
			{
				triangle = triangles.erase(triangle);
			}
			else
			{
				++triangle;
			}
		}

		std::vector<Edge> bad_edges;
		for (auto s = polygon.begin(); s != polygon.end(); ++s)
		{
			for (auto t = polygon.begin(); t != polygon.end(); ++t)
			{
				if (s == t)
				{
					continue;
				}

				if ((s->first == t->first && s->second == t->second) ||
					(s->second == t->first && s->first == t->second))
				{
					bad_edges.push_back(*s);
					bad_edges.push_back(*t);
				}
			}
		}

		for (auto edge = polygon.begin(); edge != polygon.end(); /* Nop. */)
		{
			bool erase = false;
			for (auto& bad_edge: bad_edges)
			{
				if ((edge->first == bad_edge.first && edge->second == bad_edge.second) ||
					(edge->second == bad_edge.first && edge->first == bad_edge.second))
				{
					erase = true;
					break;
				}
			}

			if (erase)
			{
				edge = polygon.erase(edge);
			}
			else
			{
				++edge;
			}
		}

		for (auto& edge: polygon)
		{
			add_triangle(triangles, edge.first, edge.second, p);
		}
	}

	for (auto triangle = triangles.begin(); triangle != triangles.end(); /* Nop. */)
	{
		if (triangle->a == super_triangle.a ||
			triangle->a == super_triangle.b ||
			triangle->a == super_triangle.c)
		{
			triangle = triangles.erase(triangle);
		}
		else if (triangle->b == super_triangle.a ||
			triangle->b == super_triangle.b ||
			triangle->b == super_triangle.c)
		{
			triangle = triangles.erase(triangle);
		}
		else if (triangle->c == super_triangle.a ||
			triangle->c == super_triangle.b ||
			triangle->c == super_triangle.c)
		{
			triangle = triangles.erase(triangle);
		}
		else
		{
			++triangle;
		}
	}

	auto connect = [&nodes, &atlas, &map](Vertex a, Vertex b)
	{
		auto s = nodes.find(a.second)->second;
		auto t = nodes.find(b.second)->second;
		thoreau::PathFinder path(map);
		if (path.try_find_path(s.anchor, t.anchor))
		{
			atlas.connect(mapp::TravelType::walk, s, t, (float)path.length());
		}
	};

	for (auto& triangle: triangles)
	{
		connect(triangle.a, triangle.b);
		connect(triangle.b, triangle.c);
		connect(triangle.c, triangle.a);
	}

#if 0

	for (auto& child: detached_nodes)
	{
		auto parent = atlas.parent(child);

		thoreau::PathFinder parent_to_child(map);
		if (parent_to_child.try_find_path(parent.anchor, child.anchor))
		{
			atlas.connect(mapp::TravelType::walk, parent, child, parent_to_child.length());
		}

		thoreau::PathFinder child_to_parent(map);
		if (child_to_parent.try_find_path(child.anchor, parent.anchor))
		{
			atlas.connect(mapp::TravelType::walk, child, parent, child_to_parent.length());
		}
	}
#endif
}

void plato::connect_walkable_children(
	twoflower::Brochure& brochure,
	mapp::Atlas& atlas,
	const mapp::Location& parent,
	const thoreau::Map& map,
	bool recursive)
{
	//connect_nearest_neighbors(brochure, atlas, map);
	//return;

	auto children = atlas.children(parent);
	std::vector<mapp::Location> waypoints;
	for (auto& child: children)
	{
		if (brochure.has_tag(child.resource, "mapp-unwalkable"))
		{
			continue;
		}

		auto grandchildren = atlas.children(child);
		for (auto& grandchild: grandchildren)
		{
			if (grandchild.shape.get_num_points() == 1)
			{
				waypoints.push_back(grandchild);
			}
		}

		for (auto& neighbor: children)
		{
			if (neighbor.resource.get_id() == child.resource.get_id())
			{
				continue;
			}

			thoreau::PathFinder path(map);
			if (!path.try_find_path(neighbor.anchor, child.anchor))
			{
				continue;
			}

			float cost = path.length();
			atlas.disconnect(neighbor, child, mapp::TravelType::walk);
			atlas.connect(mapp::TravelType::walk, neighbor, child, cost);
		}

		thoreau::PathFinder path(map);
		if (path.try_find_path(parent.anchor, child.anchor))
		{
			atlas.disconnect(child, parent, mapp::TravelType::walk);
			atlas.connect(mapp::TravelType::walk, child, parent, path.length());
			atlas.disconnect(parent, child, mapp::TravelType::walk);
			atlas.connect(mapp::TravelType::walk, parent, child, path.length());
		}

		if (recursive)
		{
			connect_walkable_children(brochure, atlas, child, map, true);
		}
	}

	for (auto i = waypoints.begin(); i != waypoints.end(); ++i)
	{
		for (auto j = waypoints.begin(); j != waypoints.end(); ++j)
		{
			if (i == j)
			{
				continue;
			}

			thoreau::PathFinder path(map);
			if (!path.try_find_path(i->anchor, j->anchor))
			{
				continue;
			}

			float cost = path.length();
			atlas.disconnect(*i, *j, mapp::TravelType::walk);
			atlas.connect(mapp::TravelType::walk, *i, *j, cost);
		}
	}
}
