/**
* @file bounding_volume.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement bounding volume creators
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include "pch.h"

namespace {
	/**
*
* @param sph
* @param pt
*/
	void sphere_of_sphere_and_pt(Sphere &sph, const vec3 &pt)
	{
		vec3 diff = pt - sph.center;
		float dist2 = glm::dot(diff, diff);
		// only update sph if point p is outside it
		if (dist2 > sph.radius * sph.radius) {
			float dist = sqrt(dist2);
			float new_radius = (sph.radius + dist) * 0.5f;
			float t = (new_radius - sph.radius) / dist;
			sph.radius = new_radius;
			sph.center += diff * t;
		}
	}
	/**
*
* @param _aabb
*/
	void increment_epsilon(AABB &_aabb, float epsilon_mul)
	{
		_aabb.min_point -= vec3{ cEpsilon * epsilon_mul};
		_aabb.max_point += vec3{ cEpsilon * epsilon_mul};
	}
	/**
*
* @param sph
*/
	void increment_epsilon(Sphere &sph, float epsilon_mul)
	{
		sph.radius += cEpsilon * epsilon_mul;
	}
	// Give the user the option to fit th BV perfectly (this makes some tests to fail)
#define BV_TIGHT 0
#if BV_TIGHT
#define INC_EPSILON(bv)
#else
#define INC_EPSILON(bv, epsilon_mul) increment_epsilon(bv, epsilon_mul);
#endif
}
/**
*
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_points(const vec3* points, const size_t point_count
)
{
	vec3 min_p = vec3{ FLT_MAX }, max_p = vec3{ -FLT_MAX };

	for (int i = 0; i < point_count; ++i) {
		const vec3 &p = points[i];
		// x
		if (p.x > max_p.x) max_p.x = p.x;
		if (p.x < min_p.x) min_p.x = p.x;
		// y
		if (p.y > max_p.y) max_p.y = p.y;
		if (p.y < min_p.y) min_p.y = p.y;
		// z
		if (p.z > max_p.z) max_p.z = p.z;
		if (p.z < min_p.z) min_p.z = p.z;
	}
	AABB ab = AABB{ min_p, max_p };

	INC_EPSILON(ab, 1);
	
	return ab;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_points(const mat4& transform, const vec3* points, const size_t point_count
)
{
	vec3 min_p = vec3{ FLT_MAX }, max_p = vec3{ -FLT_MAX };

	for (int i = 0; i < point_count; ++i) {
		const vec3 p = transform * vec4{ points[i], 1.f };
		// x
		if (p.x > max_p.x) max_p.x = p.x;
		if (p.x < min_p.x) min_p.x = p.x;
		// y
		if (p.y > max_p.y) max_p.y = p.y;
		if (p.y < min_p.y) min_p.y = p.y;
		// z
		if (p.z > max_p.z) max_p.z = p.z;
		if (p.z < min_p.z) min_p.z = p.z;
	}
	AABB ab = AABB{ min_p, max_p };

	INC_EPSILON(ab, 1);

	return ab;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
AABB compute_bv_aabb_from_obb(const mat4& transform, const vec3* points, const size_t point_count)
{
	AABB ab = compute_bv_aabb_from_points(points, point_count);
	// transform aabb
	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	std::array<vec3, 8> p = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	for (int i = 0; i < 8; ++i) {
		p[i] = transform * vec4{ p[i], 1.f };
	}
	return compute_bv_aabb_from_points(mat4(1.f), p.data(), p.size());
}
/**
*
* @param transform
* @param obb
* @return
*/
AABB compute_bv_aabb_from_obb(const mat4& transform, const AABB& obb) 
{
	// transform aabb
	const vec3& MAX = obb.max_point;
	const vec3& MIN = obb.min_point;
	std::array<vec3, 8> p = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };
	for (int i = 0; i < 8; ++i) {
		p[i] = transform * vec4{ p[i], 1.f };
	}
	return compute_bv_aabb_from_points(mat4(1.f), p.data(), p.size());
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_centroid(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// compute midpoint
	vec3 midpoint = vec3{ 0.f };
	for (const vec3& pos : p)
		midpoint += pos;
	midpoint /= p.size();
	// find fartest vertex
	float max_dist2 = FLT_MIN;
	for (const vec3& pos : p) {
		float dist2 = glm::distance2(pos, midpoint);
		if (dist2 > max_dist2)
			max_dist2 = dist2;
	}
	Sphere sph = Sphere{ midpoint, sqrt(max_dist2) };

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_ritter(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// 1st pass: Get sphere encompassing two approximately most distant points

	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// most separated points on aabb
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (int i = 0; i < point_count; ++i) {
		if (p[i].x < p[minx].x) minx = i;
		if (p[i].x > p[maxx].x) maxx = i;
		if (p[i].y < p[miny].y) miny = i;
		if (p[i].y > p[maxy].y) maxy = i;
		if (p[i].z < p[minz].z) minz = i;
		if (p[i].z > p[maxz].z) maxz = i;
	}
	float dist2x = glm::distance2(p[maxx], p[minx]);
	float dist2y = glm::distance2(p[maxy], p[miny]);
	float dist2z = glm::distance2(p[maxz], p[minz]);
	// pick the pair (min, max) of points most distant
	int minp = minx;
	int maxp = maxx;
	if (dist2y > dist2x && dist2y > dist2z) {
		maxp = maxy; minp = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) {
		maxp = maxz; minp = minz;
	}
	// set up sphere to just encompass these two points
	Sphere sph;
	sph.center = (p[minp] + p[maxp]) * 0.5f;
	sph.radius = glm::distance(p[maxp], sph.center);

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// compute squared distance between point and sphere center
		sphere_of_sphere_and_pt(sph, p[i]);
	}

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_iterative(const mat4 &transform, const vec3* points, const size_t point_count)
{
	const int NUM_ITER = 20;

	// Ritter Sphere

	// 1st pass: Get sphere encompassing two approximately most distant points

	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// most separated points on aabb
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (int i = 0; i < point_count; ++i) {
		if (p[i].x < p[minx].x) minx = i;
		if (p[i].x > p[maxx].x) maxx = i;
		if (p[i].y < p[miny].y) miny = i;
		if (p[i].y > p[maxy].y) maxy = i;
		if (p[i].z < p[minz].z) minz = i;
		if (p[i].z > p[maxz].z) maxz = i;
	}
	float dist2x = glm::distance2(p[maxx], p[minx]);
	float dist2y = glm::distance2(p[maxy], p[miny]);
	float dist2z = glm::distance2(p[maxz], p[minz]);
	// pick the pair (min, max) of points most distant
	int minp = minx;
	int maxp = maxx;
	if (dist2y > dist2x && dist2y > dist2z) {
		maxp = maxy; minp = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) {
		maxp = maxz; minp = minz;
	}
	// set up sphere to just encompass these two points
	Sphere sph;
	sph.center = (p[minp] + p[maxp]) * 0.5f;
	sph.radius = glm::distance(p[maxp], sph.center);

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// only update sph if point p is outside it
		sphere_of_sphere_and_pt(sph, p[i]);
	}
	Sphere sph_tmp = sph;

	for (int j = 0; j < NUM_ITER; ++j) {
		// shrink sphere somewhat to make it an underestimate (not bound) // THIS FAILED ON TESTS!!!
		sph_tmp.radius *= 1.0f;
		// make sphere bound data again
		for (int i = 0; i < point_count - 1; ++i) {
			int random_idx = glm::linearRand(i + 1, (int)point_count - 1);
			std::swap(p[i], p[random_idx]);
			// compute squared distance between point and sphere center
			sphere_of_sphere_and_pt(sph_tmp, p[i]);
		}
		// update sph whenever a tighter sphere is found
		if (sph_tmp.radius < sph.radius)
			sph = sph_tmp;
	}

	INC_EPSILON(sph, 10);

	return sph;
}
/**
*
* @param transform
* @param points
* @param point_count
* @return
*/
Sphere compute_bv_sphere_pca(const mat4 &transform, const vec3* points, const size_t point_count)
{
	// compute points in world space
	std::vector<vec3> p(points, points + point_count);
	for (vec3& pos : p)
		pos = transform * vec4{ pos, 1.f };
	// 1st pass: find pair most separated
	int minp = 0, maxp = 0;
	float max_dist2 = FLT_MIN;
	for (int i = 0; i < point_count - 1; ++i) {
		for (int j = i + 1; j < point_count; ++j) {
			float dist2 = glm::distance2(p[i], p[j]);
			if (dist2 > max_dist2) {
				max_dist2 = dist2;
				minp = i;
				maxp = j;
			}
		}
	}
	vec3 diff = p[maxp] - p[minp];
	Sphere sph = { p[minp] + diff * 0.5f, sqrt(max_dist2) * 0.5f };

	// 2nd pass: Grow sphere to include all points
	for (int i = 0; i < point_count; ++i) {
		// only update sph if point p is outside it
		sphere_of_sphere_and_pt(sph, p[i]);
	}

	INC_EPSILON(sph, 20);

	return sph;
}

/**
*
* @param obj
* @return
*/
bool bounding_volume_hierarchy::add_object(Object& obj) {
	// if room empty, create first node
	if (m_root == nullptr) {
		m_root = new node;
		m_root->objects.push_back(&obj);
		m_root->bounding_volume = obj.get_aabb();
		return true;
	}
	// use stack to traverse the tree the best way
	std::stack<node*> nodes;
	nodes.push(m_root);

	while (!nodes.empty()) {
		auto n = nodes.top();
		nodes.pop();

		// find best node by delta surface
		// if leaf node, found it
		bool is_leaf = n->children[0] == nullptr && n->children[1] == nullptr;
		if (is_leaf) {
			//check that not already here
			assert(n->objects.size() == 1);
			if (n->objects[0] == &obj)
				return false;
			// create new nodes
			node* new_child0 = new node;
			node* new_child1 = new node;
			//link
			new_child0->parent = new_child1->parent = n;
			n->children = { new_child0, new_child1 };
			//move objects
			new_child0->objects = std::move(n->objects);
			new_child1->objects.push_back(&obj);
			// set aabb
			const AABB &bv_node = new_child0->bounding_volume = n->bounding_volume;
			const AABB &bv_obj = new_child1->bounding_volume = obj.get_aabb();
			// compute new aabb
			n->bounding_volume = AABB{ glm::min(bv_obj.min_point, bv_node.min_point), glm::max(bv_obj.max_point, bv_node.max_point) };

			// also dont forget to rebuild up!
			node* grandparent = n->parent;
			while (grandparent) {
				const AABB &bv0 = grandparent->children[0]->bounding_volume;
				const AABB &bv1 = grandparent->children[1]->bounding_volume;
				grandparent->bounding_volume = AABB{ glm::min(bv0.min_point, bv1.min_point), glm::max(bv0.max_point, bv1.max_point) };

				grandparent = grandparent->parent;
			}
			return true;
		}
		else   // keep iterating... but choose best surface!
		//if (n->children[0] != nullptr) 
		{
			assert(n->children[0]);	// must have children
			assert(n->children[1]);	// must have sibling
			const AABB &bv_obj = obj.get_aabb();
			const AABB &bv0 = n->children[0]->bounding_volume;
			const AABB &bv1 = n->children[1]->bounding_volume;
			// compare addition of aabb bounding volumes
			AABB bv_obj0 = AABB{ glm::min(bv0.min_point, bv_obj.min_point), glm::max(bv0.max_point, bv_obj.max_point) };
			AABB bv_obj1 = AABB{ glm::min(bv_obj.min_point, bv1.min_point), glm::max(bv_obj.max_point, bv1.max_point) };
			// delta surface
			float dSurface = bv_obj0.surface_area() - bv_obj1.surface_area();
			if (dSurface < 0.f) {
				// child 1 is bigger, go to child 0
				nodes.push(n->children[0]);
			}
			else {
				// child 0 is bigger
				nodes.push(n->children[1]);
			}
		}

	}
	return false;
}

/**
*
* @param obj
* @param current
* @return
*/
bool bounding_volume_hierarchy::remove_object(const Object& obj, node* current) {

	// base case: return if node null
	if (current == nullptr)
		return false;

	assert(current != nullptr);
	auto & objs = current->objects;

	std::pair<node*, node*> best_order = select_branch_by_position(obj, *current);
	// leaf check
	bool is_leaf = best_order.first == nullptr && best_order.second == nullptr;
	if (!is_leaf) {
		assert(objs.empty());	// intermediate always empty
		return remove_object(obj, best_order.first) || remove_object(obj, best_order.second);
	}
	assert(objs.size() <= 1);	// one object per node
	// object may be here...
	auto it = std::find(objs.begin(), objs.end(), &obj);
	if (it == objs.end())
		return false;	// not here

	std::swap(*it, objs[objs.size() - 1]);
	objs.pop_back();

	// get grandparent; if dont, pass last single object to root node, its parent
	if (current == m_root) {
		assert(current->parent == nullptr);
		delete m_root;
		m_root = nullptr;
		return true;
	}
	node* parent = current->parent;
	// get sibling before updating grandpa
	node* sibling = parent->children[0] == current ? parent->children[1] : parent->children[0];
	// copy sibling to parent (no need to know grandpa)
	parent->bounding_volume = sibling->bounding_volume;
	parent->objects = std::move(sibling->objects);
	// relink children with parent (if any)
	if (sibling->children[0]) {
		sibling->children[0]->parent = sibling->children[1]->parent = parent;
	}
	parent->children = std::move(sibling->children);	// copy since sibling might be intermediate node
	sibling->children = { nullptr, nullptr };			// make sure not deleting their subtree
	// delete nodes
	delete current;
	delete sibling;
	// also dont forget to rebuild up!
	parent = parent->parent;
	while (parent) {
		const AABB &bv0 = parent->children[0]->bounding_volume;
		const AABB &bv1 = parent->children[1]->bounding_volume;
		parent->bounding_volume = AABB{ glm::min(bv0.min_point, bv1.min_point), glm::max(bv0.max_point, bv1.max_point) };

		parent = parent->parent;
	}

	// done
	return true;
}
/**
*
* @param obj
* @param current
* @return
*/
std::pair<BVH::node*, BVH::node*> bounding_volume_hierarchy::select_branch_by_position(const Object& obj, const node& current )
{
	// if some child null, return null (but be sure the other is null too)
	if (current.children[0] == nullptr) {
		assert(current.children[1] == nullptr);	// sibling null too
		return {nullptr, nullptr};
	}

	// select branch node with aabb closest to obj
	const AABB& obj_bv = obj.get_aabb();
	vec3 obj_center = obj_bv.center();
	intersection_type result = intersection_point_aabb(obj_center, current.children[0]->bounding_volume);
	// if inside, good candidate
	if (result == intersection_type::INSIDE) {
		return { current.children[0], current.children[1] };
	}
	result = intersection_point_aabb(obj_center, current.children[0]->bounding_volume);
	// if inside, good candidate
	if (result == intersection_type::INSIDE) {
		return { current.children[1], current.children[0] };
	}
#if 0	// select method of choice
	// select closest center
	vec3 center0 = current.children[0]->bounding_volume.center();
	vec3 center1 = current.children[1]->bounding_volume.center();
	if (glm::distance2(obj_center, center0) < glm::distance2(obj_center, center1)) 
		return { current.children[0], current.children[1] };
	else
		return {current.children[1], current.children[0]};
#else
	// select lesser surface area
	AABB ab0 = AABB{ glm::min(obj_bv.min_point, current.children[0]->bounding_volume.min_point),
					 glm::max(obj_bv.max_point, current.children[0]->bounding_volume.max_point) };
	AABB ab1 = AABB{ glm::min(obj_bv.min_point, current.children[1]->bounding_volume.min_point),
					 glm::max(obj_bv.max_point, current.children[1]->bounding_volume.max_point) };
	if(ab0.surface_area() < ab1.surface_area())
		return { current.children[0], current.children[1] };
		return { current.children[1], current.children[0] };
#endif
#if 0
	assert(0 && "Could happen?");	// NO
	return {};
#endif
}
/**
*
* @param objects
* @return
*/
AABB compute_aabb(const std::vector<Object*>& objects) {
	//assert(objects.size() > 1);	// allow single object
	AABB result = objects[0]->get_aabb();
	for (int i = 1; i < objects.size(); ++i) {
		const AABB& ab = objects[i]->get_aabb();
		result.min_point = glm::min(result.min_point, ab.min_point);
		result.max_point = glm::max(result.max_point, ab.max_point);
	}
	return result;
}
/**
*
* @param objects
* @param bv_all
* @return
*/
std::pair<std::vector<Object*>, std::vector<Object*>> partition(const std::vector<Object*> & objects, const AABB& bv_all){
	assert(objects.size() > 1);	// must!

	// get biggest axis for partition plane
	vec3 dir = bv_all.max_point - bv_all.min_point;
	vec3 normal = vec3{1, 0, 0};
	float biggest_axis = dir.x;
	if (dir.y > biggest_axis) {
		normal = vec3{0, 1, 0};
		biggest_axis = dir.y;
	}
	if (dir.z > biggest_axis) {
		normal = vec3{ 0, 0, 1 };
		biggest_axis = dir.z;
	}
	// create plane
	Plane pl(normal, bv_all.center());
	// assign each object {INSIDE first, OUTSIDE second}
	std::pair<std::vector<Object*>, std::vector<Object*>> result;
	for (int i = (int)objects.size() - 1; i >= 0; --i) {
		Object* o = objects[i];
		// get bv center
		const AABB ab = o->get_aabb();
		vec3 pos = ab.center();
		// select side
		intersection_type intersection_result = intersection_point_plane(pos, pl);
		if (intersection_result == INSIDE) {
			result.first.push_back(o);
		}
		else
			result.second.push_back(o);
	}
	// balance if one side without objects
	if (result.first.empty()) {
		// randomly?
		result.first.push_back(result.second.back());
		result.second.pop_back();
	}
	else if (result.second.empty()) {
		result.second.push_back(result.first.back());
		result.first.pop_back();
	}

	return result;
}
/**
*
* @param objects
* @param n
* @return
*/
bounding_volume_hierarchy::node* bounding_volume_hierarchy::build_top_down(std::vector <Object*>& objects, node* n)
{
	recursion_counter++;
	/*
		// compute bounding volume for all
		bv_all = compute_bv(objects);
		//create the node
		node = create_node(objects, m_root);
		// are we finished?
		if end_condition(node)
			node.type = leaf
		else
			// Do the partition and recurse
			[left, right] = partition(objects)
			bvh_topdown(left, node)
			bvh_topdown(right, node)

	*/
	// exit if no objects
	if (objects.empty())
		return nullptr;
	assert(!objects.empty());
	assert(n != nullptr);	// parent node must be valid
	// compute common aabb
	node* new_node = new node;
	new_node->bounding_volume = compute_aabb(objects);
	// check if we are finished
	if (objects.size() == 1) {
		new_node->objects = objects;
		return new_node;
	}
	//assign sides
	auto part = partition(objects, new_node->bounding_volume);
	// clear memory from old objects 
	//objects.clear();
	//objects.shrink_to_fit();
	// recursion
	node* child0 = build_top_down(part.first, new_node);
	node* child1 = build_top_down(part.second, new_node);
	// link children
	assert(child0 && child1);	// should?
	new_node->children[0] = child0;
	new_node->children[1] = child1;
	child0->parent = child1->parent = new_node;
	// return new_node to allow link
	return new_node;
}
/**
*
* @param objects
*/
void bounding_volume_hierarchy::build_top_down(const std::vector  <Object* >& objects)
{
	// exit if no objects
	if (objects.empty())
		return;
	assert(m_root == nullptr);	// tree should be cleared
	m_root = new node;
	// compute common aabb
	m_root->bounding_volume = compute_aabb(objects);
	// check if we are finished
	if (objects.size() == 1) {
		m_root->objects = objects;
		return;
	}
	//assign sides
	auto part = partition(objects, m_root->bounding_volume);
	// recursion
	node* child0 = build_top_down(part.first, m_root);
	node* child1 = build_top_down(part.second, m_root);
	// link children
	assert(child0 && child1);	// should?
	m_root->children[0] = child0;
	m_root->children[1] = child1;
	child0->parent = child1->parent = m_root;

	recursion_counter = 0;
}
/**
*
* @param nodes
* @return
*/
std::tuple<BVH::node*, BVH::node*, AABB> find_candidates(const std::vector<BVH::node*> &nodes) {
	float best_surface_area = FLT_MAX;	// smaller
	std::tuple<BVH::node*, BVH::node*, AABB> best_pair = { nullptr, nullptr, AABB{} };
	// find pairs O(n^2)
	for (int i = 0; i < nodes.size() - 1; ++i) {
		for (int j = i + 1; j < nodes.size(); ++j) {
			const AABB& ab0 = nodes[i]->bounding_volume;
			const AABB& ab1 = nodes[j]->bounding_volume;
			AABB ab = AABB{ glm::min(ab0.min_point, ab1.min_point),
				 glm::max(ab0.max_point, ab1.max_point) };

			float surf_area = ab.surface_area();
			if (surf_area < best_surface_area) {
				best_surface_area = surf_area;
				best_pair = { nodes[i], nodes[j], ab};
			}
		}
	}
	return best_pair;
}
/**
*
* @param objects
*/
void bounding_volume_hierarchy::build_bottom_up(const std::vector <Object*>& objects)
{
	assert(m_root == nullptr);	// tree should be cleared
	/*
		nodes = []
		// compute bounding volumes for all
		for obj : objects {
			node = create_node(obj)
			node.compute_bv()
			nodes.add(node)
		}
		while(nodes.size > 1){
			[node_a, node_b] = find_candidates(nodes)
			node = create_node()
			node.children = [node_a, node_b]
			node.compute_bv()
			nodes.erase([node_a, node_b])
		}
		return nodes[0]
	*/
	std::vector<node*> nodes;
	nodes.reserve(objects.size());
	// compute bounding volumes for all and create new nodes
	for (auto obj : objects) {
		node* n = new node;
		n->bounding_volume = obj->get_aabb();
		n->objects.push_back(obj);
		nodes.push_back(n);
	}
	while (nodes.size() > 1) {
		auto best_pair = find_candidates(nodes);
		node* child0 = std::get<0>(best_pair);
		node* child1 = std::get<1>(best_pair);
		node* n = new node;
		//link
		n->children = {child0, child1};
		child0->parent = child1->parent = n;
		//compute bv (already done in pair find
		n->bounding_volume = std::get<2>(best_pair);
		//erase (back swap) nodes before adding new
		auto it = std::find(nodes.begin(), nodes.end(), child0);
		assert(it != nodes.end());
		nodes.erase(it);
		it = std::find(nodes.begin(), nodes.end(), child1);
		assert(it != nodes.end());
		nodes.erase(it);
		//add new node
		nodes.push_back(n);
	}
	// link the last node to the root
	m_root = nodes.back();
}
