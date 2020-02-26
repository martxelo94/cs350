/**
* @file mesh.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define mesh
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef MESH_H
#define MESH_H


/*
	Vertex Attributes:	position (0), uvs/texture_coords (1), color (2)
*/
#define VBO_SIZE 1		//positions

/*
	Choose triangle index format (switch lines)
#define MESH_IDX_TYPE GL_UNSIGNED_SHORT
*/
#define MESH_IDX_TYPE GL_UNSIGNED_INT
#if MESH_IDX_TYPE == GL_UNSIGNED_INT
using tri_idx = int;
#elif MESH_IDX_TYPE == GL_UNSIGNED_SHORT
using tri_idx = u16;
#elif MESH_IDX_TYPE == GL_UNSIGNED_BYTE
using tri_idx = u8;
#endif

struct MeshData
{
	//define arrays for each attribute
	std::vector<vec3> positions;
	//std::vector<vec2> uvs;
	//std::vector<Color> colors;
	std::vector<tri_idx> indices;

	MeshData() = default;

	inline void clear() {
		positions.clear();
		//uvs.clear();
		//colors.clear();
		indices.clear();
	}
};

struct MeshBuffers
{
	GLuint vao = 0;
	GLuint vbo[VBO_SIZE] = { 0 };
	GLuint ibo = 0;
	tri_idx index_count = 0;

	MeshBuffers() {}
	MeshBuffers(MeshBuffers && rhs);
	MeshBuffers& operator=(MeshBuffers &&rhs);
	MeshBuffers(const MeshBuffers &) = delete;
	~MeshBuffers();

	// free GPU memory
	inline void clear() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(VBO_SIZE, vbo);
		glDeleteBuffers(1, &ibo);
		index_count = 0;
	}

	inline void draw(GLuint mode = GL_TRIANGLES) const { glDrawElements(mode, index_count, MESH_IDX_TYPE, 0); }
};

void create_buffers(MeshBuffers* mb,
	vec3 const* positions, const tri_idx vertex_count,
	tri_idx const* indices, const tri_idx index_count);
void create_buffers(MeshBuffers* mb, const MeshData *md);

bool load_mesh_obj(const char* filepath, MeshBuffers* mb, MeshData* md = nullptr);
void create_aabb_mesh(const AABB& ab, MeshBuffers* mb, MeshData* md = nullptr);
void create_sphere_mesh(const Sphere& sph, MeshBuffers* mb, MeshData* md = nullptr);

#endif