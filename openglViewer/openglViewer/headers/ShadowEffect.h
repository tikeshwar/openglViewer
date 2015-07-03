#pragma once

namespace glv
{
	class ShadowEffect
	{
	public:
		ShadowEffect(glv::LightSharedPtr light, glv::CameraSharedPtr camera, int screenWidth, int screenHeight);
		~ShadowEffect();

		void initialize();
		void render(glv::DrawableNodeSharedPtr drawableNode);

		void release();

		GLuint shaderProgram()const;
		GLuint shaderProgram();

		glv::LightSharedPtr light()const;
		glv::LightSharedPtr light();

		glv::CameraSharedPtr camera()const;
		glv::CameraSharedPtr camera();

		const glm::mat4 & depthMVP()const;
		glm::mat4 & depthMVP();

		GLuint depthTexture()const;
		GLuint depthTexture();

	private:
		GLuint mShaderProgram;
		GLuint mDepthTexture;
		GLuint mFrameBuffer;

		GLuint mDepthMVP;
		GLuint mNodeTransform;

		glm::mat4 mDepthMVPMat;

		glv::LightSharedPtr mLight;
		glv::CameraSharedPtr mCamera;
		int mWidth, mHeight;
	};
}

