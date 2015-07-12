#include "Stdafx.h"
#include "Scene.h"

using namespace glv;

Scene::Scene()
{
	Scene(800, 640);
}

Scene::Scene(int width, int height)
: mWidth(width)
, mHeight(height)
{
	mIsReflectionEnabled = true;
	mIsShadowEnabled = true;

	// Initialise GLFW														    
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a mWindow and create its OpenGL context
	mWindow = glfwCreateWindow(mWidth, mHeight, "Tutorial 04 - Colored Cube", NULL, NULL);
	if (mWindow == NULL){
		fprintf(stderr, "Failed to open GLFW mWindow. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(mWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
}

Scene::~Scene()
{

}

void Scene::addCamera(CameraSharedPtr camera)
{
	mCamera = camera;
}

void Scene::addDrawable(DrawableSharedPtr drawable)
{
	addDrawableNode(std::make_shared<DrawableNode>(drawable));
}

void Scene::addDrawableNode(DrawableNodeSharedPtr drawableNode)
{
	if (mRootNode)
		mRootNode->addChild(drawableNode);
	else
		mRootNode = drawableNode;

	if (drawableNode->data()->ifIncludedInBBoxCalculation())
		mBBox.extend(drawableNode->data()->geomPackBBox());
}

void Scene::addLight(LightSharedPtr lightNode)
{
	mLights.insert(std::pair<std::string, LightSharedPtr>(lightNode->name(), lightNode));
}

void Scene::removeLight(const char* lightName)
{
	auto findIt = mLights.find(lightName);
	if (findIt != mLights.end())
	{
		mLights.erase(findIt);
	}
}

void Scene::render()
{
	LightSharedPtr light;
	if (!mLights.empty())
		light = mLights.begin()->second;

	glfwSetKeyCallback(mWindow, glv::glvKeyCallback);
	glfwSetCursorPosCallback(mWindow, glv::glvMouseMoveCallback);
	glfwSetMouseButtonCallback(mWindow, glv::glvMouseButtonCallback);
	glfwSetScrollCallback(mWindow, glv::glvScrollCallback);


	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	auto normalSceneEffect = std::make_shared<NormalSceneEffect>(mCamera, light, width(), height());
	normalSceneEffect->initialize();

	auto shadowEffect = std::make_shared<ShadowEffect>(light, mCamera, width(), height());
	shadowEffect->initialize();

	const glm::vec4 reflectionPlane(0, 0, 1, -mBBox.lower().z);
	auto reflectionEffect = std::make_shared<ReflectionEffect>(reflectionPlane, mCamera, light, width(), height());
	reflectionEffect->initialize();

	normalSceneEffect->setReflectionEffect(reflectionEffect);
	normalSceneEffect->setShadowEffect(shadowEffect);

	glv::NewFrameEffect quadTexture;
	quadTexture.initialize();

	do{

		glEnable(GL_CLIP_DISTANCE0);

		// first grab shadow from light POV
		shadowEffect->bindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadowEffect->render(mRootNode);

		// render it to reflection frame buffer
		reflectionEffect->bindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		reflectionEffect->render(mRootNode);

		normalSceneEffect->bindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (mIsReflectionEnabled)
			normalSceneEffect->renderWithReflection(mRootNode);
		//if (mIsShadowEnabled)
			//normalSceneEffect->renderWithShadow(mRootNode);
		//if (!mIsShadowEnabled && !mIsReflectionEnabled)
		normalSceneEffect->render(mRootNode);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//quadTexture.bindBuffer();
		//quadTexture.render(shadowEffect->depthTexture(), 0, 0, width() / 3, height() / 3);

		//quadTexture.bindBuffer();
		//quadTexture.render(reflectionEffect.colorTexture(), width() * 2.0 / 3, height() * 2.0 / 3, width() / 3, height() / 3);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();

	} while (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mWindow) == 0);

	// Cleanup VBO and shader

}

void Scene::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

void Scene::enableShadow(bool enable)
{
	mIsShadowEnabled = enable;
}

bool Scene::isShadowEnabled()const
{
	return mIsShadowEnabled;
}

bool Scene::isShadowEnabled()
{
	return mIsShadowEnabled;
}

void Scene::enableReflection(bool enable)
{
	mIsReflectionEnabled = enable;
}

bool Scene::isReflectionEnabled()const
{
	return mIsReflectionEnabled;
}

bool Scene::isReflectionEnabled()
{
	return mIsReflectionEnabled;
}

