// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvlc/collision/scene.hpp"
#include "kvlc/collision/query.hpp"
#include "kvlc/geometry/ray.hpp"

kvlc::Scene::Scene()
{
	dInitODE();

	this->world = dWorldCreate();
	dWorldSetAutoDisableFlag(this->world, false);

	set_num_threads(1);
}

kvlc::Scene::~Scene()
{
	clear();
}

kvlc::Scene::Tag kvlc::Scene::add(const Mesh& mesh)
{
	assert(mesh.get_num_points() >= 3);
	assert(mesh.get_num_triangles() >= 1);

	auto tag = ++current_tag;
	this->meshes[tag] = mesh;

	auto& pending_mesh = this->meshes[tag];
	auto pending_result = this->thread_pool.push(
		[tag, &pending_mesh](int)
		{
			return MeshInstance::build(tag, pending_mesh);
		});
	this->pending_meshes.emplace(tag, std::move(pending_result));
	this->is_dirty = true;

	return tag;
}

void kvlc::Scene::remove(Tag tag)
{
	this->pending_removal.push_back(tag);
	this->is_dirty = true;
}

void kvlc::Scene::clear()
{
	update_space();

	for (auto& mesh_instance: this->mesh_instances)
	{
		dGeomDestroy(mesh_instance.second.geometry);
		dGeomTriMeshDataDestroy(mesh_instance.second.mesh);
	}
	this->mesh_instances.clear();
	this->meshes.clear();

	if (this->space != 0)
	{
		dSpaceDestroy(this->space);
	}
	this->space = 0;
	this->is_dirty = false;

	this->current_tag = 0;
}

bool kvlc::Scene::has(Tag tag) const
{
	return this->meshes.count(tag) != 0;
}

const kvlc::Mesh& kvlc::Scene::get(Tag tag) const
{
	assert(has(tag));
	return this->meshes.find(tag)->second;
}

void kvlc::Scene::update()
{
	update_space();
}

int kvlc::Scene::get_num_threads() const
{
	return this->thread_pool.size();
}

void kvlc::Scene::set_num_threads(int count)
{
	this->thread_pool.resize(count);
}

bool kvlc::Scene::test(const glm::vec3& center, float radius, Query& result) const
{
	update_space();

	result = Query();
	if (this->meshes.empty())
	{
		return false;
	}

	dGeomID geometry = dCreateSphere(this->space, radius);
	dBodyID body = dBodyCreate(this->world);
	dGeomSetBody(geometry, body);
	dBodySetPosition(body, center.x, center.y, center.z);

	dSpaceCollide2(
		(dGeomID)this->space, geometry,
		&result, &perform_space_collision);

	dGeomDestroy(geometry);
	dBodyDestroy(body);

	return !result.empty();
}

bool kvlc::Scene::test(const Bounds& bounds, Query& result) const
{
	update_space();

	result = Query();
	if (this->meshes.empty())
	{
		return false;
	}

	auto size = bounds.compute_size();
	auto center = bounds.compute_center();

	dGeomID geometry = dCreateBox(this->space, size.x, size.y, size.z);
	dBodyID body = dBodyCreate(this->world);
	dGeomSetBody(geometry, body);
	dBodySetPosition(body, center.x, center.y, center.z);

	dSpaceCollide2(
		(dGeomID)this->space, geometry,
		&result, &perform_space_collision);

	dGeomDestroy(geometry);
	dBodyDestroy(body);

	return !result.empty();
}

bool kvlc::Scene::test(const Ray& ray, float length, Query& result) const
{
	update_space();

	result = Query();
	if (this->meshes.empty())
	{
		return false;
	}

	dGeomID geometry = dCreateRay(space, length);
	dGeomRaySet(geometry,
		ray.position.x, ray.position.y, ray.position.z,
		ray.direction.x, ray.direction.y, ray.direction.z);
	dGeomRaySetParams(geometry, false, false);
	dGeomRaySetClosestHit(geometry, true);

	dSpaceCollide2(
		(dGeomID)this->space, geometry,
		&result, &perform_space_collision);

	dGeomDestroy(geometry);

	return !result.empty();
}

kvlc::Bounds kvlc::Scene::compute_bounds() const
{
	if (this->mesh_instances.empty())
	{
		return kvlc::Bounds();
	}

	kvlc::Bounds result(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	for (auto& mesh_instance: this->mesh_instances)
	{
		result.expand(mesh_instance.second.bounds);
	}

	return result;
}

void kvlc::Scene::update_space() const
{
	if (!this->is_dirty)
	{
		return;
	}

	if (this->space != 0)
	{
		for (auto& mesh_instance: this->mesh_instances)
		{
			if (dSpaceQuery(this->space, mesh_instance.second.geometry))
			{
				dSpaceRemove(this->space, mesh_instance.second.geometry);
			}
		}

		dSpaceDestroy(this->space);
	}

	for (auto& pending: this->pending_meshes)
	{
		pending.second.wait();
		auto instance = pending.second.get();
		this->mesh_instances.emplace(pending.first, instance);
	}
	this->pending_meshes.clear();

	for (auto& tag: this->pending_removal)
	{
		auto& instance = this->mesh_instances[tag];
		dGeomDestroy(instance.geometry);
		dGeomTriMeshDataDestroy(instance.mesh);

		this->mesh_instances.erase(tag);
		this->meshes.erase(tag);
	}
	this->pending_removal.clear();

	if (this->meshes.empty())
	{
		assert(this->mesh_instances.empty());
		this->is_dirty = false;
		return;
	}

	auto bounds = compute_bounds();
	auto bounds_center = bounds.compute_center();
	auto bounds_half_size = bounds.compute_size() / 2.0f;

	dVector3 center;
	center[0] = bounds_center.x;
	center[1] = bounds_center.y;
	center[2] = bounds_center.z;

	dVector3 extents;
	extents[0] = bounds_half_size.x;
	extents[1] = bounds_half_size.y;
	extents[2] = bounds_half_size.z;

	this->space = dQuadTreeSpaceCreate(0, center, extents, 4);
	for (auto& mesh_instance: this->mesh_instances)
	{
		dSpaceAdd(this->space, mesh_instance.second.geometry);
	}

	this->is_dirty = false;
}

void kvlc::Scene::perform_space_collision(void* data, dGeomID a, dGeomID b)
{
	Query* query = (Query*)data;

	dContact contact;
	int num_contacts = dCollide(a, b, 1, &contact.geom, sizeof(dContact));
	if (num_contacts == 1)
	{
		dGeomID mesh;
		if (dGeomGetClass(a) == dTriMeshClass)
		{
			assert(dGeomGetClass(b) != dTriMeshClass);
			mesh = a;
		}
		else if (dGeomGetClass(b) == dTriMeshClass)
		{
			mesh = b;
		}
		else
		{
			assert(false);
			return;
		}

		Scene::Tag tag = (std::size_t)dGeomGetData(mesh);
		glm::vec3 point(contact.geom.pos[0], contact.geom.pos[1], contact.geom.pos[2]);

		query->add_result(tag, point);
	}
}

kvlc::Scene::MeshInstance kvlc::Scene::MeshInstance::build(int tag, const Mesh& mesh)
{
	MeshInstance instance;
	instance.mesh = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSimple(
		instance.mesh,
		(const dReal*)mesh.get_points(), mesh.get_num_points(),
		mesh.get_triangles(), mesh.get_num_triangles() * 3);
	instance.geometry = dCreateTriMesh(0, instance.mesh, nullptr, nullptr, nullptr);
	dGeomSetData(instance.geometry, (void*)((std::intptr_t)tag));

	instance.bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	auto points = mesh.get_points();
	for (std::size_t i = 0; i < mesh.get_num_points(); ++i)
	{
		instance.bounds.expand(glm::vec3(points[i]));
	}

	return instance;
}
