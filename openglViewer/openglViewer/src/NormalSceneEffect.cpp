#include "Stdafx.h"
#include "NormalSceneEffect.h"

using namespace glv;

NormalSceneEffect::NormalSceneEffect(glv::CameraSharedPtr camera, glv::LightSharedPtr light, int width, int height)
: mCamera(camera)
, mLight(light)
, mWidth(width)
, mHeight(height)
{
}


NormalSceneEffect::~NormalSceneEffect()
{
	release();
}

void NormalSceneEffect::initialize()
{
	{
		mProgramID = Shaders::LoadShaders("shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader");

		// Get a handle for our "MVP" uniform
		mMatrixID = glGetUniformLocation(mProgramID, "MVP");
		mModelMatrixID = glGetUniformLocation(mProgramID, "M");
		mViewMatrixID = glGetUniformLocation(mProgramID, "V");
		mProjectionMatrixID = glGetUniformLocation(mProgramID, "P");
		mNormalMatrixID = glGetUniformLocation(mProgramID, "N");
		mObjectMatrixID = glGetUniformLocation(mProgramID, "OM");

		mTextureID = glGetUniformLocation(mProgramID, "TextureSampler");
		mIsTexAvailable = glGetUniformLocation(mProgramID, "IsTexAvailable");

		mDepthBiasID = glGetUniformLocation(mProgramID, "DepthBiasMVP");
		mShadowMapID = glGetUniformLocation(mProgramID, "shadowMap");
		mReflectionID = glGetUniformLocation(mProgramID, "ReflectionMap");

		mLightPosID = glGetUniformLocation(mProgramID, "LightPosition_worldspace");
		mLightLookAtID = glGetUniformLocation(mProgramID, "LightLookAt");
		mLightAmbientID = glGetUniformLocation(mProgramID, "LightAmbientColor");
		mLightDiffuseID = glGetUniformLocation(mProgramID, "LightDiffuseColor");
		mLightSpecularID = glGetUniformLocation(mProgramID, "LightSpecularColor");

		mMatAmbientID = glGetUniformLocation(mProgramID, "MatAmbientColor");
		mMatDiffuseID = glGetUniformLocation(mProgramID, "MatDiffuseColor");
		mMatSpecularID = glGetUniformLocation(mProgramID, "MatSpecularColor");
		mMatShininessID = glGetUniformLocation(mProgramID, "MatShininess");

		mReflectionEnabledID = glGetUniformLocation(mProgramID, "reflectionEnabled");
		mShadowEnabledID = glGetUniformLocation(mProgramID, "shadowEnabled");
	}

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

void NormalSceneEffect::renderWithReflection(glv::DrawableNodeSharedPtr drawableNode)
{
	// reflect the camera about the plane


	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

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

	glm::mat4 depthBiasMVP = biasMatrix*mShadowEffect->depthMVP();
	//glm::mat4 depthBiasMVP = depthMVP;
	glUniformMatrix4fv(mDepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);


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

	glUniform1i(mReflectionEnabledID, 1);
	glUniform1i(mShadowEnabledID, 1);

	drawableNode->traverse([&](const DrawableNodeSharedPtr & node)
	{
		// idea is to render the plane and render the texture
		if (!node->data()->ifIncludedInReflectionCalculation())
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

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mReflectionEffect->colorTexture());
			glUniform1i(mReflectionID, 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mShadowEffect->depthTexture());
			glUniform1i(mShadowMapID, 2);

			node->data()->draw();
		}
	});										 
}

void NormalSceneEffect::renderWithShadow(glv::DrawableNodeSharedPtr drawableNode)
{


	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
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

	glm::mat4 depthBiasMVP = biasMatrix*mShadowEffect->depthMVP();
	//glm::mat4 depthBiasMVP = depthMVP;
	glUniformMatrix4fv(mDepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);


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

	glUniform1i(mReflectionEnabledID, 0);
	glUniform1i(mShadowEnabledID, 1);

	drawableNode->traverse([&](const DrawableNodeSharedPtr & node)
	{
		if (node->data()->isVisible())
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

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mShadowEffect->depthTexture());
			glUniform1i(mShadowMapID, 1);

			node->data()->draw();
		}
	});
}

void NormalSceneEffect::render(glv::DrawableNodeSharedPtr drawableNode)
{

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glm::mat4 MVP = mCamera->MVP();
	glm::mat4 modelMatrix = mCamera->modelMatrix();
	glm::mat4 viewMatrix = mCamera->viewMatrix();
	glm::mat4 projectionMatrix = mCamera->projectionMatrix();
	glm::mat4 normalMatrix = mCamera->normalMatrix();

	glm::mat4 depthBiasMVP(1.0);
	glUniformMatrix4fv(mDepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);


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

	glUniform1i(mReflectionEnabledID, 0);
	glUniform1i(mShadowEnabledID, 0);

	drawableNode->traverse([&](const DrawableNodeSharedPtr & node)
	{
		if (node->data()->isVisible())
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

void NormalSceneEffect::setCamera(glv::CameraSharedPtr camera)
{
	mCamera = camera;
}

void NormalSceneEffect::release()
{
	glDeleteProgram(mProgramID);
	glDeleteTextures(1, &mTextureID);

	glDeleteFramebuffers(1, &mFrameBuffer);
	glDeleteFramebuffers(1, &mDepthBufferRenderer);
	glDeleteTextures(1, &mColorTexture);
}

GLuint NormalSceneEffect::shaderProgram()const
{
	return mProgramID;
}

GLuint NormalSceneEffect::shaderProgram()
{
	return mProgramID;
}

glv::CameraSharedPtr NormalSceneEffect::camera()const
{
	return mCamera;
}

glv::CameraSharedPtr NormalSceneEffect::camera()
{
	return mCamera;
}

glv::LightSharedPtr NormalSceneEffect::light()const
{
	return mLight;
}

glv::LightSharedPtr NormalSceneEffect::light()
{
	return mLight;
}

GLuint NormalSceneEffect::colorTexture()const
{
	return mColorTexture;
}
GLuint NormalSceneEffect::colorTexture()
{
	return mColorTexture;
}

void NormalSceneEffect::bindBuffer()
{
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mWidth, mHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glUseProgram(mProgramID);
}

void NormalSceneEffect::setReflectionEffect(glv::ReflectionEffectSharedPtr reflectionEffect)
{
	mReflectionEffect = reflectionEffect;
}

glv::ReflectionEffectSharedPtr NormalSceneEffect::reflectionEffect()const
{
	return mReflectionEffect;
}

glv::ReflectionEffectSharedPtr NormalSceneEffect::reflectionEffect()
{
	return mReflectionEffect;
}

void NormalSceneEffect::setShadowEffect(glv::ShadowEffectSharedPtr shadowEffect)
{
	mShadowEffect = shadowEffect;
}

glv::ShadowEffectSharedPtr NormalSceneEffect::shadowEffect()const
{
	return mShadowEffect;
}

glv::ShadowEffectSharedPtr NormalSceneEffect::shadowEffect()
{
	return mShadowEffect;
}