/*****************************************************************//**
 * \file   Object.h
 * \brief  An object is a model with attributes
 *
 * \author g.boonxuensean
 * \date   September 2024
 *********************************************************************/

#pragma once


#include "stdafx.h"
#include "Model.h"


class Object {
	private:
		std::string model_ref;
		std::string shader_ref;

		Vector3 color;
		Vector2 position;
		Vector2 scale;

		float orientation;
		float rotation;
		float velocity;

		Matrix33::Matrix_33 mdl_to_ndc_xform;

	public : 
		// Constructors
		Object()
			: model_ref(""), shader_ref(""), color(0.0f, 0.0f, 0.0f), position(0.0f, 0.0f), scale(1.0f, 1.0f), orientation(0.0f), rotation(0.0f), velocity(0.f), mdl_to_ndc_xform(1.f) {}

		Object(const std::string& mdl, const std::string& shdr, Vector3 clr = Vector3(0.0f, 0.0f, 0.0f), Vector2 pos = Vector2(0.0f, 0.0f), Vector2 scl = Vector2(1.0f, 1.0f), float ori = 0, float rot = 0, float vel = 0 , Matrix33::Matrix_33 xform = 1.f);

		// Getters and setters
		std::string getModelRef() const;
		std::string getShaderRef() const;
		
		void setColor(const Vector3& clr);
		Vector3 getColor() const;

		void setPosition(float x, float y);
		Vector2 getPosition() const; 

		void setScale(const Vector2& scl);
		Vector2 getScale() const;

		void setRotation(float rot);
		float getRotation() const;

		void setOrientation(float ori);
		float getOrientation() const;

		void setVelocity(float vel);
		float getVelocity() const;

		void setXform(Matrix33::Matrix_33 xform);
		Matrix33::Matrix_33 getXform() const;

		void update(float dt, const Matrix33::Matrix_33& cam_to_ndc_xform);

		void draw() const;
};