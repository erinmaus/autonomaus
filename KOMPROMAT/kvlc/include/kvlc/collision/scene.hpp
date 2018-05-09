// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVLC_COLLISION_SCENE_HPP
#define KVLC_COLLISION_SCENE_HPP

#include <map>
#include <set>
#include <ode/ode.h>
#include "kommon/deps/ctpl/ctpl.h"
#include "kvlc/geometry/mesh.hpp"
#include "kvlc/geometry/bounds.hpp"

namespace kvlc
{
	class Ray;
	class Query;

	class Scene
	{
	public:
		const static int TAG_NONE = 0;
		typedef int Tag;

		Scene();
		~Scene();

		Tag add(const Mesh& mesh);
		void remove(Tag tag);
		void clear();

		void update();
		int get_num_threads() const;
		void set_num_threads(int count);

		bool has(Tag tag) const;
		const Mesh& get(Tag tag) const;

		bool test(const Bounds& bounds, Query& result) const;
		bool test(const glm::vec3& position, float radius, Query& result) const;
		bool test(const Ray& ray, float length, Query& result) const;

		kvlc::Bounds compute_bounds() const;

	private:
		typedef std::map<Tag, Mesh> MeshCollection;
		mutable MeshCollection meshes;

		struct MeshInstance
		{
			dTriMeshDataID mesh;
			dGeomID geometry;
			kvlc::Bounds bounds;

			static MeshInstance build(int tag, const Mesh& mesh);
		};
		mutable std::map<Tag, std::future<MeshInstance>> pending_meshes;
		mutable std::vector<Tag> pending_removal;
		mutable std::map<Tag, MeshInstance> mesh_instances;

		mutable ctpl::thread_pool thread_pool;
		mutable dSpaceID space = 0;
		dWorldID world;

		mutable bool is_dirty = false;
		Tag current_tag = 0;

		void update_space() const;
		static void perform_space_collision(void* data, dGeomID o1, dGeomID o2);

	public:
		typedef MeshCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
