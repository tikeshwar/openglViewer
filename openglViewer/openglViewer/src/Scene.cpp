#include "Stdafx.h"
#include "Scene.h"

using namespace glv;

Scene::Scene()
{
	mWidth = 640;
	mHeight = 480;

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

Scene::Scene(int width, int height)
:mWidth(width), mHeight(height)
{
	Scene();
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
	// Dark blue background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = Shaders::LoadShaders("shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	glUseProgram(programID);
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUseProgram(programID);
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	glUseProgram(programID);
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	glUseProgram(programID);
	GLuint ProjectionMatrixID = glGetUniformLocation(programID, "P");
	glUseProgram(programID);
	GLuint NormalMatrixID = glGetUniformLocation(programID, "N");
	glUseProgram(programID);
	GLuint ObjectMatrixID = glGetUniformLocation(programID, "OM");

	glUseProgram(programID);
	GLuint TextureID = glGetUniformLocation(programID, "TextureSampler");
	glUseProgram(programID);
	GLuint IsTexAvailable = glGetUniformLocation(programID, "IsTexAvailable");

	mCamera->setWindowSize(mWidth, mHeight);
	mCamera->setSize(mBBox);

	LightSharedPtr light;
	if (!mLights.empty())						
		light = mLights.begin()->second;

	glfwSetKeyCallback(mWindow, glv::glvKeyCallback);
	glfwSetCursorPosCallback(mWindow, glv::glvMouseMoveCallback);
	glfwSetMouseButtonCallback(mWindow, glv::glvMouseButtonCallback);
	glfwSetScrollCallback(mWindow, glv::glvScrollCallback);

	glUseProgram(programID);
	GLuint LightPosID = glGetUniformLocation(programID, "LightPosition_worldspace");
	glUseProgram(programID);
	GLuint LightAmbientID = glGetUniformLocation(programID, "LightAmbientColor");
	glUseProgram(programID);
	GLuint LightDiffuseID = glGetUniformLocation(programID, "LightDiffuseColor");
	glUseProgram(programID);
	GLuint LightSpecularID = glGetUniformLocation(programID, "LightSpecularColor");

	glUseProgram(programID);
	GLuint MatAmbientID = glGetUniformLocation(programID, "MatAmbientColor");
	glUseProgram(programID);
	GLuint MatDiffuseID = glGetUniformLocation(programID, "MatDiffuseColor");
	glUseProgram(programID);
	GLuint MatSpecularID = glGetUniformLocation(programID, "MatSpecularColor");
	glUseProgram(programID);
	GLuint MatShininessID = glGetUniformLocation(programID, "MatShininess");

	glUseProgram(programID);
	GLuint BBoxID = glGetUniformLocation(programID, "Scene_center");
	glm::vec3 bboxCenter = mBBox.center();
	glUniform3f(BBoxID, bboxCenter.x, bboxCenter.y, bboxCenter.z);

	do{
		glm::mat4 MVP = mCamera->MVP();
		glm::mat4 modelMatrix = mCamera->modelMatrix();
		glm::mat4 viewMatrix = mCamera->viewMatrix();
		glm::mat4 projectionMatrix = mCamera->projectionMatrix();
		glm::mat4 normalMatrix = mCamera->normalMatrix();

		//glm::vec4 camPosition = glm::inverse(viewMatrix * modelMatrix) * glm::vec4(mCamera->position().x, mCamera->position().y, mCamera->position().z, 0);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(NormalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);

		if (light)
		{
			light->setPosition(mCamera->position());
			glUseProgram(programID);
			glUniform3f(LightPosID, light->position().x, light->position().y, light->position().z);
			glUniform3f(LightAmbientID, light->ambientColor().x, light->ambientColor().y, light->ambientColor().z);
			glUniform3f(LightDiffuseID, light->diffuseColor().x, light->diffuseColor().y, light->diffuseColor().z);
			glUniform3f(LightSpecularID, light->specularColor().x, light->specularColor().y, light->specularColor().z);
		}

		mRootNode->traverse([&](const DrawableNodeSharedPtr & node)
		{
			if (node->data()->isVisible())
			{
				glUniformMatrix4fv(ObjectMatrixID, 1, GL_FALSE, &(node->data()->transform())[0][0]);

				const glv::Material	& nodeMat = (std::dynamic_pointer_cast<MeshDrawable>(node->data()))->material();
				glUniform3f(MatAmbientID, nodeMat.ambientColor().x, nodeMat.ambientColor().y, nodeMat.ambientColor().z);
				glUniform3f(MatDiffuseID, nodeMat.diffuseColor().x, nodeMat.diffuseColor().y, nodeMat.diffuseColor().z);
				glUniform3f(MatSpecularID, nodeMat.specularColor().x, nodeMat.specularColor().y, nodeMat.specularColor().z);
				glUniform1f(MatShininessID, nodeMat.shininess());

				if (nodeMat.textureID())
					glUniform1i(IsTexAvailable, 1);
				else
					glUniform1i(IsTexAvailable, 0);

				if (nodeMat.textureID() > 0)
				{
					// Bind our texture in Texture Unit 0
					glActiveTexture(GL_TEXTURE0);
					// Set our "myTextureSampler" sampler to user Texture Unit 0
					glBindTexture(GL_TEXTURE_2D, nodeMat.textureID());
					glUniform1i(TextureID, 0);
				}

				node->data()->draw();
			}
		});

		glfwSwapBuffers(mWindow);
		glfwPollEvents();

	} while (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mWindow) == 0);

	// Cleanup VBO and shader
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void Scene::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

