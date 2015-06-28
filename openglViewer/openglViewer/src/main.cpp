#include "stdafx.h"
#include "Parser.h"
#include "NTreeNode.h"
#include "Scene.h"

using namespace std;

shared_ptr<glv::Scene> gScene;

int main()
{
	gScene = make_shared<glv::Scene>();

	//// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	//// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	//float g_vertex_buffer_data[] = {
	//	0.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//};

	//int g_index_buffer_data[] = {
	//	0, 1, 2
	//};

	//// One color for each vertex. They were generated randomly.
	//float g_color_buffer_data[] = {
	//	0.583f, 0.771f, 0.014f,
	//	0.609f, 0.115f, 0.436f,
	//	0.327f, 0.483f, 0.844f,
	//};

	//shared_ptr<glv::MeshDrawable> cube = make_shared<glv::MeshDrawable>();
	//cube->setVertexArray(g_vertex_buffer_data, sizeof(g_vertex_buffer_data) / sizeof(float));
	//cube->setColorArray(g_color_buffer_data, sizeof(g_color_buffer_data) / sizeof(float));
	//cube->setIndexArray(g_index_buffer_data, sizeof(g_index_buffer_data) / sizeof(int));
	//cube->setAutoNormals(true);

	//scene->addCamera(make_shared<glv::Camera>());

	glv::Parser parser;

	auto torusMesh = parser.read("C:/TK/CCTechViewer/CCTechViewer/TestFiles/STL/ASCII/torus.stl", glv::Parser::STL);
	glv::MeshDrawableSharedPtr torusMeshPtr{ std::move(torusMesh) };
	{
		glm::vec3 center = torusMeshPtr->geomPackBBox().center();
		glm::mat4 meshTransform = glm::translate(torusMeshPtr->transform(), -center);
		torusMeshPtr->setTransform(meshTransform);

		torusMeshPtr->setMaterial(glv::Material(glm::vec3(0, 0, 1.0)));
	}

	auto cubeMesh = parser.read("C:/TK/CCTechViewer/CCTechViewer/TestFiles/STL/ASCII/cube.stl", glv::Parser::STL);
	glv::MeshDrawableSharedPtr cubeMeshPtr{ std::move(cubeMesh) };
	{
		glm::vec3 center = cubeMeshPtr->geomPackBBox().center();
		glm::mat4 meshTransform = glm::translate(cubeMeshPtr->transform(), glm::vec3(5.0f, 0, 0));
		cubeMeshPtr->setTransform(meshTransform);

		glv::Material texMaterial;
		texMaterial.applyTexture("C:/Users/Tikeshwar/Desktop/chessboard.bmp");

		cubeMeshPtr->setMaterial(texMaterial);
		//cubeMeshPtr->setMaterial(glv::Material(glm::vec3(1.0, 1.0, 1.0)));
	}

	glv::Utility util;

	auto rootdsp = util.getDrawableNode(torusMeshPtr);
	auto childdsp = util.getDrawableNode(cubeMeshPtr);

	util.connect(rootdsp, childdsp);

	gScene->addDrawableNode(rootdsp);

	gScene->addCamera(make_shared<glv::Camera>(
		//glm::vec3(4, 3, -3),
		//glm::vec3(0, 1, 0),
		//glm::vec3(0, 0, 0),
		//glv::Camera::Parallel
		));

	gScene->addLight(make_shared<glv::Light>("default", glm::vec3(1.0), glm::vec3(0, 0, 0)));

	cout << gScene->boundingBox().center().x;
	cout << gScene->boundingBox().center().y;
	cout << gScene->boundingBox().center().z;

	gScene->render();


	/*
	typedef glv::NTreeNode<int> IntNode;

	{
	IntNode::NodeSharedPtr p(make_shared<IntNode::Node>(1));
	IntNode::NodeSharedPtr c1(make_shared<IntNode::Node>(2));
	IntNode::NodeSharedPtr c2(make_shared<IntNode::Node>(3));

	p->addChild(c1);
	p->addChild(c2);

	p->traverse([](const IntNode::NodeSharedPtr & node){
	cout << node->data() << endl;
	});

	} */
	return 0;
}
