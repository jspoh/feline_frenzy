/*****************************************************************//**
 * \file   RenderManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "stdafx.h"
#include "RenderManager.h"
#include "Engine.h"

RenderManager::RenderManager() {
	// compile shaders
	shaderManager.loadShader("base", "shaders/base.vert", "shaders/base.frag");
	shaderManager.loadShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");
	// !TODO: shader for text rendering(?)
}

RenderManager::~RenderManager() {
    for (auto& [ref, model] : models) {
        delete model;
    }
}

RenderManager& RenderManager::getInstance() {
	static RenderManager instance;
	return instance;
}

void RenderManager::registerModel(const std::string& model_ref, const std::string& path_to_mesh) {
    Model* model = new Model();
    if (model->loadMesh(path_to_mesh)) {
        models[model_ref] = model;
    }
    else {
        delete model;
        cerr << "Failed to load model: " << model_ref << endl;
    }
}

void RenderManager::registerObject(const std::string& object_ref, const std::string& model_ref) {
    // Check if the model exists before creating the object
    if (models.find(model_ref) == models.end()) {
        cerr << "Model not found: " << model_ref << endl;
        return;
    }

    Object obj(model_ref);

    objects[object_ref] = obj;
}

//!TODO change to draw object to draw each object and each object will reference the model
void RenderManager::drawObjects() {
    shaderManager.useShader("base");

    for (const auto& [object_ref, object] : objects) {
        object.draw();
    }

    shaderManager.unuseShader();
}