/**
* @file graphics.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define graphics
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H


#define WINDOW_W 1080
#define WINDOW_H int(WINDOW_W * (9.f / 16.f))


enum Texture : int { no_texture = -1, t_white, t_black, t_red, t_green, t_blue, t_pattern, texture_count };
enum Shader : int {
	no_shader = -1,
	/*3D, 2D basic*/	sh_raw = 0, sh_color,
	shader_count
};
enum Primitive : int {no_primitive = -1, primitive_cube, primitive_sphere, primitive_count };


struct Graphics
{
	
	SINGLETON(Graphics)

	//load data and initialize systems
	void init();
	//free data and systems (openGL, SDL)
	void free();

	mat3 window_to_ndc() const;
	mat3 ndc_to_window() const;
	bool toggle_fullscreen();
	void resize_window(int size_x, int size_y);

	//shader controll
	void delete_shaders();
	void compile_shaders();

	// debug
	void debug_line(const vec3& start, const vec3& end);
	void draw_debug_lines(const mat4 & m, Color color, float line_width = 1.f);

	//window stuff (dont set)
	GLFWwindow* window = nullptr;
	bool is_fullscreen = false;
	ivec2 window_size = vec2{WINDOW_W, WINDOW_H};
	ivec2 window_pos;

	//shader stuff
	GLuint shader_program[Shader::shader_count] = { 0 };

	//textures
	GLuint tex[Texture::texture_count] = { 0 };

	// primitives
	MeshBuffers primitives[primitive_count];

private:
	std::vector<vec3> m_debug_lines;

};
#define graphics Graphics::instance()
//extern Graphics graphics;	// TODO: make singleton

// outputs error message, returns number of errors
unsigned pop_gl_errors(const char* fun_name);
// LOAD TEXTURES
void load_texture(GLuint *texture_id, const char* filepath, bool invert_y = false, bool generate_mipmaps = false);
// color checker pattern texture
void load_pattern_texture(GLuint * texture_id, const u32 pixel_size = 6);
// Single pixel white texture
void load_color_texture(GLuint * texture_id, Color color);
// get monitor size
ivec2 get_monitor_size();

// draw primitives
void draw_obb(const AABB& obb, const mat4& mvp, Color color);
void draw_aabb(const AABB& ab, const mat4& vp, Color color);
void draw_sphere(const Sphere& sph, const mat4& vp, Color color);



#endif