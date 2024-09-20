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
		Vector2 position;
		Vector2 scale;
		float rotation;
		glm::mat3 mdl_to_ndc_xform;
		// !TODO: change to our matrix lib

	public : 
		// Constructors
		Object()
			: model_ref(""), position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0f), mdl_to_ndc_xform(1.0f) {}

		Object(const std::string& mdl, Vector2 pos = Vector2(0.0f, 0.0f), Vector2 scl = Vector2(1.0f, 1.0f), float rot = 0, glm::mat3 xform = 1.0f);

		// Getters and setters
		std::string getModelRef() const;

		void setPosition(const Vector2& pos);
		Vector2 getPosition() const; 

		void setScale(const Vector2& scl);
		Vector2 getScale() const;

		void setRot(float rot);
		float getRot() const;

		void setXform(glm::mat3 xform);
		glm::mat3 getXform() const;

		void update(float dt);

		void draw() const;
};