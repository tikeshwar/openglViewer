#pragma once

namespace glv
{
	class NormalSceneEffect
	{
	public:
		NormalSceneEffect(glv::CameraSharedPtr camera, glv::LightSharedPtr light, int width, int height);
		~NormalSceneEffect();

		void initialize();
		void renderWithReflection(glv::DrawableNodeSharedPtr drawableNode);
		void renderWithShadow(glv::DrawableNodeSharedPtr drawableNode);
		void render(glv::DrawableNodeSharedPtr drawableNode);

		void release();

		GLuint shaderProgram()const;
		GLuint shaderProgram();

		void setCamera(glv::CameraSharedPtr camera);
		glv::CameraSharedPtr camera()const;
		glv::CameraSharedPtr camera();

		glv::LightSharedPtr light()const;
		glv::LightSharedPtr light();

		GLuint colorTexture()const;
		GLuint colorTexture();

		void bindBuffer();

		void setReflectionEffect(glv::ReflectionEffectSharedPtr reflectionEffect);
		glv::ReflectionEffectSharedPtr reflectionEffect()const;
		glv::ReflectionEffectSharedPtr reflectionEffect();

		void setShadowEffect(glv::ShadowEffectSharedPtr shadowEffect);
		glv::ShadowEffectSharedPtr shadowEffect()const;
		glv::ShadowEffectSharedPtr shadowEffect();

	private:
		GLuint mProgramID;
		GLuint mReflecetionProgramID;
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
		GLuint mReflectionID;

		GLuint mLightPosID;
		GLuint mLightLookAtID;
		GLuint mLightAmbientID;
		GLuint mLightDiffuseID;
		GLuint mLightSpecularID;

		GLuint mMatAmbientID;
		GLuint mMatDiffuseID;
		GLuint mMatSpecularID;
		GLuint mMatShininessID;

		GLuint mReflectionEnabledID;
		GLuint mShadowEnabledID;

		GLuint mColorTexture;
		GLuint mFrameBuffer;
		GLuint mDepthBufferRenderer;

		int mWidth, mHeight;

		glv::CameraSharedPtr mCamera;
		glv::LightSharedPtr mLight;

		glv::ReflectionEffectSharedPtr mReflectionEffect;
		glv::ShadowEffectSharedPtr mShadowEffect;
	};
}
