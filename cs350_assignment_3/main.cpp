#include "pch.h"
#include <gtest/gtest.h>
#include <cfenv>

#define _CRT_SECURE_NO_WARNINGS

void enable_floating_point_exceptions()
{
#ifdef _MSC_VER
	unsigned exceptions = 0;
	exceptions |= EM_ZERODIVIDE;
	exceptions |= EM_INVALID;
	_controlfp_s(&exceptions, ~exceptions, _MCW_EM);
	//_controlfp(~exceptions, _MCW_EM);
#else
	feenableexcept(FE_DIVBYZERO | FE_INVALID);
#endif
}

int main_demo(int argc, char** argv)
{

	// disable unused variable warning
	(void)argc;
	(void)argv;

	// initialize systems
	graphics.init();
	// input system	
	keyboard.init();
	mouse.init();

	glfwSetKeyCallback(graphics.window, keyboard_callback);
	glfwSetMouseButtonCallback(graphics.window, mouse_callback);
	glfwSetCursorPosCallback(graphics.window, mouse_move_callback);
	glfwSetCursorEnterCallback(graphics.window, mouse_over_callback);
	glfwSetScrollCallback(graphics.window, mouse_scroll_callback);
	// mouse settings
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(graphics.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	else
		std::cerr << "Current MACHINE does not support RAW_MOUSE_MOTION! Please, consider upgrading your MACHINE." << std::endl;
	
	

	// reset time system
	frc.reset();

	Demo* demo = new Demo();

	// main loop
	while (!glfwWindowShouldClose(graphics.window)) {
		frc.start_frame();
		// Update ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		// update input
		keyboard.update();
		mouse.update();
		// let GLFW communicate with window system
		glfwPollEvents();

		// update scene
		demo->update();
		// render scene
		demo->render();
		// render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffers
		glfwSwapBuffers(graphics.window);

		frc.end_frame();
	}

	delete demo;
	demo = nullptr;

	// shutdown systems
	graphics.free();

	return EXIT_SUCCESS;
};

GTEST_API_ int main(int argc, char** argv)
{
	enable_floating_point_exceptions();

#if 0
	if (argc > 1 && strcmp(argv[1], "test") == 0){
		// GTest
		testing::InitGoogleTest(&argc, argv);
		auto ret = RUN_ALL_TESTS();
		getchar();
		return ret;
	}
	else
	{
		// Demo
		return main_demo(argc, argv);
	}
#else
	// fast testing from VS execution
	testing::InitGoogleTest(&argc, argv);
	auto ret = RUN_ALL_TESTS();
	getchar();
	return ret;
#endif
}
