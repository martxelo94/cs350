/**
* @file input.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/02/01	(eus)
* @brief Define bounding volume creators
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/
#include "pch.h"

//JoystickDevice joystick;	// Done Singleton
//KeyboardDevice keyboard;	// Done Singleton
//MouseDevice mouse;		// Done Singleton

/**
*
* @brief reset mouse info
*/
void MouseDevice::init()
{
	InputDevice::init();
	pos = prevPos = move = scrollOffset = prevScrollOffset = scroll = ivec2{};
	overWindow = false;
}
/**
*
* @brief update input buttons and mouse info
*/
void MouseDevice::update()
{
	InputDevice::update();
	// update mouse motion
	move = pos - prevPos;
	prevPos = pos;
	// update mouse scroll
	scroll = scrollOffset - prevScrollOffset;
	prevScrollOffset = scrollOffset;
}
/**
*
* @brief create a ray in NDC space from near to far, in the direction of the cursor
* @return
*/
Ray MouseDevice::ray_cast() const
{
// return ray in NDC space
	vec3 mouse_pos{pos, 1.f};
	mouse_pos = graphics.window_to_ndc() * mouse_pos;
	mouse_pos.z = -1.f;
	return Ray{ mouse_pos, vec3{0, 0, 2} };
}


// callback functions

	/**
*
* @param window
* @param key
* @param scancode
* @param action
* @param mods
*/
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// not used yet...
	(void)scancode;
	(void)mods;

	assert(key >= KEYS_LOW_BOUND);
	assert(key <= KEYS_HIGH_BOUND);

	// close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	
	if (action == GLFW_PRESS) {
		keyboard.set(key, KeyState::triggered);
	}
	else if (action == GLFW_RELEASE) {
		keyboard.set(key, KeyState::up);
	}

	// IMGUI
	//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}
/**
*
* @param window
* @param button
* @param action
* @param mods
*/
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	// not used yet...
	(void)mods;

	assert(button >= GLFW_MOUSE_BUTTON_1);
	assert(button <= GLFW_MOUSE_BUTTON_LAST);

	if (action == GLFW_PRESS) {
		mouse.set(button, KeyState::triggered);
	}
	else if (action == GLFW_RELEASE) {
		mouse.set(button, KeyState::up);
	}

	// IMGUI
	//ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}
/**
*
* @param window
* @param xpos
* @param ypos
*/
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
// handle mouse position; top-left coordinate relative
	mouse.pos = { xpos, ypos };
	
	// handle ImGui mouse pos
	//ImGuiIO& io = ImGui::GetIO();
	//io.MousePos.x = mouse.pos.x;
	//io.MousePos.y = mouse.pos.y;
}
/**
*
* @param window
* @param entered 
*/
void mouse_over_callback(GLFWwindow* window, int entered)
{
// handle mouse enter/leave window area
	if (entered)
		mouse.overWindow = true;
	else
		mouse.overWindow = false;
}
/**
*
* @param window
* @param xoffset
* @param yoffset
*/
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
// handle mouse scroll wheel
	mouse.scrollOffset = {xoffset, yoffset};

	// IMGUI
	//ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}
