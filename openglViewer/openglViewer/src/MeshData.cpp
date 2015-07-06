#include "Stdafx.h"
#include "MeshData.h"

using namespace glv;

MeshData::MeshData()
: mIsIndexed(true)
{
}

MeshData::MeshData(std::vector<glm::vec3> & vertexArray,
	std::vector<glm::vec3> & indexArray,
	std::vector<glm::vec3> & normalArray,
	std::vector<glm::vec3> & colorArray,
	std::vector<glm::vec2> & uvArray)
	: mIsIndexed(true)
{
	this->vertexArray = vertexArray;
	this->indexArray = indexArray;
	this->normalArray = normalArray;
	this->colorArray = colorArray;
	this->uvArray = uvArray;
}

MeshData::~MeshData()
{
}

void MeshData::createNonIndexedMesh(MeshData & outPutMesh)
{
	mIsIndexed = false;
	outPutMesh.vertexArray.assign(vertexArray.begin(), vertexArray.end());

	outPutMesh.indexArray.resize(vertexArray.size() / 3);
	for (int i = 0; i < vertexArray.size() / 3; i++)
	{
		outPutMesh.indexArray[i] = glm::vec3(3 * i + 0, 3 * i + 1, 3 * i + 2);
	}

	outPutMesh.normalArray.resize(vertexArray.size());
	for (int i = 0; i < outPutMesh.vertexArray.size() / 3; i++)
	{
		outPutMesh.normalArray[3 * i + 0] = normalArray[i];
		outPutMesh.normalArray[3 * i + 1] = normalArray[i];
		outPutMesh.normalArray[3 * i + 2] = normalArray[i];
	}

	outPutMesh.colorArray.clear();
	outPutMesh.colorArray.resize(outPutMesh.vertexArray.size());

	for (int i = 0; i < outPutMesh.colorArray.size(); i++)
		outPutMesh.colorArray[i] = glm::vec3(1.0, 0.0, 0.0);
}

void MeshData::createIndexedMesh(MeshData & outPutMesh)
{
	mIsIndexed = true;

	createIndexes(outPutMesh);

	std::map<unsigned int, glm::vec3 > vertex2NormalMap;
	calculatePhongNormals(outPutMesh.indexArray, vertex2NormalMap);

	outPutMesh.normalArray.clear();
	outPutMesh.normalArray.resize(outPutMesh.vertexArray.size());

	for (auto it = vertex2NormalMap.begin(); it != vertex2NormalMap.end(); it++)
	{
		outPutMesh.normalArray[it->first] = it->second;
	}

	outPutMesh.colorArray.clear();
	outPutMesh.colorArray.resize(outPutMesh.vertexArray.size());

	for (int i = 0; i < outPutMesh.colorArray.size(); i++)
		outPutMesh.colorArray[i] = glm::vec3(1.0, 0.0, 0.0);
}

void MeshData::createIndexes(MeshData & outPutMesh)
{
	class LessThan
	{
	public:
		bool operator()(const glm::vec3 & p1, const glm::vec3 & p2)const
		{
			if (p1[0] < (p2[0] - _MFR_LOWER_TOL_))
				return true;
			else if (fabs(p1[0] - p2[0]) < _MFR_LOWER_TOL_)
			{
				if (p1[1] < (p2[1] - _MFR_LOWER_TOL_))
					return true;
				else if (fabs(p1[1] - p2[1]) < _MFR_LOWER_TOL_)
				{
					if (p1[2] < (p2[2] - _MFR_LOWER_TOL_))
						return true;
				}
			}
			return false;
		}
	};

	std::map<glm::vec3, unsigned int, LessThan> vertex2indexMap;

	int triVertex = 0;
	glm::vec3 triangle;

	for (int i = 0; i < vertexArray.size(); i++)
	{
		auto itPair = vertex2indexMap.insert(std::pair<glm::vec3, unsigned int>(vertexArray[i], outPutMesh.vertexArray.size()));
		if (itPair.second)
			outPutMesh.vertexArray.push_back(itPair.first->first);

		triangle[triVertex++] = itPair.first->second;
		if (triVertex == 3)
		{
			triVertex = 0;
			outPutMesh.indexArray.push_back(triangle);
		}
	}
}

void MeshData::calculatePhongNormals(const std::vector<glm::vec3> & indexArray,
	std::map<unsigned int, glm::vec3> & vertex2NormalMap)
{

	std::map<unsigned int, std::vector<unsigned int>> vertex2TrianglesMap;
	for (int i = 0; i < indexArray.size(); i++)
	{
		auto itPair = vertex2TrianglesMap.insert(std::pair<unsigned int, std::vector<unsigned int>>(indexArray[i].x, std::vector<unsigned int>()));
		itPair.first->second.push_back(i);

		itPair = vertex2TrianglesMap.insert(std::pair<unsigned int, std::vector<unsigned int>>(indexArray[i].y, std::vector<unsigned int>()));
		itPair.first->second.push_back(i);

		itPair = vertex2TrianglesMap.insert(std::pair<unsigned int, std::vector<unsigned int>>(indexArray[i].z, std::vector<unsigned int>()));
		itPair.first->second.push_back(i);
	}

	for (auto it = vertex2TrianglesMap.begin(); it != vertex2TrianglesMap.end(); it++)
	{
		glm::vec3 phongNormal;
		std::vector<unsigned int> neighborTriangle = it->second;
		double threshold = glm::cos(glm::radians(20.0f));
		int triCount = 0;
		for (int nt = 0; nt < neighborTriangle.size(); nt++)
		{
			//float dot = glm::dot(normalArray[neighborTriangle[glm::max(nt - 1, 0)]], normalArray[neighborTriangle[nt]]);
			//if (dot > threshold)
			{
				phongNormal.x = phongNormal.x + normalArray[neighborTriangle[nt]].x;
				phongNormal.y = phongNormal.y + normalArray[neighborTriangle[nt]].y;
				phongNormal.z = phongNormal.z + normalArray[neighborTriangle[nt]].z;

				triCount++;
			}
			//else
			//break;
		}

		phongNormal.x = phongNormal.x / triCount;
		phongNormal.y = phongNormal.y / triCount;
		phongNormal.z = phongNormal.z / triCount;

		vertex2NormalMap.insert(std::pair<unsigned int, glm::vec3>(it->first, phongNormal));
	}
}

