#include "Stdafx.h"
#include "NewFrameEffect.h"

using namespace glv;

NewFrameEffect::NewFrameEffect()
{
}

NewFrameEffect::~NewFrameEffect()
{
}

void NewFrameEffect::initialize()
{
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	mShaderProgram = glv::Shaders::LoadShaders("shaders/Passthrough.vertexshader", "shaders/SimpleTexture.fragmentshader");
	mTexture = glGetUniformLocation(mShaderProgram, "texture");
}

void NewFrameEffect::render(GLuint textureID, GLint top, GLint left, GLint width, GLint height)
{
	glViewport(top, left, width, height);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(mTexture, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
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
}

GLuint NewFrameEffect::shaderProgram()const
{
	return mShaderProgram;
}
GLuint NewFrameEffect::shaderProgram()
{
	return mShaderProgram;
}

void NewFrameEffect::release()
{
	glDeleteProgram(mShaderProgram);
	glDeleteTextures(1, &mTexture);
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(1, &mVertexBuffer);
}

void NewFrameEffect::bindBuffer()
{
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(mShaderProgram);
}
