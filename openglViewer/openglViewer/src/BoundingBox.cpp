#include "Stdafx.h"
#include "BoundingBox.h"

using namespace glv;

BoundingBox::BoundingBox()
{
	mInitialized = false;
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox(const glm::vec3 & lower, const glm::vec3 & upper)
{
	mLower = lower;
	mUpper = upper;
	mInitialized = true;
}

void BoundingBox::extend(double x, double y, double z)
{
	if (mInitialized)
	{
		mLower.x = x < mLower.x ? x : mLower.x;
		mLower.y = y < mLower.y ? y : mLower.y;
		mLower.z = z < mLower.z ? z : mLower.z;

		mUpper.x = x > mUpper.x ? x : mUpper.x;
		mUpper.y = y > mUpper.y ? y : mUpper.y;
		mUpper.z = z > mUpper.z ? z : mUpper.z;
	}
	else
	{
		mLower = mUpper = glm::vec3(x, y, z);
		mInitialized = true;
	}
}

void BoundingBox::extend(const glm::vec3 & point)
{
	if (mInitialized)
	{
		mLower.x = point.x < mLower.x ? point.x : mLower.x;
		mLower.y = point.y < mLower.y ? point.y : mLower.y;
		mLower.z = point.z < mLower.z ? point.z : mLower.z;

		mUpper.x = point.x > mUpper.x ? point.x : mUpper.x;
		mUpper.y = point.y > mUpper.y ? point.y : mUpper.y;
		mUpper.z = point.z > mUpper.z ? point.z : mUpper.z;
	}
	else
	{
		mLower = mUpper = point;
		mInitialized = true;
	}
}

void BoundingBox::expand(float scalarVal)
{
	mLower.x -= scalarVal;
	mLower.y -= scalarVal;
	mLower.z -= scalarVal;

	mUpper.x += scalarVal;
	mUpper.y += scalarVal;
	mUpper.z += scalarVal;
}

void BoundingBox::extend(const BoundingBox & box)
{
	this->extend(box.lower());
	this->extend(box.upper());
}

bool BoundingBox::intersect(const BoundingBox & box)
{
	return contains(box.lower()) || contains(box.upper());
}

bool BoundingBox::intersect(const BoundingBox & box)const
{
	return contains(box.lower()) || contains(box.upper());
}

bool BoundingBox::contains(const glm::vec3 & point)
{
	return mLower.x > point.x && mLower.y > point.y && mLower.z > point.z && mUpper.x < point.x && mUpper.y < point.y && mUpper.z < point.z;
}

bool BoundingBox::contains(const glm::vec3 & point)const
{
	return mLower.x > point.x && mLower.y > point.y && mLower.z > point.z && mUpper.x < point.x && mUpper.y < point.y && mUpper.z < point.z;
}

glm::vec3 BoundingBox::diagonal()const
{
	return glm::vec3(mUpper.x - mLower.x, mUpper.y - mLower.y, mUpper.z - mLower.z);
}

const glm::vec3 & BoundingBox::lower()const
{
	return mLower;
}

glm::vec3 BoundingBox::lower()
{
	return mLower;
}

const glm::vec3 & BoundingBox::upper()const
{
	return mUpper;
}

glm::vec3 BoundingBox::upper()
{
	return mUpper;
}

glm::vec3 BoundingBox::center()const
{
	return glm::vec3((mLower.x + mUpper.x) / 2, (mLower.y + mUpper.y) / 2, (mLower.z + mUpper.z) / 2);
}

bool BoundingBox::initialized()
{
	return mInitialized;
}
bool BoundingBox::initialized()const
{
	return mInitialized;
}
