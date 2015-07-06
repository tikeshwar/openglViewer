#include "stdafx.h"
#include "Parser.h"
#include "NTreeNode.h"
#include "Scene.h"

using namespace std;

shared_ptr<glv::Scene> gScene;

int main()
{
	gScene = make_shared<glv::Scene>();

	auto light = make_shared<glv::Light>("default", glm::vec3(1.0), glm::vec3(0, -75, 150));
	gScene->addLight(light);

	auto camera = make_shared<glv::Camera>();
	camera->setPosition(-100, -100, 100);
	camera->setUpVector(0, 0, 1.0);
	camera->setLookAt(0, 0, 0);
	gScene->addCamera(camera);

	glv::Parser parser;

	/*
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
	*/
	auto plyWoodMesh = parser.read("C:/TK/CCTechViewer/CCTechViewer/TestFiles/STL/ASCII/cube.stl", glv::Parser::STL);
	glv::MeshDrawableSharedPtr plyWoodMeshPtr{ std::move(plyWoodMesh) };
	{
		glm::vec3 center = plyWoodMeshPtr->geomPackBBox().center();
		glm::mat4 meshTransform = glm::translate(plyWoodMeshPtr->transform(), -center);
		plyWoodMeshPtr->setTransform(meshTransform);

		glv::Material texMaterial;
		texMaterial.applyTexture("C:/Users/Tikeshwar/Desktop/wood.bmp");

		plyWoodMeshPtr->setMaterial(glv::Material(glm::vec3(250.0f/255.0f, 235.0f/255.0f, 215.0f/255.0f)));
		//cubeMeshPtr->setMaterial(glv::Material(glm::vec3(1.0, 1.0, 1.0)));
	}

	auto lightMesh = parser.read("C:/TK/CCTechViewer/CCTechViewer/TestFiles/STL/ASCII/point_light_box.stl", glv::Parser::STL);
	glv::MeshDrawableSharedPtr lightMeshPtr{ std::move(lightMesh) };
	{
		glm::vec3 center = lightMeshPtr->geomPackBBox().center();
		glm::mat4 meshTransform = glm::translate(lightMeshPtr->transform(), light->position());
		lightMeshPtr->setTransform(meshTransform);

		lightMeshPtr->setMaterial(glv::Material(glm::vec3(1.0, 1.0, 0.0)));
		//cubeMeshPtr->setMaterial(glv::Material(glm::vec3(1.0, 1.0, 1.0)));
	}

	auto bendPipeMesh = parser.read("C:/TK/CCTechViewer/CCTechViewer/TestFiles/STL/ASCII/plane.stl", glv::Parser::STL);
	glv::MeshDrawableSharedPtr bendPipeMeshPtr{ std::move(bendPipeMesh) };
	{
		glm::vec3 center = plyWoodMeshPtr->geomPackBBox().center();
		glm::vec3 mesh = plyWoodMeshPtr->geomPackBBox().diagonal();
		glm::vec3 plane = bendPipeMeshPtr->geomPackBBox().diagonal();

		plane.x = plane.x < 0.000001 && plane.x > -0.000001 ? 1.0f : plane.x;
		plane.y = plane.y < 0.000001 && plane.y > -0.000001 ? 1.0f : plane.y;
		plane.z = plane.z < 0.000001 && plane.z > -0.000001 ? 1.0f : plane.z;

		glm::mat4 meshTransform = glm::translate(bendPipeMeshPtr->transform(), -center);
		meshTransform = glm::scale(meshTransform, glm::vec3(100* mesh.x / plane.x, 100*mesh.y / plane.y, 100*mesh.z / plane.z));
		meshTransform = glm::translate(bendPipeMeshPtr->transform(), glm::vec3(0, 0, 0));
		bendPipeMeshPtr->setTransform(meshTransform);
		bendPipeMeshPtr->includeInBBoxCalculation(false);
		bendPipeMeshPtr->includeInShadowCalculation(false);

		bendPipeMeshPtr->setMaterial(glv::Material("AllWhite", glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0)));
		bendPipeMeshPtr->preDraw([](){
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		});
		bendPipeMeshPtr->postDraw([](){
			glDisable(GL_CULL_FACE);
		});
	}

	glv::Utility util;

	auto rootdsp = util.getDrawableNode(plyWoodMeshPtr);
	//auto childdsp1 = util.getDrawableNode(lightMeshPtr);
	auto childdsp2 = util.getDrawableNode(bendPipeMeshPtr);

	//util.connect(rootdsp, childdsp1);
	util.connect(rootdsp, childdsp2);

	gScene->addDrawableNode(rootdsp);

	cout << gScene->boundingBox().center().x;
	cout << gScene->boundingBox().center().y;
	cout << gScene->boundingBox().center().z;

	gScene->render();

	return 0;
}
