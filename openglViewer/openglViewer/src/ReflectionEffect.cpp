#include "Stdafx.h"
#include "ReflectionEffect.h"

using namespace glv;

ReflectionEffect::ReflectionEffect(const glm::vec4 & plane, glv::CameraSharedPtr camera, glv::LightSharedPtr light, int width, int height)
: mPlane(plane)
, mCamera(camera)
, mLight(light)
, mWidth(width)
, mHeight(height)
{
}

ReflectionEffect::~ReflectionEffect()
{
	release();
}

void ReflectionEffect::initialize()
{
	mProgramID = Shaders::LoadShaders("shaders/Reflection.vertexshader", "shaders/Reflection.fragmentshader");

	// Get a handle for our "MVP" uniform
	mMatrixID = glGetUniformLocation(mProgramID, "MVP");
	mModelMatrixID = glGetUniformLocation(mProgramID, "M");
	mViewMatrixID = glGetUniformLocation(mProgramID, "V");
	mProjectionMatrixID = glGetUniformLocation(mProgramID, "P");
	mNormalMatrixID = glGetUniformLocation(mProgramID, "N");
	mObjectMatrixID = glGetUniformLocation(mProgramID, "OM");

	mTextureID = glGetUniformLocation(mProgramID, "TextureSampler");
	mIsTexAvailable = glGetUniformLocation(mProgramID, "IsTexAvailable");

	mLightPosID = glGetUniformLocation(mProgramID, "LightPosition_worldspace");
	mLightLookAtID = glGetUniformLocation(mProgramID, "LightLookAt");
	mLightAmbientID = glGetUniformLocation(mProgramID, "LightAmbientColor");
	mLightDiffuseID = glGetUniformLocation(mProgramID, "LightDiffuseColor");
	mLightSpecularID = glGetUniformLocation(mProgramID, "LightSpecularColor");

	mMatAmbientID = glGetUniformLocation(mProgramID, "MatAmbientColor");
	mMatDiffuseID = glGetUniformLocation(mProgramID, "MatDiffuseColor");
	mMatSpecularID = glGetUniformLocation(mProgramID, "MatSpecularColor");
	mMatShininessID = glGetUniformLocation(mProgramID, "MatShininess");
	mPlaneID = glGetUniformLocation(mProgramID, "ReflectionPlane");
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	mFrameBuffer = 0;
	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &mColorTexture);
	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glGenRenderbuffers(1, &mDepthBufferRenderer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferRenderer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferRenderer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mColorTexture, 0);

	// No color output in the bound framebuffer, only depth.
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ReflectionEffect::render(glv::DrawableNodeSharedPtr drawableNode)
{
	glm::mat4 MVP = mCamera->MVP();
	glm::mat4 modelMatrix = mCamera->modelMatrix();
	glm::mat4 viewMatrix = mCamera->viewMatrix();
	glm::mat4 projectionMatrix = mCamera->projectionMatrix();
	glm::mat4 normalMatrix = mCamera->normalMatrix();

	// Use our shader
	glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(mModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(mViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(mProjectionMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(mNormalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);

	if (mLight)
	{
		//light->setPosition(mCamera->position());
		glUniform3f(mLightPosID, mLight->position().x, mLight->position().y, mLight->position().z);
		glUniform3f(mLightLookAtID, mLight->lookAt().x, mLight->lookAt().y, mLight->lookAt().z);
		glUniform3f(mLightAmbientID, mLight->ambientColor().x, mLight->ambientColor().y, mLight->ambientColor().z);
		glUniform3f(mLightDiffuseID, mLight->diffuseColor().x, mLight->diffuseColor().y, mLight->diffuseColor().z);
		glUniform3f(mLightSpecularID, mLight->specularColor().x, mLight->specularColor().y, mLight->specularColor().z);
	}

	glUniform4f(mPlaneID, mPlane.x, mPlane.y, mPlane.z, mPlane.w);

	drawableNode->traverse([&](const DrawableNodeSharedPtr & node)
	{
		if (node->data()->isVisible() && node->data()->ifIncludedInShadowCalculation())
		{
			glm::mat4 & nodeMat4 = node->data()->transform();
			glUniformMatrix4fv(mObjectMatrixID, 1, GL_FALSE, &nodeMat4[0][0]);

			const glv::Material	& nodeMat = (std::dynamic_pointer_cast<MeshDrawable>(node->data()))->material();
			glUniform3f(mMatAmbientID, nodeMat.ambientColor().x, nodeMat.ambientColor().y, nodeMat.ambientColor().z);
			glUniform3f(mMatDiffuseID, nodeMat.diffuseColor().x, nodeMat.diffuseColor().y, nodeMat.diffuseColor().z);
			glUniform3f(mMatSpecularID, nodeMat.specularColor().x, nodeMat.specularColor().y, nodeMat.specularColor().z);
			glUniform1f(mMatShininessID, nodeMat.shininess());

			if (nodeMat.textureID())
				glUniform1i(mIsTexAvailable, 1);
			else
				glUniform1i(mIsTexAvailable, 0);

			if (nodeMat.textureID() > 0)
			{
				// Bind our texture in Texture Unit 0
				glActiveTexture(GL_TEXTURE0);
				// Set our "myTextureSampler" sampler to user Texture Unit 0
				glBindTexture(GL_TEXTURE_2D, nodeMat.textureID());
				glUniform1i(mTextureID, 0);
			}

			node->data()->draw();
		}
	});
}

void ReflectionEffect::release()
{
	glDeleteProgram(mProgramID);
	glDeleteFramebuffers(1, &mFrameBuffer);
	glDeleteFramebuffers(1, &mDepthBufferRenderer);
	glDeleteTextures(1, &mColorTexture);
}

glv::CameraSharedPtr ReflectionEffect::camera()const
{
	return mCamera;
}
glv::CameraSharedPtr ReflectionEffect::camera()
{
	return mCamera;
}

GLuint ReflectionEffect::colorTexture()const
{
	return mColorTexture;
}
GLuint ReflectionEffect::colorTexture()
{
	return mColorTexture;
}

void ReflectionEffect::bindBuffer()
{
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glViewport(0, 0, mWidth, mHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glUseProgram(mProgramID);
}