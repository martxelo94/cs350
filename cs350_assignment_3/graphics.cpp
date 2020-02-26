/**
* @file graphics.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define geometrical shapes
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#include "pch.h"

//Graphics graphics;	// Done Singleton

/**
*
* @brief pop all accumulated openGL errors
* @param fun_name
* @return
*/
unsigned pop_gl_errors(const char* fun_name)
{
	unsigned error_count = 0;
	while (GLuint error = glGetError())
	{
		error_count++;
		//MessageBox(NULL, std::to_string(error).c_str(), "GL error code:", MB_ICONERROR | MB_OK);
		//std::cout << "GL error code: " << std::to_string(error) << " \"" << (const char*)glewGetErrorString(error) << "\" " << " at \"" << fun_name << "\"" << std::endl;
		std::cout << "GL error code: " << std::to_string(error) << " at \"" << fun_name << "\"" << std::endl;
	}
	return error_count;
}
namespace
{
	/**
*
* @param program_handle
*/
	void check_gl_program_link_errors(GLuint program_handle)
	{
		GLint status = GL_FALSE;
		glGetProgramiv(program_handle, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			int infoLogLength = 0, maxLength = 0;
			glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &maxLength);
			std::string infoLog(maxLength, 0);
			glGetProgramInfoLog(program_handle, maxLength, &infoLogLength, &infoLog[0]);
			//pop error window
			std::cout << "Program compile error: " << infoLog << std::endl;
			//MessageBox(NULL, infoLog.c_str(), "Program compile error", MB_ICONERROR | MB_OK);
		}
	}
	/**
*
* @param shader_handle
*/
	void check_gl_shader_compile_errors(GLuint shader_handle)
	{
		GLint status = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			int infoLogLength = 0, maxLength = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &maxLength);
			std::string infoLog(maxLength, 0);
			glGetShaderInfoLog(shader_handle, maxLength, &infoLogLength, &infoLog[0]);
			//pop error window
			std::cout << "Shader compile error: " << infoLog << std::endl;
			//MessageBox(NULL, infoLog.c_str(), "Shader compile error", MB_ICONERROR | MB_OK);
		}
	}
	/**
*
* @param shader_type
* @param source
* @return
*/
	GLuint compile_shader(GLuint shader_type, const std::string &source)
	{
		GLuint shader_handle = glCreateShader(shader_type);
		const char* c_source = source.c_str();
		glShaderSource(shader_handle, 1, &c_source, NULL);
		glCompileShader(shader_handle);
		check_gl_shader_compile_errors(shader_handle);
		return shader_handle;
	}
	/**
*
* @param filename
* @return
*/
	std::string get_extension(const char* filename)
	{
		std::string f(filename);
		size_t dot_position = f.find_last_of('.');
		return f.substr(dot_position);
	}
	/**
*
* @brief return file contents in string format
* @param filename
* @return
*/
	std::string read_to_string(const char* filename)
	{
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			file.open(filename);
			std::string res((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return res;
		}
		catch (std::ifstream::failure e) {
			std::cerr << "Exception opening/reading/closing file \"" << filename << "\"\n";
			return "";
		}
	}

		/**
*
* @param filepath
* @return
*/
	GLuint compile_shader(const char* filepath)
	{
		// knows the shader type by the file extension
		std::string extension = get_extension(filepath);
		GLuint shader_type = (GLuint)-1;
		if (extension == ".vert")
			shader_type = GL_VERTEX_SHADER;
		else if (extension == ".frag")
			shader_type = GL_FRAGMENT_SHADER;
		else if (extension == ".geo")
			shader_type = GL_GEOMETRY_SHADER;
		else if (extension == ".tcs")
			shader_type = GL_TESS_CONTROL_SHADER;
		else if (extension == ".tes")
			shader_type = GL_TESS_EVALUATION_SHADER;
		else {
			std::cerr << "\"" << filepath << "\" has not a valid shader extension.\n";
			return 0;	//no shader handle
		}
		//open file
		std::string shader_source = read_to_string(filepath);
		if (shader_source.empty()) {
			std::cerr << "Shader source is empty.\n";
			return 0;
		}
		return compile_shader(shader_type, shader_source);
	}
	/**
*
* @param image_bytes
* @param width
* @param height
* @param bytes_per_pixel
*/
	void invert_image_y(u8 *image_bytes, u32 width, u32 height, u32 bytes_per_pixel)
	{
		u32 half_h = height / 2;
		u32 row_bytes = width * bytes_per_pixel;
		u8 *tmp_row = new u8[row_bytes];

		for (u32 i = 0; i < half_h; i++) {
			u32 idx_top = row_bytes * i;
			u32 idx_bot = row_bytes * (height - i - 1);
			memcpy(tmp_row, image_bytes + idx_top, row_bytes);
			memcpy(image_bytes + idx_top, image_bytes + idx_bot, row_bytes);
			memcpy(image_bytes + idx_bot, tmp_row, row_bytes);
		}

		delete tmp_row;
	}
}

// TEXTURES
	/**
*
* @param texture_id
* @param filepath
* @param invert_y
* @param generate_mipmaps
*/
void load_texture(GLuint *texture_id, const char* filepath, bool invert_y, bool generate_mipmaps)
{
#ifdef SDL_h_
	SDL_Surface *sdl_tex = IMG_Load(filepath);
	if (sdl_tex == nullptr) {
		std::cout << "Failed loading \"" << filepath << "\"" << std::endl;
		return;
	}
	load_texture(texture_id, sdl_tex, invert_y, generate_mipmaps);
	SDL_FreeSurface(sdl_tex);
#else
	(void)texture_id;
	(void)filepath;
	(void)invert_y;
	(void)generate_mipmaps;
#endif
}
/**
*
* @param texture_id
* @param pixel_size
*/
void load_pattern_texture(GLuint * texture_id, const u32 pixel_size)
{
	assert(texture_id);
	glDeleteTextures(1, texture_id);

	const Color colors[] = {
		0x0000ffff /*blue*/, 0x00ffffff /*cyan*/, 0x00ff00ff /*green*/,
		0xffff00ff/*yellow*/, 0xff0000ff/*red*/, 0xff69b4ff /*pink*/
	};
	const u32 colorCount = sizeof(colors) / sizeof(Color);
	assert(pixel_size >= colorCount);
	const u32 colorByteSize = 4;	/* RGBA -> MUST BE 4-byte aligned */
	const u32 widthBytes = pixel_size * colorByteSize;
	const u32 colorQuadByteWidth = widthBytes / colorCount;
	const u32 colorQuadByteHeight = pixel_size / colorCount;
	//allocate mem
	std::vector<u8> data(pixel_size * widthBytes, 0);
	for (u32 y = 0; y < pixel_size; ++y)
	{
		for (u32 colorChunk = 0; colorChunk < colorCount; ++colorChunk)
		{
			const u32 idx = (pixel_size - y - 1) * widthBytes + colorChunk * colorQuadByteWidth;
			const u32 color_idx = (colorChunk + (y / colorQuadByteHeight)) % colorCount;
			const Color& c = colors[color_idx];
			for (u32 currPixIdx = 0; currPixIdx < colorQuadByteWidth; currPixIdx += colorByteSize)
			{
				const u32 & color_int = (const u32&)c;
				std::memcpy(data.data() + idx + currPixIdx, &color_int, colorByteSize * sizeof(u8));
			}
		}
	}
	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_size, pixel_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	pop_gl_errors(__FUNCTION__);
}
/**
*
* @param texture_id
* @param color
*/
void load_color_texture(GLuint * texture_id, Color color) {
// Single pixel white texture
	assert(texture_id);
	glDeleteTextures(1, texture_id);
	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &u32(color));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	pop_gl_errors(__FUNCTION__);
}

// UTILS

	/**
*
* @brief get monitor size in pixels
* @return
*/
ivec2 get_monitor_size()
{
// get monitor size
	int width, height, xpos, ypos;
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	//glfwGetMonitorContentScale(monitor, &scale_x, &scale_y);
	//glfwGetMonitorPhysicalSize(monitor, &monitor_w, &monitor_h);
	glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
	return ivec2{width, height};
}


/**
*
* @brief initialize graphics system
*/
void Graphics::init()
{
	int error_code = -1;
	// initialize GLFW
	error_code = glfwInit();
	if (!error_code) {
		// failed initializing GLFW
		std::cerr << "glfwInit failed with code \"" << error_code << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	// set OpenGL context version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//initialize window & gl_context
	window_size = get_monitor_size();
	window = glfwCreateWindow(window_size.x, window_size.y, "CS350 - MARTXELO", NULL, NULL);
	if (!window) {
		// failed creating window
		glfwTerminate();
		std::cerr << "glfwCreateWindow failed creating a WINDOW" << std::endl;
		exit(EXIT_FAILURE);
	}
	// create context
	glfwMakeContextCurrent(window);
	// initialize GLAD lib
	error_code = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!error_code) {
		glfwTerminate();
		std::cerr << "gladLoadGLLoader failed with code \"" << error_code << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	error_code = gladLoadGL();
	if (!error_code) {
		glfwTerminate();
		std::cerr << "gladLoadGL failed with code \"" << error_code << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	// set buffer swap interval
	glfwSwapInterval(1);
	
	// set viewport
	glViewport(0, 0, window_size.x, window_size.y);

	//openGL alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	// Does not work with G-BUFFER!!!
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
#if 0	// Debug info
	GLint range[2];
	glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, range);
	printf("SMOOTH RANGE min=%d max=%d\n", range[0], range[1]);
	glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range);
	printf("ALIASED RANGE min=%d max=%d\n", range[0], range[1]);
#endif

	pop_gl_errors("OpenGL Environment Initialization");

	//SHADER INITIALIZATION
	compile_shaders();

	// PRIMITIVE CREATION
	create_aabb_mesh(AABB{ vec3{-1.f}, vec3{1.f} }, &primitives[primitive_cube]);
	create_sphere_mesh(Sphere{ vec3{}, 1.f }, &primitives[primitive_sphere]);

	// IMGUI initialization
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		const ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui_ImplGlfw_InitForOpenGL(graphics.window, true);
		ImGui_ImplOpenGL3_Init("#version 420");
		//ImGui::StyleColorsClassic();
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
	}

	//check for GL errors
	pop_gl_errors(__FUNCTION__);
}
/**
*
* @brief shutdown graphics system
* @return
*/
void Graphics::free()
{
	//free ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//delete all textures
	for (int i = 0; i < Texture::texture_count; ++i)
		glDeleteTextures(1, &tex[i]);

	//delete shaders
	delete_shaders();

	//free window & gl_context
	glfwDestroyWindow(window);
	glfwTerminate();
}

/**
*
* @brief transform from window pixel coordinates to NDC space
* @return
*/
mat3 Graphics::window_to_ndc() const
{
// NDC coordinates x{-1,1},y{-1,1}
	int w = window_size.x, h = window_size.y;
	//glfwGetWindowSize(window, &w, &h);
	// remember that glm matrices are column mayor, transpose what you know!
	return mat3(
		2.f / w, 0, 0,
		0, -2.f / h, 0,
		-1, 1, 1);
}

/**
*
* @brief transform from NDC space to window coordinates
* @return
*/
mat3 Graphics::ndc_to_window() const
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);

	return mat3(
		(float)w / 2.f, 0, 0,
		0, (float)h / 2.f, 0,
		(float)w / 2.f, (float)h / 2.f, 1
	);
}

/**
*
* @brief toggle fullscreen/windowed
* @return
*/
bool Graphics::toggle_fullscreen()
{
	ivec2 monitor = get_monitor_size();
	if (is_fullscreen) {
		// switch to windowed
		ivec2 screen_centered_pos = {monitor.x / 2 - window_size.x / 2, monitor.y / 2 - window_size.y / 2};
		glfwSetWindowMonitor(window, NULL, screen_centered_pos.x, screen_centered_pos.y, window_size.x, window_size.y, 1);
		glViewport(0, 0, window_size.x, window_size.y);
	}
	else
	{
		// switch to fullscreen
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, monitor.x, monitor.y, 1);
		glViewport(0, 0, monitor.x, monitor.y);
	}

	return is_fullscreen = !is_fullscreen;
}
/**
*
* @param size_x
* @param size_y
* @return
*/
void Graphics::resize_window(int size_x, int size_y)
{
	window_size = {size_x, size_y};
	glfwSetWindowSize(window, size_x, size_y);
	glViewport(0, 0, size_x, size_y);
}


// SHADERS

	/**
*
* @brief delete all shader programs from memory
*/
void Graphics::delete_shaders()
{
	for (int i = 0; i < Shader::shader_count; i++)
		glDeleteProgram(shader_program[i]);
}
/**
*
* @brief hardcoded initialization of every shader program
*/
void Graphics::compile_shaders()
{
	GLuint vertex_shader, fragment_shader;
	// RAW
	{
		vertex_shader = compile_shader("../resources/shaders/raw.vert");
		fragment_shader = compile_shader("../resources/shaders/raw.frag");

		//compile sh_line shader program
		shader_program[Shader::sh_raw] = glCreateProgram();
		glAttachShader(shader_program[Shader::sh_raw], vertex_shader);
		glAttachShader(shader_program[Shader::sh_raw], fragment_shader);
		//glAttachShader(sh_line_shader_program, geometry_shader);
		glLinkProgram(shader_program[Shader::sh_raw]);


		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		check_gl_program_link_errors(shader_program[Shader::sh_raw]);
		pop_gl_errors("sh_raw Shader Initialization");

	}
	// COLOR
	{
		vertex_shader = compile_shader("../resources/shaders/color.vert");
		fragment_shader = compile_shader("../resources/shaders/color.frag");

		//compile sh_line shader program
		shader_program[Shader::sh_color] = glCreateProgram();
		glAttachShader(shader_program[Shader::sh_color], vertex_shader);
		glAttachShader(shader_program[Shader::sh_color], fragment_shader);
		//glAttachShader(sh_line_shader_program, geometry_shader);
		glLinkProgram(shader_program[Shader::sh_color]);


		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		check_gl_program_link_errors(shader_program[Shader::sh_color]);
		pop_gl_errors("sh_color Shader Initialization");

	}
}

// DEBUG

	/**
*
* @brief add a segment to debug lines list, ready to be poped and rendered
* @param start
* @param end
*/
void Graphics::debug_line(const vec3& start, const vec3& end)
{
	m_debug_lines.push_back(start);
	m_debug_lines.push_back(end);
}
/**
*
* @brief draw all stacked debug lines and pop them after
* @param m
* @param color
* @param line_width
*/
void Graphics::draw_debug_lines(const mat4 & m, Color color, float line_width)
{
	if (m_debug_lines.empty())
		return;
	// create buffer
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_debug_lines.size(), m_debug_lines.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw
	//glLineWidth(line_width);
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &m[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glDrawArrays(GL_LINES, 0, (GLsizei)m_debug_lines.size());

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	m_debug_lines.clear();

	pop_gl_errors(__FUNCTION__);
}

// draw primitives
/**
*
* @param obb
* @param mvp
* @param color
*/
void draw_obb(const AABB& obb, const mat4& mvp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(mvp)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_cube].vao);
	graphics.primitives[primitive_cube].draw();
}
/**
*
* @param ab
* @param vp
* @param color
*/
void draw_aabb(const AABB& ab, const mat4& vp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	vec3 diff = ab.max_point - ab.min_point;
	diff /= 2.f;
	vec3 center = ab.min_point + diff;
	mat4 model = glm::translate(center) * glm::scale(diff);
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(vp * model)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_cube].vao);
	graphics.primitives[primitive_cube].draw();
}
/**
*
* @param sph
* @param vp
* @param color
*/
void draw_sphere(const Sphere& sph, const mat4& vp, Color color)
{
	glUseProgram(graphics.shader_program[sh_color]);
	mat4 model = glm::translate(sph.center) * glm::scale(vec3{sph.radius});
	glUniformMatrix4fv(glGetUniformLocation(graphics.shader_program[sh_color], "MVP"), 1, false, &(vp * model)[0][0]);
	color.set_uniform_RGBA(glGetUniformLocation(graphics.shader_program[sh_color], "color"));
	glBindVertexArray(graphics.primitives[primitive_sphere].vao);
	graphics.primitives[primitive_sphere].draw();
}
