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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// ---------------------------------------------
	// Render to Texture - specific code begins here
	// ---------------------------------------------

	// Create and compile our GLSL program from the shaders
	GLuint depthProgramID = Shaders::LoadShaders("shaders/DepthRTT.vertexshader", "shaders/DepthRTT.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
	GLuint depthObjMatrixID = glGetUniformLocation(depthProgramID, "depthOM");


	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width(), height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint quad_programID = glv::Shaders::LoadShaders("shaders/Passthrough.vertexshader", "shaders/SimpleTexture.fragmentshader");
	GLuint texID = glGetUniformLocation(quad_programID, "texture");


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

	// just to set the view to lights POV for shadow
	double zoomFactor = mCamera->zoomfactor();
	double ortho[6];
	mCamera->orthoProjection(&ortho[0]);

	do{
		// Use our shader
		glUseProgram(depthProgramID);

		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, width(), height()); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		// We don't use bias in the shader, but instead we draw back faces, 
		// which are already separated from the front faces by a small distance 
		// (if your geometry is made this way)
		/*		glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT); */// Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 depthMVP;
		{
			glm::mat4 projectionMatrix = glm::ortho(ortho[0] * zoomFactor, ortho[1] * zoomFactor, ortho[2] * zoomFactor, ortho[3] * zoomFactor, ortho[4], ortho[5]);
			// Camera matrix
			glm::mat4 viewMatrix = glm::lookAt(light->position(), mCamera->lookAt(), mCamera->upVector());
			glm::mat4 modelMatrix = glm::mat4(1.0);
			// Our ModelViewProjection : multiplication of our 3 matrices
			depthMVP = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around
		}

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		mRootNode->traverse([&](const DrawableNodeSharedPtr & node)
		{
			if (node->data()->isVisible())
			{
				glm::mat4 & nodeMat4 = node->data()->transform();
				glUniformMatrix4fv(depthObjMatrixID, 1, GL_FALSE, &nodeMat4[0][0]);
				node->data()->draw();
			}
		});

		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width(), height()); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK); */// Cull back-facing triangles -> draw only front-facing triangles

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// The fullscreen quad's FBO

		// Use our shader

		glUseProgram(programID);

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

		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
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
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(ShadowMapID, 1);

				node->data()->draw();
			}
		});



		glViewport(0, 0, width() / 3, height() / 3);
		glUseProgram(quad_programID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		// Set our "renderedTexture" sampler to user Texture Unit 0
		glUniform1i(texID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		//glDisable(GL_COMPARE_R_TO_TEXTURE);
		// Draw the triangle !
		// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glDisableVertexAttribArray(0);


		glfwSwapBuffers(mWindow);
		glfwPollEvents();

	} while (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mWindow) == 0);

	// Cleanup VBO and shader
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	glDeleteProgram(depthProgramID);
	glDeleteTextures(1, &TextureID);
	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteTextures(1, &depthTexture);
}

void Scene::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

