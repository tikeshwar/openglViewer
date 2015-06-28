#pragma once

#include <memory>

namespace glv
{
	template <typename T> class NTreeNode;
	class Drawable;
	class MeshDrawable;
	class Camera;
	class Light;
	class Scene;

	typedef std::shared_ptr<Drawable> DrawableSharedPtr;
	typedef std::shared_ptr<MeshDrawable> MeshDrawableSharedPtr;
	typedef NTreeNode<DrawableSharedPtr> DrawableNode;
	typedef std::shared_ptr<DrawableNode> DrawableNodeSharedPtr;

	typedef std::shared_ptr<Camera> CameraSharedPtr;
	typedef std::shared_ptr<Light> LightSharedPtr;
}