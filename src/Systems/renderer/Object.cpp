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

using namespace Matrix33;


Object::Object(const std::string& mdl, const std::string& shdr, Vector3 clr, Vector2 pos, Vector2 scl, float ori, float rot, float vel, Matrix33::Matrix_33 xform)
	: model_ref(mdl), shader_ref(shdr), color(clr), position(pos), scale(scl), orientation(ori), rotation(rot), velocity(vel), mdl_to_ndc_xform(xform) {}

std::string Object::getModelRef() const {
	return model_ref;
}
std::string Object::getShaderRef() const {
	return shader_ref;
}

void Object::setColor(const Vector3& clr) {
	color = clr;
}

Vector3 Object::getColor() const {
	return color;
}

void Object::setPosition(float x, float y) {
	position = Vector2(x,y);
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

void Object::setVelocity(float vel) {
	velocity = vel;
}

float Object::getVelocity() const {
	return velocity;
}

void Object::setRotation(float rot) {
	rotation = rot;
}

float Object::getRotation() const {
	return rotation;
}

void Object::setOrientation(float ori) {
	orientation = ori;
}

float Object::getOrientation() const{
	return orientation;
}

void Object::setXform(Matrix33::Matrix_33 xform) {
	mdl_to_ndc_xform = xform;
}

Matrix33::Matrix_33 Object::getXform() const {
	return mdl_to_ndc_xform;
}

void Object::update(float dt, const Matrix33::Matrix_33& cam_to_ndc_xform) {

	Matrix33::Matrix_33 model_mat, world_to_ndc_mat, result, scale_mat, rot_mat, trans_mat;
	
	// Modulus the object rotation so it doesnt result in a large number overtime
	orientation = fmod(orientation, 360.f);
	float angleDisp = (orientation += rotation * dt) * PI / 180;

	Matrix_33Rot(rot_mat, angleDisp);
	Matrix_33Scale(scale_mat, scale.x, scale.y);
	Matrix_33Translate(trans_mat, position.x, position.y);

	result = cam_to_ndc_xform * trans_mat * rot_mat * scale_mat;

	// OpenGL requires matrix in col maj so transpose
	Matrix_33Transpose(model_mat, result);

	mdl_to_ndc_xform = model_mat;
	
}

void Object::draw() const {

	auto it = RenderManager::getInstance().models.find(model_ref);
	if (it != RenderManager::getInstance().models.end()) {
		it->second->draw();
	}
	else {
		cerr << "Model not found: " << model_ref << endl;
	}
}
