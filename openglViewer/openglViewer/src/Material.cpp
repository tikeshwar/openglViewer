#include "Stdafx.h"
#include "Material.h"

using namespace glv;

Material::Material()
{
	mName = "default";
	mTextureID = 0;
}

Material::Material(const char* matName)
{
	mName = std::string(matName);
	mTextureID = 0;
}

Material::Material(const glm::vec3 & color)
{
	mDiffuseColor = color;
	mAmbientColor = glm::vec3(0.2, 0.2, 0.2)*color;
	mSpecularColor = glm::vec3(0.5, 0.5, 0.5);
	mShininess = 5.0f;
	mTextureID = 0;
}

Material::Material(const char* matName, const glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, float shininess)
{
	mName = std::string(matName);
	mAmbientColor = ambientColor;
	mDiffuseColor = diffuseColor;
	mSpecularColor = specularColor;
	mShininess = shininess;
	mTextureID = 0;
}

Material::~Material()
{
}

void Material::setAmbientColor(const glm::vec3 & ambientColor)
{
	mAmbientColor = ambientColor;
}

void Material::setDiffuseColor(const glm::vec3 & diffuseColor)
{
	mDiffuseColor = diffuseColor;
}

void Material::setSpecularColor(const glm::vec3 & specularColor)
{
	mSpecularColor = specularColor;
}

void Material::setShininessColor(float shininess)
{
	mShininess = shininess;
}

const glm::vec3 & Material::ambientColor()const
{
	return mAmbientColor;
}

glm::vec3 Material::ambientColor()
{
	return mAmbientColor;
}

const glm::vec3 & Material::diffuseColor()const
{
	return mDiffuseColor;
}

glm::vec3 Material::diffuseColor()
{
	return mDiffuseColor;
}

const glm::vec3 & Material::specularColor()const
{
	return mSpecularColor;
}

glm::vec3 Material::specularColor()
{
	return mSpecularColor;
}

float Material::shininess()const
{
	return mShininess;
}

float Material::shininess()
{
	return mShininess;
}

void Material::applyTexture(const char* imagePath)
{
	//mTextureID = mTexLoader.loadBMP_custom(imagePath);
	
	FIBITMAP* bitmap = FreeImage_Load(
		FreeImage_GetFileType(imagePath, 0),
		imagePath);
	FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
	int nWidth = FreeImage_GetWidth(pImage);
	int nHeight = FreeImage_GetHeight(pImage);

	glGenTextures(1, &mTextureID);

	glBindTexture(GL_TEXTURE_2D, mTextureID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

	FreeImage_Unload(pImage);

	mDiffuseColor = glm::vec3(1.0);
	mAmbientColor = glm::vec3(0.2, 0.2, 0.2)*mDiffuseColor;
	mSpecularColor = glm::vec3(0.5, 0.5, 0.5);
	mShininess = 5.0f;
}

GLuint Material::textureID()
{
	return mTextureID;
}

GLuint Material::textureID()const
{
	return mTextureID;
}

