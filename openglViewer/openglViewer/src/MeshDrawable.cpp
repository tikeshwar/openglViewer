#include "Stdafx.h"
#include "MeshDrawable.h"

using namespace glv;

MeshDrawable::MeshDrawable()
{
	mAutoNormals = false;
	mIsVisible = true;
	mToIncludeInBBox = true;
	mToIncludeInTransform = true;
	mToIncludeInShadow = true;
	mTransformMat = glm::mat4(1.0);
	mName = "default";
}

MeshDrawable::~MeshDrawable()
{
	// Cleanup VBO
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mColorBuffer);
	glDeleteBuffers(1, &mNormalBuffer);
	glDeleteBuffers(1, &mUVBuffer);
}

const char* MeshDrawable::name()const
{
	return mName.data();
}

const char* MeshDrawable::name()
{
	return mName.data();
}

void MeshDrawable::setName(const char* name)
{
	mName = std::string(name);
}

bool MeshDrawable::isVisible()const
{
	return mIsVisible;
}

bool MeshDrawable::isVisible()
{
	return mIsVisible;
}

void MeshDrawable::setVisible(bool isVisible)
{
	mIsVisible = isVisible;
}

void MeshDrawable::setBufferObject(GLuint* buffer, const void* dataArray, int count, int dataTypeSize)
{
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
	glBufferData(GL_ARRAY_BUFFER, count*dataTypeSize, dataArray, GL_STATIC_DRAW);
}

void MeshDrawable::copyArray(const std::vector<glm::vec3> & inputArray, std::vector<float> & outputArray)
{
	outputArray.resize(inputArray.size() * 3);
	for (int i = 0; i < inputArray.size(); i++)
	{
		outputArray[3 * i + 0] = inputArray[i].x;
		outputArray[3 * i + 1] = inputArray[i].y;
		outputArray[3 * i + 2] = inputArray[i].z;
	}
}
void MeshDrawable::copyArray(const std::vector<glm::vec2> & inputArray, std::vector<float> & outputArray)
{
	outputArray.resize(inputArray.size() * 3);
	for (int i = 0; i < inputArray.size(); i++)
	{
		outputArray[3 * i + 0] = inputArray[i].x;
		outputArray[3 * i + 1] = inputArray[i].y;
	}
}

void MeshDrawable::copyArray(const std::vector<glm::vec3> & inputArray, std::vector<unsigned int> & outputArray)
{
	outputArray.resize(inputArray.size() * 3);
	for (int i = 0; i < inputArray.size(); i++)
	{
		outputArray[3 * i + 0] = inputArray[i].x;
		outputArray[3 * i + 1] = inputArray[i].y;
		outputArray[3 * i + 2] = inputArray[i].z;
	}
}

void MeshDrawable::setVertexArray(const float* vertexArray, int arraySize)
{
	mVertexArray.assign(vertexArray, vertexArray + arraySize);
	setBufferObject(&mVertexBuffer, mVertexArray.data(), mVertexArray.size(), sizeof(mVertexArray[0]));

	calculateBBox();
}

void MeshDrawable::setIndexArray(const int* indexArray, int arraySize)
{
	mIndexArray.assign(indexArray, indexArray + arraySize);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, arraySize*sizeof(int), mIndexArray.data(), GL_STATIC_DRAW);
}

void MeshDrawable::setColorArray(const float* colorArray, int arraySize)
{
	mColorArray.assign(colorArray, colorArray + arraySize);
	setBufferObject(&mColorBuffer, mColorArray.data(), mColorArray.size(), sizeof(mColorArray[0]));
}

void MeshDrawable::setNormalArray(const float* normalArray, int arraySize)
{
	mNormalArray.assign(normalArray, normalArray + arraySize);
	setBufferObject(&mNormalBuffer, mNormalArray.data(), mNormalArray.size(), sizeof(mNormalArray[0]));
}

void MeshDrawable::setUVArray(const float* uvArray, int arraySize)
{
	mUVArray.assign(uvArray, uvArray + arraySize);
	setBufferObject(&mUVBuffer, mUVArray.data(), mUVArray.size(), sizeof(mUVArray[0]));
}

void MeshDrawable::setVertexArray(const std::vector<glm::vec3> & vertexArray)
{
	copyArray(vertexArray, mVertexArray);
	setBufferObject(&mVertexBuffer, mVertexArray.data(), mVertexArray.size(), sizeof(mVertexArray[0]));

	calculateBBox();
}
void MeshDrawable::setIndexArray(const std::vector<glm::vec3> & indexArray)
{
	copyArray(indexArray, mIndexArray);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexArray.size()*sizeof(mIndexArray[0]), mIndexArray.data(), GL_STATIC_DRAW);
}
void MeshDrawable::setColorArray(const std::vector<glm::vec3> & colorArray)
{
	//copyArray(colorArray, mColorArray);
	//setBufferObject(&mColorBuffer, mColorArray.data(), mColorArray.size(), sizeof(mColorArray[0]));
}
void MeshDrawable::setNormalArray(const std::vector<glm::vec3> & normalArray)
{
	copyArray(normalArray, mNormalArray);
	setBufferObject(&mNormalBuffer, mNormalArray.data(), mNormalArray.size(), sizeof(mNormalArray[0]));
}
void MeshDrawable::setUVArray(const std::vector<glm::vec2> & uvArray)
{
	copyArray(uvArray, mUVArray);
	setBufferObject(&mUVBuffer, mUVArray.data(), mUVArray.size(), sizeof(mUVArray[0]));
}


void MeshDrawable::setAutoNormals(bool bAutoNormals)
{
	mAutoNormals = bAutoNormals;
	if (mAutoNormals)
	{

	}
}

const BoundingBox & MeshDrawable::geomPackBBox()const
{
	return mGeomPackBBox;
}

BoundingBox MeshDrawable::geomPackBBox()
{
	return mGeomPackBBox;
}

const BoundingBox & MeshDrawable::geomBBox()const
{
	return mGeomBBox;
}

BoundingBox MeshDrawable::geomBBox()
{
	return mGeomBBox;
}

void MeshDrawable::setGeomBBox(glv::BoundingBox bbox)
{
	mGeomBBox = bbox;
}

void MeshDrawable::setGeomPackBBox(glv::BoundingBox bbox)
{
	mGeomPackBBox = bbox;
}

const glm::mat4 & MeshDrawable::transform()const
{
	return mTransformMat;
}

glm::mat4 MeshDrawable::transform()
{
	return mTransformMat;
}

void MeshDrawable::setTransform(const glm::mat4 & mat)
{
	mTransformMat = mat;

	Utility util;
	mGeomBBox = util.updateBBox(mGeomBBox, mat);
	mGeomPackBBox = util.updateBBox(mGeomPackBBox, mat);
	//util.traversalUpdateBBox(util.getDrawableNode(std::make_shared<MeshDrawable>(*this)));
}

const glv::Material & MeshDrawable::material()const
{
	return mMaterial;
}

glv::Material MeshDrawable::material()
{
	return mMaterial;
}

void MeshDrawable::setMaterial(const glv::Material & material)
{
	mMaterial = material;
}

void MeshDrawable::setupDraw()
{
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2rd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,						  // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,						  // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glVertexAttribPointer(
		3,                                // attribute
		2,                                // size
		GL_FLOAT,						  // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}

void MeshDrawable::preDraw(const std::function<void()> & functor)
{
	mPreRenderCall = functor;
}

void MeshDrawable::draw()
{
	if (mPreRenderCall != nullptr)
		mPreRenderCall();

	setupDraw();

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		mIndexArray.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
		);

	if (mPostRenderCall != nullptr)
		mPostRenderCall();
	//glDrawArrays(GL_TRIANGLES_ADJACENCY, 0, mVertexArray.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void MeshDrawable::postDraw(const std::function<void()> & functor)
{
	mPostRenderCall = functor;
}

void MeshDrawable::calculateBBox()
{
	BoundingBox bbox;
	for (int i = 0; i < mVertexArray.size(); i += 3)
		bbox.extend(mVertexArray[i], mVertexArray[i + 1], mVertexArray[i + 2]);
	mGeomBBox = bbox;
	mGeomPackBBox = bbox;
}

void MeshDrawable::includeInBBoxCalculation(bool toInclude)
{
	mToIncludeInBBox = toInclude;
}

bool MeshDrawable::ifIncludedInBBoxCalculation()const
{
	return mToIncludeInBBox;
}

bool MeshDrawable::ifIncludedInBBoxCalculation()
{
	return mToIncludeInBBox;
}

void MeshDrawable::includeInSettingTransform(bool toInclude)
{
	mToIncludeInTransform = toInclude;
}

bool MeshDrawable::ifIncludedInSettingTransform()const
{
	return mToIncludeInTransform;
}

bool MeshDrawable::ifIncludedInSettingTransform()
{
	return mToIncludeInTransform;
}

void MeshDrawable::includeInShadowCalculation(bool toInclude)
{
	mToIncludeInShadow = toInclude;
}

bool MeshDrawable::ifIncludedInShadowCalculation()const
{
	return mToIncludeInShadow;
}

bool MeshDrawable::ifIncludedInShadowCalculation()
{
	return mToIncludeInShadow;
}

void MeshDrawable::includeInReflectionCalculation(bool toInclude)
{
	mToIncludeInReflection = toInclude;
}

bool MeshDrawable::ifIncludedInReflectionCalculation()const
{
	return mToIncludeInReflection;
}

bool MeshDrawable::ifIncludedInReflectionCalculation()
{
	return mToIncludeInReflection;
}