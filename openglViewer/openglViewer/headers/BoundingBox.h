#pragma once

namespace glv{
	class BoundingBox
	{
	public:
		BoundingBox();
		BoundingBox(const glm::vec3 & lower, const glm::vec3 & upper);
		~BoundingBox();

		void extend(double x, double y, double z);
		void extend(const glm::vec3 & point);
		void extend(const BoundingBox & box);
		void expand(float scalarVal);

		bool intersect(const BoundingBox & box);
		bool intersect(const BoundingBox & box)const;

		bool contains(const glm::vec3 & point);
		bool contains(const glm::vec3 & point)const;

		glm::vec3 diagonal()const;

		const glm::vec3 & lower()const;
		glm::vec3 lower();
		const glm::vec3 & upper()const;
		glm::vec3 upper();

		glm::vec3 center()const;

		bool initialized();
		bool initialized()const;

	private:
		glm::vec3 mLower;
		glm::vec3 mUpper;
		bool mInitialized;
	};
}

