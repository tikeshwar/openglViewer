#pragma once

#include <memory>
#include "NTreeNode.h"
#include "Camera.h"
#include "MeshDrawable.h"

namespace glv
{
	class Scene
	{
	public:
		Scene();
		Scene(int width, int height);
		~Scene();

		void addCamera(CameraSharedPtr camera);
		void addDrawable(DrawableSharedPtr drawable);
		void addDrawableNode(DrawableNodeSharedPtr drawableNode);

		void addLight(LightSharedPtr lightNode);
		void removeLight(const char* lightName);

		void render();

		void setWindowSize(int width, int height);
		const BoundingBox & boundingBox()const{ return mBBox; }
		BoundingBox boundingBox(){ return mBBox; }

		int width()const{ return mWidth; }
		int height()const{ return mHeight; }

		int setKeyCallback(int x, int y, int key, int action);
		int setMouseCallback(int x, int y, int button, int action, int mods);
		int setScrollCallback(int x, int y, double xoffet, double yoffet);

		CameraSharedPtr activeCamera(){ return mCamera; }

		void enableShadow(bool enable);
		bool isShadowEnabled()const;
		bool isShadowEnabled();

		void enableReflection(bool enable);
		bool isReflectionEnabled()const;
		bool isReflectionEnabled();

	private:
		DrawableNodeSharedPtr mRootNode;
		CameraSharedPtr mCamera;
		std::map<std::string, LightSharedPtr> mLights;

		GLFWwindow* mWindow;

		BoundingBox mBBox;

		int mWidth, mHeight;

		bool mIsShadowEnabled;
		bool mIsReflectionEnabled;

	};
}

