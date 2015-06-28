#include "Stdafx.h"
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

unique_ptr<MeshDrawable> Parser::readStl(const char* file)
{
	if (!file)
		throw exception("invalid file name");

	ifstream stlFile(file);

	if (!stlFile.is_open())
		return 0;

	double x, y, z;
	char key[100];

	MeshData mesh;

	while (!stlFile.eof())
	{
		stlFile >> key;

		if (!strcmp(key, "normal"))
		{
			stlFile >> x >> y >> z;
			mesh.normalArray.push_back(glm::normalize(glm::vec3(x, y, z)));
		}
		if (!strcmp(key, "vertex"))
		{
			stlFile >> x >> y >> z;
			mesh.vertexArray.push_back(glm::vec3(x, y, z));
		}
	}

	stlFile.close();

	MeshData indexedMesh;
	indexedMesh.normalArray = mesh.normalArray;
	//mesh.createIndexedMesh(indexedMesh);
	mesh.createNonIndexedMesh(indexedMesh);

	auto geomMesh = make_unique<glv::MeshDrawable>();
	geomMesh->setVertexArray(indexedMesh.vertexArray);
	geomMesh->setIndexArray(indexedMesh.indexArray);
	geomMesh->setNormalArray(indexedMesh.normalArray);
	geomMesh->setColorArray(indexedMesh.colorArray);

	glv::BoundingBox box = geomMesh->geomBBox();

	indexedMesh.uvArray.resize(indexedMesh.vertexArray.size() / 3.0*2.0);
	for (int uv = 0; uv < indexedMesh.vertexArray.size() / 3.0; uv++)
	{
		indexedMesh.uvArray[2 * uv + 0].x = indexedMesh.vertexArray[3 * uv + 0].x;// / box.diagonal().x;
		indexedMesh.uvArray[2 * uv + 1].y = indexedMesh.vertexArray[3 * uv + 1].y;// / box.diagonal().y;
	}
	geomMesh->setUVArray(indexedMesh.uvArray);

	return geomMesh;
}



