#include "Stdafx.h"
#include "Light.h"

using namespace glv;

// defaults to white light with no spot component positioned at (0,0,0)
Light::Light(const char* name)
{
	setName(name);
	setColor(glm::vec3(1, 1, 1));
	setPosition(glm::vec3(0, 0, 0));	
}
// defaults to white light with no spot component positioned at position
Light::Light(const char* name, const glm::vec3 & position, const glm::vec3 & lookAt)
{
	setName(name);
	setColor(glm::vec3(1, 1, 1));
	setLookAt(lookAt);
	setPosition(position);
}

Light::Light(const char* name, const glm::vec3 & color, const glm::vec3 & position, const glm::vec3 & lookAt)
{
	setName(name);
	setColor(color);
	setPosition(position);
	setLookAt(lookAt);
}

Light::Light(const char* name, const glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, const glm::vec3 & position, const glm::vec3 & lookAt)
{
	setName(name);
	setAmbientColor(ambientColor);
	setDiffuseColor(diffuseColor);
	setSpecularColor(specularColor);
	setPosition(position);
	setLookAt(lookAt);
}

Light::~Light()
{

}

void Light::setName(const char* name)
{
	mName = name;
}

void Light::setColor(const glm::vec3 & color)
{
	mDiffuseColor = color;
	mAmbientColor = color;
	mSpecularColor = color;
}

void Light::setPosition(const glm::vec3 & position)
{
	mPosition = position;
}

void Light::setLookAt(const glm::vec3 & lookAt)
{
	mLookAt = lookAt;
}

void Light::setAmbientColor(const glm::vec3 & color)
{
	mAmbientColor = color;
}

void Light::setDiffuseColor(const glm::vec3 & color)
{
	mDiffuseColor = color;
}

void Light::setSpecularColor(const glm::vec3 & color)
{
	mSpecularColor = color;
}

const char* Light::name()
{
	return mName.data();
}

const char* Light::name()const
{
	return mName.data();
}

const glm::vec3 & Light::ambientColor()const
{
	return mAmbientColor;
}

glm::vec3 Light::ambientColor()
{
	return mAmbientColor;
}

const glm::vec3 & Light::diffuseColor()const
{
	return mDiffuseColor;
}

glm::vec3 Light::diffuseColor()
{
	return mDiffuseColor;
}

const glm::vec3 & Light::specularColor()const
{
	return mSpecularColor;
}

glm::vec3 Light::specularColor()
{
	return mSpecularColor;
}

const glm::vec3 & Light::position()const
{
	return mPosition;
}

glm::vec3 Light::position()
{
	return mPosition;
}

const glm::vec3 & Light::lookAt()const
{
	return mLookAt;
}

glm::vec3 Light::lookAt()
{
	return mLookAt;
}
