#include "Stdafx.h"
#include <array>
#include "MeshDrawable.h"
#include "Parser.h"

using namespace glv;
using namespace std;

Parser::Parser()
{
}

Parser::~Parser()
{
}

unique_ptr<MeshDrawable> Parser::read(const char* file, FILE_FORMAT format)
{
	switch (format)
	{
	case Parser::STL:
		return readStl(file);
		break;
	default:
		break;
	}
}

unique_ptr<MeshDrawable> Parser::readStl(const char* file) throw (exception)
{
	if (!file)
		throw exception("invalid file name");

	std::shared_ptr<MeshData> mesh;
	if (isStlAscii(file))
		mesh = readStlAscii(file);
	else
		mesh = readStlBinary(file);

	MeshData indexedMesh;
	indexedMesh.normalArray = mesh->normalArray;
	//mesh->createIndexedMesh(indexedMesh);
	mesh->createNonIndexedMesh(indexedMesh);

	auto geomMesh = make_unique<glv::MeshDrawable>();
	geomMesh->setVertexArray(indexedMesh.vertexArray);
	geomMesh->setIndexArray(indexedMesh.indexArray);
	geomMesh->setNormalArray(indexedMesh.normalArray);
	geomMesh->setColorArray(indexedMesh.colorArray);

	glv::BoundingBox box = geomMesh->geomBBox();

	if (mesh->isIndexed() == false)
	{
		indexedMesh.uvArray.resize(indexedMesh.vertexArray.size() / 3.0*2.0);
		for (int uv = 0; uv < indexedMesh.vertexArray.size() / 3.0; uv++)
		{
			indexedMesh.uvArray[2 * uv + 0].x = indexedMesh.vertexArray[3 * uv + 0].x;// / box.diagonal().x;
			indexedMesh.uvArray[2 * uv + 1].y = indexedMesh.vertexArray[3 * uv + 1].y;// / box.diagonal().y;
		}
	}
	else
	{
		indexedMesh.uvArray.resize(indexedMesh.vertexArray.size());
		for (int uv = 0; uv < indexedMesh.vertexArray.size(); uv++)
		{
			indexedMesh.uvArray[uv].x = indexedMesh.vertexArray[uv].x;// / box.diagonal().x;
			indexedMesh.uvArray[uv].y = indexedMesh.vertexArray[uv].y;// / box.diagonal().y;
		}
	}
	geomMesh->setUVArray(indexedMesh.uvArray);

	return geomMesh;
}

bool Parser::isStlAscii(const char* file)const throw(exception)
{
	char header[80];

	ifstream stlFile(file, ios::in);
	if (stlFile.is_open())
	{
		stlFile >> header;
		std::string str(header);
		stlFile.close();
		return string::npos != str.find("SOLID") || string::npos != str.find("solid");
	}
	else
		throw exception("invalid file name");

}
std::shared_ptr<MeshData> Parser::readStlAscii(const char* file)const
{
	ifstream stlFile(file);

	double x, y, z;
	char key[100];

	std::shared_ptr<MeshData> mesh = std::make_shared<MeshData>();

	while (!stlFile.eof())
	{
		stlFile >> key;

		if (!strcmp(key, "normal"))
		{
			stlFile >> x >> y >> z;
			mesh->normalArray.push_back(glm::normalize(glm::vec3(x, y, z)));
		}
		if (!strcmp(key, "vertex"))
		{
			stlFile >> x >> y >> z;
			mesh->vertexArray.push_back(glm::vec3(x, y, z));
		}
	}

	stlFile.close();

	return mesh;
}
std::shared_ptr<MeshData> Parser::readStlBinary(const char* file)const
{
	ifstream stlFile(file, ios::in|ios::binary);

	unsigned long triangleCount = 0;
	char header[80];
	char key[4];
	char xyz[50];

	std::shared_ptr<MeshData> mesh = std::make_shared<MeshData>();

	// read header 
	stlFile.read(header, 80);

	// read number of triangles
	stlFile.read(key, 4);
	triangleCount = *(unsigned long*)(&key);

	while (triangleCount--)
	{
		// Read nornals
		stlFile.read(xyz, 50);
		mesh->normalArray.push_back(glm::normalize(glm::vec3(*(float*)(xyz), *(float*)(xyz + 4), *(float*)(xyz + 8))));

		// Read vertices
		for (int t = 0; t < 3; t++)
		{
			mesh->vertexArray.push_back(glm::vec3(*(float*)(xyz + 12*t + 12), *(float*)(xyz + 12*t + 16), *(float*)(xyz + 12*t + 20)));
		}

		// skip colors for now
	}

	stlFile.close();

	return 	mesh;
}





