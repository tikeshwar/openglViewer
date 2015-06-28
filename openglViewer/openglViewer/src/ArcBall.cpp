#include "Stdafx.h"
#include "ArcBall.h"

using namespace glv;

ArcBall::ArcBall()
: mInitialized(false)
{

}

ArcBall::ArcBall(int width, int height)
: mInitialized(false), mWidth(width), mHeight(height)
{
}

ArcBall::~ArcBall()
{
}

void ArcBall::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
}

void ArcBall::updateRotation(int curPosx, int curPosy, const glm::mat4 & inverseofMV)
{
	if (mInitialized)
	{
		// limit the screen pts to [-1, 1];
		glm::vec3 P = getPointOnArcBall(curPosx, curPosy);
		glm::vec3 Q = getPointOnArcBall(mLastPosx, mLastPosy);
		//float angle = glm::acos(glm::min(1.0f, glm::dot(P, Q)));
		float angle = glm::acos(glm::dot(P, Q));
		if (angle < 0.175 && angle > -0.175)
		{
			glm::vec3 axis = glm::cross(Q, P);
			if (glm::length(axis) > 0.000001)
			{
				glm::vec4 camAxis = inverseofMV * glm::vec4(axis, 0.0);
				glm::dquat deltaQuat = glm::angleAxis(glm::degrees(angle) / 2.0f, glm::vec3(camAxis.x, camAxis.y, camAxis.z));
				deltaQuat = glm::normalize(deltaQuat);
				mRotQuat *= deltaQuat;
			}
		}
	}
	else
		mInitialized = true;

	mLastPosx = curPosx;
	mLastPosy = curPosy;
}

glm::mat4 ArcBall::transformationMatrix()const
{
	return glm::mat4_cast(mRotQuat);
}

void ArcBall::setToInitialState()
{
	mRotQuat = glm::dquat();
}

glm::vec3 ArcBall::getPointOnArcBall(int x, int y)
{
	double rad = 1.0f;
	glm::vec3 P(2.0f * x / mWidth - rad, rad - 2.0f * y / mHeight, 0);

	double sqz = P.x*P.x + P.y*P.y;
	if (sqz <= rad*rad)
		P.z = glm::sqrt(rad*rad - sqz);
	else
	{
		P = glm::normalize(P);
	}
	return P;
}


