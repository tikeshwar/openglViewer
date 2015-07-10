#pragma once

namespace glv
{
	class NewFrameEffect
	{
	public:
		NewFrameEffect();
		~NewFrameEffect();

		void initialize();
		void render(GLuint textureID, GLint top, GLint left, GLint width, GLint height);

		GLuint shaderProgram()const;
		GLuint shaderProgram();

		void release();

		void bindBuffer();

	private:
		GLuint mShaderProgram;
		GLuint mTexture;
		GLuint mVertexArray;
		GLuint mVertexBuffer;
	};
}

