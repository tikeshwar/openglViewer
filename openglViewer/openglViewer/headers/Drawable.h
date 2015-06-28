#pragma once

namespace glv
{
	class Drawable
	{
	public:

		virtual const char* name()const = 0;
		virtual const char* name() = 0;
		virtual void setName(const char* name) = 0;

		virtual bool isVisible()const = 0;
		virtual bool isVisible() = 0;
		virtual void setVisible(bool isVisible) = 0;

		virtual const BoundingBox & geomPackBBox()const = 0;
		virtual BoundingBox geomPackBBox() = 0;

		virtual const BoundingBox & geomBBox()const = 0;
		virtual BoundingBox geomBBox() = 0;

		virtual void setGeomBBox(glv::BoundingBox bbox) = 0;
		virtual void setGeomPackBBox(glv::BoundingBox bbox) = 0;

		virtual const glm::mat4 & transform()const = 0;
		virtual glm::mat4 transform() = 0;
		virtual void setTransform(const glm::mat4 & mat) = 0;

		virtual void draw() = 0;

	};
}

