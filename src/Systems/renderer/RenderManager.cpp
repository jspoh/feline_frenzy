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

void RenderManager::drawModel(const std::string& model_ref) {
    auto it = models.find(model_ref);
    if (it != models.end()) {
        shaderManager.useShader("base");
        it->second->draw();
        shaderManager.unuseShader();
    }
    else {
        cerr << "Model not found: " << model_ref << endl;
    }
}