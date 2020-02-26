/**
* @file pch.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Include headers
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef PCH_H
#define PCH_H


// std
#include <fstream>
#include <iostream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <bitset>
#include <stack>
#include <memory>	// smart pointers

// glm
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm/gtc/random.hpp>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ImGui
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#ifndef M_PI
#define M_PI    glm::pi<float>()	/* 3.14159265358979323846264338327950288  */
#endif

#define NOT_NAMED_UNIONS 0

#include "types.h"
#include "singleton.h"
#include "shapes.h"
#include "color.h"
#include "mesh.h"
#include "geometry.h"
#include "graphics.h"
#include "input.h"
#include "camera.h"
#include "frame_rate_controller.h"
#include "object.h"
#include "bounding_volume.h"

#include "demo.h"


#endif
