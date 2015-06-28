#include "Stdafx.h"
#include "Camera.h"

using namespace glv;

Camera::Camera()
{
	setProjectionType(Camera::ProjectionType::Perspective);
	setFOV(glm::radians(32.0f));
	setZoom(1.0);
	initProjection();
}

Camera::Camera(const glm::vec3 & position, const glm::vec3 & upVector, const glm::vec3 & lookAt, Camera::ProjectionType projectionType, float fovInRads)
{
	initProjection();
	setProjectionType(projectionType);
	setPosition(position.x, position.y, position.z);
	setUpVector(upVector.x, upVector.y, upVector.z);
	setLookAt(lookAt.x, lookAt.y, lookAt.z);
	setFOV(glm::radians<float>(fovInRads));
	setZoom(1.0);
	apply();
}

Camera::~Camera()
{

}

void Camera::initProjection()
{
	// ortho
	{
		double zNear = 1.0f, zFar = 1000.0f, ar = 4.0f / 3.0f;
		double w = 10.0f;
		double h = w / ar;

		mOrthoLeft = -w, mOrthoRight = w;
		mOrthoBottom = -h, mOrthoTop = h;
		mOrthoNear = zNear, mOrthoFar = zFar;
	}

	// perspective
	{
		double zNear = 1.0f, zFar = 1000.0f, ar = 4.0f / 3.0f;
		double w = glm::tan((float)mFieldOfView) * zNear;
		double h = w / ar;

		mFrustumLeft = -w, mFrustumRight = w;
		mFrustumBottom = -h, mFrustumTop = h;
		mFrustumNear = zNear, mFrustumFar = zFar;
	}
}

void Camera::resetView()
{
	setZoom(1.0);
	mArcBall.setToInitialState();
	mMVP = mViewMatrix = mModelMatrix = mProjectionMatrix = glm::mat4();
	setSize(mBBox);
}

void Camera::resetView(ProjectionType projectionType)
{
	if (projectionType == ProjectionType::Parallel)
	{
		mProjectionType = ProjectionType::Parallel;
		setSize(mBBox);
		updateZoom(0, 0);
	}
	else if (projectionType == ProjectionType::Perspective)
	{
		mProjectionType = ProjectionType::Perspective;
		setSize(mBBox);
		updateZoom(0, 0);
	}
}

void Camera::setWindowSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	mArcBall.setWindowSize(mWidth, mHeight);
}

void Camera::setProjectionType(ProjectionType projectionType)
{
	mProjectionType = projectionType;
}

void Camera::setPosition(double px, double py, double pz)
{
	mPosition.x = px;
	mPosition.y = py;
	mPosition.z = pz;
}

void Camera::setUpVector(double ux, double uy, double uz)
{
	mUpVector.x = ux;
	mUpVector.y = uy;
	mUpVector.z = uz;
}

void Camera::setLookAt(double lx, double ly, double lz)
{
	mLookAt.x = lx;
	mLookAt.y = ly;
	mLookAt.z = lz;
}

void Camera::setFOV(double angleInRads)
{
	mFieldOfView = angleInRads;
}

void Camera::setZoom(double zoomfactor)
{
	mZoomFactor = zoomfactor;
}

int Camera::setKeyCallback(int x, int y, int key, int action)
{
	switch (key)
	{
	case GLFW_KEY_P:
		resetView(ProjectionType::Perspective);
		break;
	case GLFW_KEY_O:
		resetView(ProjectionType::Parallel);
		break;
	case GLFW_KEY_R:
		resetView();
		break;
	}
	return 0;
}

int Camera::setMouseMoveCallback(int x, int y)
{
	return 0;
}

int Camera::setMouseButtonCallback(int x, int y, int button, int action, int mods)
{
	int btn = 0, state = 0;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		btn = 1;
		state = 1;

		mArcBall.updateRotation(x, y, glm::inverse(mViewMatrix * mModelMatrix));
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		btn = 1;
		state = 1;

		mArcBall.setInitialized(false);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		btn = 2;
		state = 1;

		updatePan(x, y);
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		btn = 2;
		state = 1;
	}

	apply();
	return 0;
}

int Camera::setScrollCallback(int x, int y, double xoffet, double yoffet)
{
	updateZoom(xoffet, yoffet);
	return 0;
}

void Camera::updateRotation(int curPosx, int curPosy)
{
	mArcBall.updateRotation(curPosx, curPosy, glm::inverse(mViewMatrix * mModelMatrix));
	apply();
}

void Camera::updateZoom(double xOffset, double yOffset)
{
	if (yOffset > 0)
		mZoomFactor *= 0.98;
	else if (yOffset < 0)
		mZoomFactor *= 1.02;

	apply();
}

void Camera::updatePan(int curPosx, int curPosy)
{
	double delx = (curPosx - mLastx);
	double dely = (curPosy - mLasty);

	if (glm::abs(delx) < 100 && glm::abs(dely) < 100)
	{
		if (mProjectionType == ProjectionType::Perspective)
		{
			delx = delx*(mFrustumLeft - mFrustumRight) / (float)mWidth;
			dely = dely*(mFrustumBottom - mFrustumTop) / (float)mHeight;

			mFrustumLeft += delx;
			mFrustumRight += delx;
			mFrustumTop -= dely;
			mFrustumBottom -= dely;

			apply();
		}
		else if (mProjectionType == ProjectionType::Parallel)
		{
			delx = delx*(mOrthoLeft - mOrthoRight) / (float)mWidth;
			dely = dely*(mOrthoBottom - mOrthoTop) / (float)mHeight;

			mOrthoLeft += delx;
			mOrthoRight += delx;
			mOrthoTop -= dely;
			mOrthoBottom -= dely;

			apply();
		}
	}
	mLastx = curPosx;
	mLasty = curPosy;
}

// left, right, bottom, top, near, far
void Camera::orthoProjection(double* ortho)
{
	ortho[0] = mOrthoLeft;
	ortho[1] = mOrthoRight;
	ortho[2] = mOrthoBottom;
	ortho[3] = mOrthoTop;
	ortho[4] = mOrthoNear;
	ortho[5] = mOrthoFar;
}
void Camera::orthoProjection(double* ortho)const
{
	ortho[0] = mOrthoLeft;
	ortho[1] = mOrthoRight;
	ortho[2] = mOrthoBottom;
	ortho[3] = mOrthoTop;
	ortho[4] = mOrthoNear;
	ortho[5] = mOrthoFar;
}

// left, right, bottom, top, near, far
void Camera::frustumProjection(double* frustum)
{
	frustum[0] = mFrustumLeft;
	frustum[1] = mFrustumRight;
	frustum[2] = mFrustumBottom;
	frustum[3] = mFrustumTop;
	frustum[4] = mFrustumNear;
	frustum[5] = mFrustumFar;
}
void Camera::frustumProjection(double* frustum)const
{
	frustum[0] = mFrustumLeft;
	frustum[1] = mFrustumRight;
	frustum[2] = mFrustumBottom;
	frustum[3] = mFrustumTop;
	frustum[4] = mFrustumNear;
	frustum[5] = mFrustumFar;
}

double Camera::zoomfactor()const
{
	return mZoomFactor;
}
double Camera::zoomfactor()
{
	return mZoomFactor;
}

void Camera::computeFromMouse(int x, int y, int mouseKey, int state)
{

}

void Camera::apply()
{
	if (Camera::ProjectionType::Perspective == mProjectionType)
	{
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		//mProjectionMatrix = glm::perspective((float)mFieldOfView * (float)mZoomFactor, 4.0f / 3.0f, 0.1f, 10000.0f);


		mProjectionMatrix = glm::frustum(mFrustumLeft * mZoomFactor, mFrustumRight * mZoomFactor, mFrustumBottom * mZoomFactor, mFrustumTop * mZoomFactor, mFrustumNear, mFrustumFar);

		// Camera matrix
		mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
		mModelMatrix = mArcBall.transformationMatrix() * glm::translate(glm::mat4(1.0f), -mBBox.center());

		// Our ModelViewProjection : multiplication of our 3 matrices
		mMVP = mProjectionMatrix * mViewMatrix * mModelMatrix; // Remember, matrix multiplication is the other way around
	}
	else if (Camera::ProjectionType::Parallel == mProjectionType)
	{
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		mProjectionMatrix = glm::ortho(mOrthoLeft * mZoomFactor, mOrthoRight * mZoomFactor, mOrthoBottom * mZoomFactor, mOrthoTop * mZoomFactor, mOrthoNear, mOrthoFar);
		// Camera matrix
		mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
		mModelMatrix = mArcBall.transformationMatrix() * glm::translate(glm::mat4(1.0f), -mBBox.center());
		// Our ModelViewProjection : multiplication of our 3 matrices
		mMVP = mProjectionMatrix * mViewMatrix * mModelMatrix; // Remember, matrix multiplication is the other way around
	}
}

Camera::ProjectionType Camera::projectionType()
{
	return mProjectionType;
}

Camera::ProjectionType Camera::projectionType()const
{
	return mProjectionType;
}

double Camera::FOV()
{
	return mFieldOfView;
}

double Camera::FOV()const
{
	return mFieldOfView;
}

glm::vec3 Camera::position()
{
	return mPosition;
}

const glm::vec3 & Camera::position()const
{
	return mPosition;
}

glm::vec3 Camera::upVector()
{
	return mUpVector;
}

const glm::vec3 & Camera::upVector()const
{
	return mUpVector;
}

glm::vec3 Camera::lookAt()
{
	return mLookAt;
}

const glm::vec3 & Camera::lookAt()const
{
	return mLookAt;
}

glm::mat4 Camera::MVP()
{
	return mMVP;
}

glm::mat4 Camera::viewMatrix()
{
	return mViewMatrix;
}
const glm::mat4 & Camera::viewMatrix()const
{
	return mViewMatrix;
}

glm::mat4 Camera::modelMatrix()
{
	return mModelMatrix;
}
const glm::mat4 & Camera::modelMatrix()const
{
	return mModelMatrix;
}

glm::mat4 Camera::projectionMatrix()
{
	return mProjectionMatrix;
}
const glm::mat4 & Camera::projectionMatrix()const
{
	return mProjectionMatrix;
}

glm::mat4 Camera::normalMatrix()
{
	return glm::transpose(glm::inverse(mViewMatrix*mModelMatrix));
}
const glm::mat4 & Camera::normalMatrix()const
{
	return glm::transpose(glm::inverse(mViewMatrix*mModelMatrix));
}

const glm::mat4 & Camera::MVP()const
{
	return mMVP;
}

void Camera::setSize(const BoundingBox & bbox)
{
	mBBox = bbox;
	if (Camera::ProjectionType::Perspective == mProjectionType)
	{
		glm::vec3 center = bbox.center();
		double distance = 0.5*glm::sqrt(bbox.diagonal().y*bbox.diagonal().y + bbox.diagonal().x + bbox.diagonal().x) / glm::tan(mFieldOfView / 2.0f);
		mLookAt = glm::vec3(0, 0, 0);
		mPosition = glm::vec3(mLookAt.x, mLookAt.y, mLookAt.z + distance);
		mUpVector = glm::vec3(0, 1, 0);
	}
	else if (Camera::ProjectionType::Parallel == mProjectionType)
	{
		glm::vec3 center = bbox.center();
		glm::vec3 lower = bbox.lower();
		glm::vec3 upper = bbox.upper();
		double distance = glm::sqrt(bbox.diagonal().y*bbox.diagonal().y + bbox.diagonal().x + bbox.diagonal().x);

		double ar = float(mWidth) / mHeight;

		mOrthoLeft = 0 - distance;
		mOrthoRight = 0 + distance;
		mOrthoTop = 0 + distance / ar;
		mOrthoBottom = 0 - distance / ar;
		mOrthoNear = 0 - 2 * distance;
		mOrthoFar = 0 + 2 * distance;

		mLookAt = glm::vec3(0, 0, 0);;
		mPosition = glm::vec3(mLookAt.x, mLookAt.y, mLookAt.z + distance);
		mUpVector = glm::vec3(0, 1, 0);
	}
	apply();
}
