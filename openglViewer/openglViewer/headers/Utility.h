#pragma once

namespace glv
{
	class Utility
	{
	public:
		Utility();
		~Utility();

		glv::DrawableNodeSharedPtr getDrawableNode(glv::DrawableSharedPtr drawable);

		void connect(glv::DrawableNodeSharedPtr parent, glv::DrawableNodeSharedPtr child);
		void connect(glv::DrawableNodeSharedPtr parent, glv::DrawableSharedPtr child);

		void traversalUpdateBBox(glv::DrawableNodeSharedPtr drawableNode);
		void traversalUpdateTransform(glv::DrawableNodeSharedPtr drawableNode);

		glv::BoundingBox updateBBox(const glv::BoundingBox & box, const glm::mat4 & mat);

		glm::vec3 reflect(const glm::vec4 & plane, const glm::vec3 point);
	};
}

