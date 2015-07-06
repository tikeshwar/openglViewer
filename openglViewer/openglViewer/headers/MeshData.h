#pragma once

#define _MFR_LOWER_TOL_ 0.0000001

namespace glv
{
	struct MeshData
	{
	public:
		MeshData();
		MeshData(std::vector<glm::vec3> & vertexArray,
			std::vector<glm::vec3> & indexArray,
			std::vector<glm::vec3> & normalArray,
			std::vector<glm::vec3> & colorArray,
			std::vector<glm::vec2> & uvArray);

		~MeshData();

		bool isIndexed()const
		{
			return mIsIndexed;
		}
		bool isIndexed()
		{
			return 	mIsIndexed;
		}

		void createNonIndexedMesh(MeshData & outPutMesh);
		void createIndexedMesh(MeshData & outPutMesh);

		void createIndexes(MeshData & outPutMesh);
		void calculatePhongNormals(const std::vector<glm::vec3> & indexArray,
			std::map<unsigned int, glm::vec3> & vertex2NormalMap);

	public:
		std::vector<glm::vec3> vertexArray;
		std::vector<glm::vec3> indexArray;
		std::vector<glm::vec3> normalArray;
		std::vector<glm::vec3> colorArray;
		std::vector<glm::vec2> uvArray;

		bool mIsIndexed;
	};
}

