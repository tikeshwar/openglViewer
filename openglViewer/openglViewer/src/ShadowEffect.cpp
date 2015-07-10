#include "Stdafx.h"
#include "ShadowEffect.h"

using namespace glv;

ShadowEffect::ShadowEffect(glv::LightSharedPtr light, glv::CameraSharedPtr camera, int screenWidth, int screenHeight)
: mLight(light)
, mCamera(camera)
, mWidth(screenWidth)
, mHeight(screenHeight)
{
}

ShadowEffect::~ShadowEffect()
{
	release();
}

void ShadowEffect::initialize()
{
	// Create and compile our GLSL program from the shaders
	mShaderProgram = Shaders::LoadShaders("shaders/DepthRTT.vertexshader", "shaders/DepthRTT.fragmentshader");

	// Get a handle for our "MVP" uniform
	mDepthMVP = glGetUniformLocation(mShaderProgram, "depthMVP");
	mNodeTransform = glGetUniformLocation(mShaderProgram, "depthOM");


	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	mFrameBuffer = 0;
	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &mDepthTexture);
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowEffect::render(glv::DrawableNodeSharedPtr drawableNode)
{
	// Use our shader

	if (mCamera->projectionType() == Camera::ProjectionType::Parallel)
	{
		double zoomFactor = 1.0;
		double ortho[6];
		mCamera->orthoProjection(&ortho[0]);

		glm::mat4 projectionMatrix = glm::ortho(ortho[0] * zoomFactor, ortho[1] * zoomFactor, ortho[2] * zoomFactor, ortho[3] * zoomFactor, ortho[4], ortho[5]);
		// Camera matrix
		glm::mat4 viewMatrix = glm::lookAt(mLight->position(), mCamera->lookAt(), mCamera->upVector());
		glm::mat4 modelMatrix = glm::mat4(1.0);
		// Our ModelViewProjection : multiplication of our 3 matrices
		mDepthMVPMat = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around
	}
	else
	{
		double zoomFactor = 1.0;
		double frustum[6];
		mCamera->frustumProjection(&frustum[0]);

		glm::mat4 projectionMatrix = glm::frustum(frustum[0] * zoomFactor, frustum[1] * zoomFactor, frustum[2] * zoomFactor, frustum[3] * zoomFactor, frustum[4], frustum[5]);
		// Camera matrix
		glm::mat4 viewMatrix = glm::lookAt(mLight->position(), mCamera->lookAt(), mCamera->upVector());
		glm::mat4 modelMatrix = glm::mat4(1.0);
		// Our ModelViewProjection : multiplication of our 3 matrices
		mDepthMVPMat = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around
	}

	// We don't use bias in the shader, but instead we draw back faces, 
	// which are already separated from the front faces by a small distance 
	// (if your geometry is made this way)
/*	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);*/ // Cull back-facing triangles -> draw only front-facing triangles

	// Clear the screen

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(mDepthMVP, 1, GL_FALSE, &mDepthMVPMat[0][0]);

	drawableNode->traverse([&](const DrawableNodeSharedPtr & node)
	{
		if (node->data()->isVisible() && node->data()->ifIncludedInShadowCalculation())
		{
			glm::mat4 & nodeMat4 = node->data()->transform();
			glUniformMatrix4fv(mNodeTransform, 1, GL_FALSE, &nodeMat4[0][0]);
			node->data()->draw();
		}
	});
}

void ShadowEffect::release()
{
	glDeleteProgram(mShaderProgram);
	glDeleteFramebuffers(1, &mFrameBuffer);
	glDeleteTextures(1, &mDepthTexture);
}

GLuint ShadowEffect::shaderProgram()const
{
	return mShaderProgram;
}
GLuint ShadowEffect::shaderProgram()
{
	return mShaderProgram;
}

glv::LightSharedPtr ShadowEffect::light()const
{
	return mLight;
}
glv::LightSharedPtr ShadowEffect::light()
{
	return mLight;
}

glv::CameraSharedPtr ShadowEffect::camera()const
{
	return mCamera;
}
glv::CameraSharedPtr ShadowEffect::camera()
{
	return mCamera;
}

const glm::mat4 & ShadowEffect::depthMVP()const
{
	return mDepthMVPMat;
}
glm::mat4 & ShadowEffect::depthMVP()
{
	return mDepthMVPMat;
}

GLuint ShadowEffect::depthTexture()const
{
	return mDepthTexture;
}
GLuint ShadowEffect::depthTexture()
{
	return mDepthTexture;
}

void ShadowEffect::bindBuffer()
{
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glViewport(0, 0, mWidth, mHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glUseProgram(mShaderProgram);
}
