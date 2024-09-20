/*****************************************************************//**
 * \file   Object.cpp
 * \brief  
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Object.h"
#include "RenderManager.h"


Object::Object(const std::string& mdl, Vector2 pos, Vector2 scl, float rot, glm::mat3 xform)
	: model_ref(mdl), position(pos), scale(scl), rotation(rot), mdl_to_ndc_xform(xform) {}

std::string Object::getModelRef() const {
	return model_ref;
}

void Object::setPosition(const Vector2& pos) {
	position = pos;
}

Vector2 Object::getPosition() const{
	return position;
}

void Object::setScale(const Vector2& scl){
	scale = scl;
}

Vector2 Object::getScale() const {
	return scale;
}

void Object::setRot(float rot) {
	rotation = rot;
}

float Object::getRot() const {
	return rotation;
}

void Object::setXform(glm::mat3 xform) {
	mdl_to_ndc_xform = xform;
}

glm::mat3 Object::getXform() const {
	return mdl_to_ndc_xform;
}


void Object::update(float dt) {
	// empty for now
}

void Object::draw() const {

	auto it = RenderManager::getInstance().models.find(model_ref);

	it->second->draw();

}
