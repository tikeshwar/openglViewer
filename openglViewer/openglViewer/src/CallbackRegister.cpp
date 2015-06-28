#include "Stdafx.h"
#include "Scene.h"
#include "CallbackRegister.h"

namespace 
{
	double gPosx, gPosy;		// current cursor position
	int gMButton, gMButtonStatus, gMmods;	// mouse btn status
	int gKey;	// keyboard press
	int gScrollx, gScrolly;
}

extern std::shared_ptr<glv::Scene> gScene;

void glv::errorCallback(int error, const char* description)
{
	std::cout << description << std::endl;
}

void glv::glvKeyCallback(GLFWwindow * glfwWindow, int key, int scan, int action, int mods)
{
	gKey = key;
	gScene->activeCamera()->setKeyCallback(gPosx, gPosy, key, action);
}

//callback for mouse callback
void glv::glvMouseButtonCallback(GLFWwindow * glfwWindow, int button, int action, int mods)
{
	gMButton = button;
	gMButtonStatus = action;
	gMmods = mods;
	gScene->activeCamera()->setMouseButtonCallback(gPosx, gPosy, button, action, mods);
}

void glv::glvMouseMoveCallback(GLFWwindow* glfwWindow, double x, double y)
{
	gPosx = x;
	gPosy = y;
	gScene->activeCamera()->setMouseButtonCallback(gPosx, gPosy, gMButton, gMButtonStatus, gMmods);
}

// callback for scroll 
void glv::glvScrollCallback(GLFWwindow * glfwWindow, double xoffset, double yoffset)
{
	gScrollx = xoffset;
	gScrollx = yoffset;
	gScene->activeCamera()->setScrollCallback(gPosx, gPosy, xoffset, yoffset);
}