#pragma once

struct GLFWwindow;

namespace glv
{
	// error callback
	void errorCallback(int error, const char* description);

	// callback for key func GLFWKeyFun
	void glvKeyCallback(GLFWwindow * glfwWindow, int x, int y, int key, int action);

	// callback for mouse move
	void glvMouseMoveCallback(GLFWwindow* glfwWindow, double x, double y);

	//callback for mouse callback
	void glvMouseButtonCallback(GLFWwindow * glfwWindow, int button, int action, int mods);

	// callback for scroll 
	void glvScrollCallback(GLFWwindow * glfwWindow, double xoffet, double yoffet);
}