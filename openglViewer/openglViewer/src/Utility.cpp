#include "Stdafx.h"
#include "Utility.h"

using namespace glv;

Utility::Utility()
{
}

Utility::~Utility()
{
}

glv::DrawableNodeSharedPtr Utility::getDrawableNode(glv::DrawableSharedPtr drawable)
{
	return std::make_shared<glv::DrawableNode>(drawable);
}

void Utility::connect(glv::DrawableNodeSharedPtr parent, glv::DrawableNodeSharedPtr child)
{
	parent->addChild(child);

	traversalUpdateBBox(parent);
	traversalUpdateTransform(parent);
}

void Utility::connect(glv::DrawableNodeSharedPtr parent, glv::DrawableSharedPtr child)
{
	glv::DrawableNodeSharedPtr drawableNode = getDrawableNode(child);
	connect(parent, drawableNode);
}

void Utility::traversalUpdateBBox(glv::DrawableNodeSharedPtr node)
{
	glv::BoundingBox nodeBox = node->data()->geomPackBBox();
	for (auto it = node->begin(); it != node->end(); it++)
	{
		(*it)->traverse([&](const DrawableNodeSharedPtr & node)
		{
			if (node->data()->ifIncludedInBBoxCalculation())
				nodeBox.extend(node->data()->geomPackBBox());
		});
	}
	node->data()->setGeomPackBBox(nodeBox);
}

void Utility::traversalUpdateTransform(glv::DrawableNodeSharedPtr node)
{
	glm::mat4 nodeTransform = node->data()->transform();
	for (auto it = node->begin(); it != node->end(); it++)
	{
		(*it)->traverse([&](const DrawableNodeSharedPtr & node)
		{
			if (node->data()->ifIncludedInSettingTransform())
			{
				glm::mat4 mat = nodeTransform * node->data()->transform();
				node->data()->setTransform(mat);
			}
		});
	}
}

glv::BoundingBox Utility::updateBBox(const glv::BoundingBox & box, const glm::mat4 & mat)
{
	glm::vec3 diagonal = box.diagonal();

	glm::vec3 blb = box.lower();
	glm::vec3 frt = box.upper();
	glm::vec3 flt = box.lower() + glm::vec3(0, diagonal.y, diagonal.z);
	glm::vec3 brb = box.upper() + glm::vec3(0, -diagonal.y, -diagonal.z);

	glm::vec4 nblb = mat*glm::vec4(blb, 1.0);
	glm::vec4 nfrt = mat*glm::vec4(frt, 1.0);
	glm::vec4 nflt = mat*glm::vec4(flt, 1.0);
	glm::vec4 nbrb = mat*glm::vec4(brb, 1.0);

	glv::BoundingBox nbox;
	nbox.extend(glm::vec3(nblb));
	nbox.extend(glm::vec3(nfrt));
	nbox.extend(glm::vec3(nflt));
	nbox.extend(glm::vec3(nbrb));

	return nbox;
}

