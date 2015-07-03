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
	mCamera->setWindowSize(mWidth, mHeight);
	mCamera->setSize(mBBox);
	mCamera->resetView(Camera::Parallel);	 // need to do coz shadows are created using orthographic
	mCamera->resetView(Camera::Perspective);

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

	// ---------------------------------------------
	// Render to Texture - specific code begins here for shadow
	// ---------------------------------------------

	glv::ShadowEffect shadowEffect(light, mCamera, width(), height());
	shadowEffect.initialize();

	glv::NewFrameEffect quadTexture;
	quadTexture.initialize();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, width(), height());

	// Create and compile our GLSL program from the shaders
	GLuint programID = Shaders::LoadShaders("shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ProjectionMatrixID = glGetUniformLocation(programID, "P");
	GLuint NormalMatrixID = glGetUniformLocation(programID, "N");
	GLuint ObjectMatrixID = glGetUniformLocation(programID, "OM");

	GLuint TextureID = glGetUniformLocation(programID, "TextureSampler");
	GLuint IsTexAvailable = glGetUniformLocation(programID, "IsTexAvailable");

	GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");

	GLuint LightPosID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LightAmbientID = glGetUniformLocation(programID, "LightAmbientColor");
	GLuint LightDiffuseID = glGetUniformLocation(programID, "LightDiffuseColor");
	GLuint LightSpecularID = glGetUniformLocation(programID, "LightSpecularColor");

	GLuint MatAmbientID = glGetUniformLocation(programID, "MatAmbientColor");
	GLuint MatDiffuseID = glGetUniformLocation(programID, "MatDiffuseColor");
	GLuint MatSpecularID = glGetUniformLocation(programID, "MatSpecularColor");
	GLuint MatShininessID = glGetUniformLocation(programID, "MatShininess");

	GLuint BBoxID = glGetUniformLocation(programID, "Scene_center");
	glm::vec3 bboxCenter = mBBox.center();
	glUniform3f(BBoxID, bboxCenter.x, bboxCenter.y, bboxCenter.z);

	do{

		// first grab shadow from light POV
		shadowEffect.render(mRootNode);

		// Use our shader

		glUseProgram(programID);

		glDisable(GL_CULL_FACE);

		glm::mat4 MVP = mCamera->MVP();
		glm::mat4 modelMatrix = mCamera->modelMatrix();
		glm::mat4 viewMatrix = mCamera->viewMatrix();
		glm::mat4 projectionMatrix = mCamera->projectionMatrix();
		glm::mat4 normalMatrix = mCamera->normalMatrix();

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);

		glm::mat4 depthBiasMVP = biasMatrix*shadowEffect.depthMVP();
		//glm::mat4 depthBiasMVP = depthMVP;
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);


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
			//light->setPosition(mCamera->position());
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
				glm::mat4 & nodeMat4 = node->data()->transform();
				glUniformMatrix4fv(ObjectMatrixID, 1, GL_FALSE, &nodeMat4[0][0]);

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

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, shadowEffect.depthTexture());
				glUniform1i(ShadowMapID, 1);

				node->data()->draw();
			}
		});

		//quadTexture.render(shadowEffect.depthTexture(), 100, 100, width() / 3, height() / 3);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();

	} while (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mWindow) == 0);

	// Cleanup VBO and shader
	glDeleteProgram(programID);

	glDeleteTextures(1, &TextureID);
}

void Scene::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

