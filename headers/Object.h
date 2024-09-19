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
		// !TODO: add in transform mtx once math lib is done

	public : 
		// Constructors
		Object(const std::string& mdl, Vector2 pos = Vector2(0.0f, 0.0f), Vector2 scl = Vector2(1.0f, 1.0f), float rot = 0);

		// Getters and setters

		void setPosition(const Vector2& pos);
		Vector2 getPosition() const; 

		void setScale(const Vector2& scl);
		Vector2 getScale() const;

		void setRot(float rot);
		float getRot() const;

		void update(float dt);

		void draw() const;
};