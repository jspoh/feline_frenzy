/*****************************************************************//**
 * \file   Object.h
 * \brief  Model class that holds the VAO, draw count, and primitive type.
 *
 * \author g.boonxuensean
 * \date   September 2024
 *********************************************************************/

#pragma once


#include "stdafx.h"
#include "Model.h"


class Object {
	public : 
		Model* model;
		Vector2 position;
		Vector2 scale;
		Vector2 rotation;

		Object(Model* mdl, Vector2 pos = Vector2(0.0f, 0.0f), Vector2 scl = Vector2(1.0f, 1.0f), Vector2 rot = Vector2(0.0f, 0.0f))
			: model(mdl), position(pos), scale(scl), rotation(rot) {}

		void draw() const {
			model->draw();
		}
};