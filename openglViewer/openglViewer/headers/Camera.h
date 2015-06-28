#pragma once

namespace glv
{
	class Camera
	{
	public:
		enum ProjectionType
		{
			Perspective = 0,
			Parallel
		};

	public:
		Camera();
		Camera(const glm::vec3 & position, const glm::vec3 & upVector, const glm::vec3 & lookAt,
			ProjectionType projectionType = ProjectionType::Perspective, float fov = 30.0f);
		~Camera();

		void initProjection();

		void resetView();
		void resetView(ProjectionType projectionType);

		void setProjectionType(ProjectionType projectionType);
		void setPosition(double px, double py, double pz);
		void setUpVector(double ux, double uy, double uz);
		void setLookAt(double lx, double ly, double lz);
		void setFOV(double angleInRads);
		void setZoom(double zoomfactor);

		void setWindowSize(int width, int height);
		void setSize(const BoundingBox & bbox);

		int setKeyCallback(int x, int y, int key, int action);
		int setMouseMoveCallback(int x, int y);
		int setMouseButtonCallback(int x, int y, int button, int action, int mods);
		int setScrollCallback(int x, int y, double xoffet, double yoffet);

		void updateRotation(int curPosx, int curPosy);
		void updateZoom(double xOffset, double yOffset);
		void updatePan(int curPosx, int curPosy);

		// left, right, bottom, top, near, far
		void orthoProjection(double* ortho);
		void orthoProjection(double* ortho)const;

		// left, right, bottom, top, near, far
		void frustumProjection(double* frustum);
		void frustumProjection(double* frustum)const;

		double zoomfactor()const;
		double zoomfactor();

		// mouseKey - 1 for left, 2 for middle, 3 for right, 0 for none
		// state - 1 for down, 2 for up, 0 for none
		void computeFromMouse(int x, int y, int mouseKey, int state);

		void apply();

		ProjectionType projectionType();
		ProjectionType projectionType()const;

		double FOV();
		double FOV()const;

		glm::vec3 position();
		const glm::vec3 & position()const;

		glm::vec3 upVector();
		const glm::vec3 & upVector()const;

		glm::vec3 lookAt();
		const glm::vec3 & lookAt()const;

		glm::mat4 MVP();
		const glm::mat4 & MVP()const;

		glm::mat4 viewMatrix();
		const glm::mat4 & viewMatrix()const;

		glm::mat4 modelMatrix();
		const glm::mat4 & modelMatrix()const;

		glm::mat4 projectionMatrix();
		const glm::mat4 & projectionMatrix()const;

		glm::mat4 normalMatrix();
		const glm::mat4 & normalMatrix()const;

		int width()const{ return mWidth; }
		int height()const{ return mHeight; }

	private:
		ProjectionType mProjectionType;
		double mFieldOfView;
		glm::vec3 mPosition;
		glm::vec3 mUpVector;
		glm::vec3 mLookAt;

		glm::mat4 mMVP, mViewMatrix, mModelMatrix, mProjectionMatrix;
		BoundingBox mBBox;
		int mWidth;
		int	mHeight;

		int mLastx, mLasty;

		double mOrthoLeft, mOrthoRight, mOrthoTop, mOrthoBottom, mOrthoNear, mOrthoFar;
		double mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom, mFrustumNear, mFrustumFar;

		double mZoomFactor;
		ArcBall mArcBall;
	};
}

