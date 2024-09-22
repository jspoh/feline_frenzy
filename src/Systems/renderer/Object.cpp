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


Object::Object(const std::string& mdl, const std::string& shdr, Vector2 pos, Vector2 scl, float rot, Matrix33::Matrix_33 xform)
	: model_ref(mdl), shader_ref(shdr), position(pos), scale(scl), rotation(rot), mdl_to_ndc_xform(xform) {}

std::string Object::getModelRef() const {
	return model_ref;
}
std::string Object::getShaderRef() const {
	return shader_ref;
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

void Object::setXform(Matrix33::Matrix_33 xform) {
	mdl_to_ndc_xform = xform;
}

Matrix33::Matrix_33 Object::getXform() const {
	return mdl_to_ndc_xform;
}


void Object::update(float dt) {

	Matrix33::Matrix_33 model_mat, world_to_ndc_mat, trans_rot_mat, scale_mat;
	// Scaling matrix
	scale_mat = Matrix33::Matrix_33{
		scale.x, 0, 0,
		0, scale.y, 0,
		0,         0, 1,
	};

	// !TODO add orientation
	float angleDisp = rotation;
	if (rotation != 0) {

		rotation += 0.01;
	}
	// Translate and rotate matrix
	trans_rot_mat = Matrix33::Matrix_33{
		cos(angleDisp), sin(angleDisp), 0,
		-sin(angleDisp), cos(angleDisp),  0,
		position.x, position.y, 1,
	};

	model_mat = trans_rot_mat * scale_mat;


	mdl_to_ndc_xform = model_mat;
}

void Object::draw() const {


	auto it = RenderManager::getInstance().models.find(model_ref);

	it->second->draw();

}
