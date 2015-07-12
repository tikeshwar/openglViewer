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
	class ReflectionEffect;
	class ShadowEffect;
	class NormalSceneEffect;

	typedef std::shared_ptr<Drawable> DrawableSharedPtr;
	typedef std::shared_ptr<MeshDrawable> MeshDrawableSharedPtr;
	typedef NTreeNode<DrawableSharedPtr> DrawableNode;
	typedef std::shared_ptr<DrawableNode> DrawableNodeSharedPtr;

	typedef std::shared_ptr<Camera> CameraSharedPtr;
	typedef std::shared_ptr<Light> LightSharedPtr;
	typedef std::shared_ptr<ReflectionEffect> ReflectionEffectSharedPtr;
	typedef std::shared_ptr<ShadowEffect> ShadowEffectSharedPtr;
	typedef std::shared_ptr<NormalSceneEffect> NormalSceneEffectSharedPtr;
}