#pragma once

namespace glv
{
	class MeshDrawable :public Drawable
	{
	public:
		MeshDrawable();
		virtual ~MeshDrawable();

		const char* name()const;
		const char* name();
		void setName(const char* name);

		bool isVisible()const;
		bool isVisible();
		void setVisible(bool isVisible);

		void setVertexArray(const float* vertexArray, int arraySize);
		void setIndexArray(const int* indexArray, int arraySize);
		void setColorArray(const float* colorArray, int arraySize);
		void setNormalArray(const float* normalArray, int arraySize);
		void setUVArray(const float* uvArray, int arraySize);

		void setVertexArray(const std::vector<glm::vec3> & vertexArray);
		void setIndexArray(const std::vector<glm::vec3> & indexArray);
		void setColorArray(const std::vector<glm::vec3> & colorArray);
		void setNormalArray(const std::vector<glm::vec3> & normalArray);
		void setUVArray(const std::vector<glm::vec2> & uvArray);

		void setAutoNormals(bool bAutoNormals);

		const BoundingBox & geomPackBBox()const ;
		BoundingBox geomPackBBox();

		const BoundingBox & geomBBox()const;
		BoundingBox geomBBox();

		void setGeomBBox(glv::BoundingBox bbox);
		void setGeomPackBBox(glv::BoundingBox bbox);

		const glm::mat4 & transform()const;
		glm::mat4 transform();
		void setTransform(const glm::mat4 & mat);

		const Material & material()const;
		Material material();
		void setMaterial(const Material & material);

		void draw();

	private:
		void calculateBBox();
		void setBufferObject(GLuint* buffer, const void* dataArray, int count, int sizeofData);
		void copyArray(const std::vector<glm::vec3> & inputArray, std::vector<float> & outputArray);
		void copyArray(const std::vector<glm::vec2> & inputArray, std::vector<float> & outputArray);
		void copyArray(const std::vector<glm::vec3> & inputArray, std::vector<unsigned int> & outputArray);

	private:
		std::vector<float> mVertexArray;
		std::vector<unsigned int> mIndexArray;
		std::vector<float> mColorArray;
		std::vector<float> mNormalArray;
		std::vector<float> mUVArray;

		GLuint mVertexBuffer;
		GLuint mIndexBuffer;
		GLuint mColorBuffer;
		GLuint mNormalBuffer;
		GLuint mUVBuffer;

		bool mAutoNormals;
		bool mIsVisible;
		mutable BoundingBox mGeomBBox;
		mutable BoundingBox mGeomPackBBox;

		glm::mat4 mTransformMat;

		std::string mName;
		glv::Material mMaterial;
	};
}

