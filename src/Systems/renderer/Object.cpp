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


Object::Object(const std::string& mdl, Vector2 pos, Vector2 scl, float rot)
	: model_ref(mdl), position(pos), scale(scl), rotation(rot) {}

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

void Object::update(float dt) {
	// empty for now
}

void Object::draw() const {
	RenderManager::getInstance().drawModel(model_ref);
}
