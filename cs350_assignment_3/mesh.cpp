/**
* @file mesh.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Implement mesh
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include "pch.h"

/**
*
* @param rhs
* @return
*/
MeshBuffers::MeshBuffers(MeshBuffers && rhs)
{
	assert(vao == 0);
	//pass data
	vao = rhs.vao;
	std::memcpy(vbo, rhs.vbo, sizeof(GLuint) * VBO_SIZE);
	ibo = rhs.ibo;
	index_count = rhs.index_count;
	//erase data
	rhs.vao = rhs.ibo = rhs.index_count = 0;
	std::memset(rhs.vbo, 0, sizeof(GLuint) * VBO_SIZE);
}
/**
*
* @param rhs
* @return
*/
MeshBuffers& MeshBuffers::operator=(MeshBuffers &&rhs)
{
	//pass data
	glDeleteVertexArrays(1, &vao);
	vao = rhs.vao;
	glDeleteBuffers(VBO_SIZE, vbo);
	std::memcpy(vbo, rhs.vbo, sizeof(GLuint) * VBO_SIZE);
	glDeleteBuffers(1, &ibo);
	ibo = rhs.ibo;
	index_count = rhs.index_count;
	//erase data
	rhs.vao = rhs.ibo = rhs.index_count = 0;
	std::memset(rhs.vbo, 0, sizeof(GLuint) * VBO_SIZE);
	return *this;
}

MeshBuffers::~MeshBuffers() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(VBO_SIZE, vbo);
	glDeleteBuffers(1, &ibo);
}

/**
*
* @param mb
* @param positions
* @param vertex_count
* @param indices
* @param index_count
*/
void create_buffers(MeshBuffers* mb,
	vec3 const* positions,
	const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count)
{
	assert(mb);
	assert(vertex_count);
	mb->clear();

	glGenVertexArrays(1, &mb->vao);
	glBindVertexArray(mb->vao);
	//generate buffers
	glGenBuffers(VBO_SIZE, &mb->vbo[0]);
	//load positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertex_count, positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//load index buffer
	glGenBuffers(1, &mb->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_idx) * index_count, indices, GL_STATIC_DRAW);
	mb->index_count = index_count;
	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	pop_gl_errors(__FUNCTION__);
}
/**
*
* @param mb
* @param md
*/
void create_buffers(MeshBuffers* mb, const MeshData *md)
{
	assert(md);
	assert(md->positions.size());

	glGenVertexArrays(1, &mb->vao);
	glBindVertexArray(mb->vao);
	//generate buffers
	glGenBuffers(VBO_SIZE, &mb->vbo[0]);
	//load positions buffer (mandatory)
	glBindBuffer(GL_ARRAY_BUFFER, mb->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * md->positions.size(), md->positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//load index buffer (mandatory)
	glGenBuffers(1, &mb->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mb->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri_idx) * md->indices.size(), md->indices.data(), GL_STATIC_DRAW);
	mb->index_count = (tri_idx)md->indices.size();

	//Unbind buffers --- NO!!! (allow binding more buffers after this function call...)
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	pop_gl_errors(__FUNCTION__);
}

namespace  // aux namespace
{
	/**
*
* @param line
* @param position_idx
* @return
*/
	bool parse_face(const std::string& line, int * position_idx)
	{
		
		int matches = sscanf(line.c_str(), "%d %d %d",
			&position_idx[0],
			&position_idx[1],
			&position_idx[2]
		);
		if (matches == 3)
			return true;
		return false;
	}
	struct Vertex {
		vec3 position;

		bool operator <(const Vertex& rhs) const {
			return std::memcmp((const void*)this, (const void*)&rhs, sizeof(Vertex)) > 0;
		}
	};
	/**
*
* @param v
* @param vertexToOutIndex
* @param result
* @return
*/
	bool is_vertex_already_in_use(const Vertex& v, const std::map<Vertex, tri_idx> & vertexToOutIndex, tri_idx & result)
	{
		auto it = vertexToOutIndex.find(v);
		if (it == vertexToOutIndex.end())
			return false;	// does not exists, new vertex

		// vertex used, return index
		result = it->second;
		return true;
	}
	/**
*
* @param positions
* @return
*/
	MeshData index_vbo(const std::vector<vec3>& positions)
	{
		MeshData result;
		std::map<Vertex, tri_idx> vertexToOutIndex;

		//foreach input vertex
		for (tri_idx i = 0; i < positions.size(); i++) {
			Vertex v = { positions[i] };
			tri_idx index;
			bool found = is_vertex_already_in_use(v, vertexToOutIndex, index);
			if (found)	// vertex is already in the vbo, take its index
				result.indices.push_back(index);
			else {
				// new vertex, add to output data
				result.positions.push_back(positions[i]);

				tri_idx new_idx = (tri_idx)result.positions.size() - 1;
				result.indices.push_back(new_idx);
				vertexToOutIndex[v] = new_idx;
			}
		}
		return result;
	}

}
/**
*
* @param filepath
* @param mb
* @param md
* @return
*/
bool load_mesh_obj(const char* filepath, MeshBuffers* mb, MeshData* md)
{
	// obj data read
	std::vector<vec3> tmp_positions;
	// every combination of vertices to give vbo indexer
	std::vector<vec3> in_positions;

	//open file
	std::ifstream file(filepath);
	if (!file.is_open())
		return false;

	//read file
	while (!file.eof()) {
		std::string word;
		//read the firest word of the line
		file >> word;

		// attribute cases
		if (word == "v") {
			// position
			vec3 p;
			file >> word;
			p.x = (float)std::atof(word.c_str());
			file >> word;
			p.y = (float)std::atof(word.c_str());
			file >> word;
			p.z = (float)std::atof(word.c_str());

			tmp_positions.push_back(p);

		}
		else if (word == "f") {
			// face
			int p_idx[3];

			//get whole face line
			std::getline(file, word);

			if (!parse_face(word, p_idx)) {
				return false;
			}

			// decrement indices
			p_idx[0]--;
			p_idx[1]--;
			p_idx[2]--;

			// create new combination of position, normal, uv
			if (p_idx[0] >= 0) {
				in_positions.push_back(tmp_positions[p_idx[0]]);
				in_positions.push_back(tmp_positions[p_idx[1]]);
				in_positions.push_back(tmp_positions[p_idx[2]]);
			}
			else {
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
				in_positions.push_back(vec3{});
			}
		}
		else {
			// probably a commnet
			std::getline(file, word);	// skip all the line
		}
	}
	// create the actual final buffers
	MeshData result = index_vbo(in_positions);

	//outputs
	if (mb)
		create_buffers(mb, &result);
	if (md)
		*md = std::move(result);

	return true;
}
/**
*
* @param ab
* @param mb
* @param md
*/
void create_aabb_mesh(const AABB& ab, MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);

	const vec3& MAX = ab.max_point;
	const vec3& MIN = ab.min_point;
	const vec3 p[8] = { MAX, {MAX.x, MIN.y, MAX.z}, {MAX.x, MIN.y, MIN.z}, {MAX.x, MAX.y, MIN.z},
					MIN, {MIN.x, MAX.y, MIN.z}, {MIN.x, MAX.y, MAX.z}, {MIN.x, MIN.y, MAX.z} };

	const tri_idx idx[36] = {
		// +X
		0, 1, 2, 2, 3, 0,
		// -X
		4, 5, 6, 6, 7, 4,
		// +Y
		0, 3, 5, 5, 6, 0,
		// -Y
		4, 2, 1, 1, 7, 4,
		// +Z
		0, 6, 7, 7, 1, 0,
		// -Z
		3, 2, 4, 4, 5, 3
	};
	if (mb)
		create_buffers(mb, &p[0], sizeof(p) / sizeof(vec3), &idx[0], sizeof(idx) / sizeof(tri_idx));
	if (md) {
		md->positions.assign(p, p + sizeof(p) / sizeof(vec3));
		md->indices.assign(idx, idx + sizeof(idx) / sizeof(tri_idx));
	}
}
/**
*
* @param sph
* @param mb
* @param md
*/
void create_sphere_mesh(const Sphere& sph, MeshBuffers* mb, MeshData* md)
{
	assert(mb || md);	// one of those for sure, why call it otherwise?
	//calculate vtx count and angles
	const u32 rings = 20;
	const u32 slices = 20;
	const tri_idx vertexCount = rings * slices + 2 + rings;	//extra vertices like poles and overlaped ring begin/end
	const float ringStep = (float)M_PI / rings;	//half circle only
	float sliceStep = (2.0f * (float)M_PI / slices);
	float currRing = ringStep, currSlice = 0.0f;
	std::vector<vec3> positions;
	positions.reserve(vertexCount);
	for (u16 r = 0; r < rings; ++r)
	{
		//calculate height
		const float y = cos(currRing) * sph.radius;
		//calculate radius
		const float radius = sin(currRing) * sph.radius;
		for (u16 s = 0; s < slices; ++s)
		{
			//calculate xz
			const float x = radius * cos(currSlice);
			const float z = radius * sin(currSlice);
			//create the vertex position
			const vec3 pos{ x, y, z };
			//add vertex position
			positions.push_back(sph.center + pos);
			currSlice += sliceStep;
		}
		//extra vertex to avoid weird texture mapping
		positions.push_back(sph.center + vec3{  radius, y, 0.0f });

		currSlice = 0.0f;
		currRing += ringStep;
	}
	//add poles
	positions.push_back(sph.center + vec3{ 0,  sph.radius, 0 });		//top
	positions.push_back(sph.center + vec3{ 0, -sph.radius, 0 });	//bot

	//time of triangulating
	const tri_idx ringTriangleCount = slices * 2 * 3;
	const tri_idx triangleCount = (ringTriangleCount * (rings - 1)) + (slices * 3 * 2);
	std::vector<tri_idx> indices;
	indices.reserve(triangleCount);
	//middle triangles
	for (u16 r = 0; r < rings - 1; ++r)
	{
		for (u16 s = 0; s < slices; ++s)
		{
			const u16 curr = (r * (slices + 1)) + s;
			//quad
			indices.push_back(curr);
			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1);

			indices.push_back(curr + 1);
			indices.push_back(curr + slices + 1 + 1);
			indices.push_back(curr + slices + 1);
		}
	}
	//poles
	for (u16 s = 0; s < slices; ++s)
	{
		//top
		indices.push_back(s);
		indices.push_back(vertexCount - 2);
		indices.push_back(s + 1);
		//bot
		indices.push_back((rings - 1)* slices + s);
		indices.push_back(vertexCount - 1);
		indices.push_back((rings - 1)* slices + s + 1);
	}

	//check indices are good
	for (size_t i = 0; i < indices.size(); ++i)
		assert((size_t)indices[i] < positions.size());

	// Data completed, load to GPU
	if (mb)
		create_buffers(mb, positions.data(), (tri_idx)positions.size(), indices.data(), (tri_idx)indices.size());
	if (md) {
		md->positions = std::move(positions);
		md->indices = std::move(indices);
	}

}
