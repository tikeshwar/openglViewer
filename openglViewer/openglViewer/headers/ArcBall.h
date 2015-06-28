#pragma once

namespace glv {
	class ArcBall
	{
	public:
		ArcBall();
		ArcBall(int width, int height);
		~ArcBall();

		void setInitialized(bool initialized)
		{
			mInitialized = initialized;
		}
		void setWindowSize(int width, int height);
		void updateRotation(int curPosx, int curPosy, const glm::mat4 & inverseofMV);

		glm::mat4 transformationMatrix()const;
		void setToInitialState();

	private:
		glm::vec3 getPointOnArcBall(int x, int y);

	private:
		int mLastPosx, mLastPosy;
		int mWidth, mHeight;
		glm::dquat mRotQuat;
		bool mInitialized;
	};
}
