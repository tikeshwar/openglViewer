#pragma once

namespace glv
{
	class NormalSceneEffect;
	class ReflectionEffect
	{
	public:
		ReflectionEffect(const glm::vec4 & plane, glv::CameraSharedPtr camera, glv::LightSharedPtr light, int width, int height);
		~ReflectionEffect();

		void initialize();
		void render(glv::DrawableNodeSharedPtr drawableNode);

		void release();

		glv::CameraSharedPtr camera()const;
		glv::CameraSharedPtr camera();

		GLuint colorTexture()const;
		GLuint colorTexture();

		void bindBuffer();

	private:
		glm::vec4 mPlane;
		GLuint mProgramID;
		GLuint mMatrixID;
		GLuint mModelMatrixID;
		GLuint mViewMatrixID;
		GLuint mProjectionMatrixID;
		GLuint mNormalMatrixID;
		GLuint mObjectMatrixID;

		GLuint mTextureID;
		GLuint mIsTexAvailable;

		GLuint mDepthBiasID;
		GLuint mShadowMapID;

		GLuint mLightPosID;
		GLuint mLightLookAtID;
		GLuint mLightAmbientID;
		GLuint mLightDiffuseID;
		GLuint mLightSpecularID;

		GLuint mMatAmbientID;
		GLuint mMatDiffuseID;
		GLuint mMatSpecularID;
		GLuint mMatShininessID;

		GLuint mPlaneID;

		GLuint mColorTexture;
		GLuint mFrameBuffer;
		GLuint mDepthBufferRenderer;

		int mWidth, mHeight;

		glv::CameraSharedPtr mCamera;
		glv::LightSharedPtr mLight;
	};
}

