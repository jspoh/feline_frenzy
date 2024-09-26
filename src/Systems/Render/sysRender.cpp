/*****************************************************************//**
 * \file   RenderManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysRender.h"

Render::Manager::Manager() {
	// compile shaders
	shaderManager.loadShader("base", "shaders/base.vert", "shaders/base.frag");
	shaderManager.loadShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");
	// !TODO: shader for text rendering(?)
}

Render::Manager::~Manager() {
    for (auto& [ref, model] : models) {
        delete model;
    }
}

Render::Manager& Render::Manager::getInstance() {
	static Render::Manager instance;
	return instance;
}

void Render::Manager::initCamera(const std::string& object_ref) {
    if (objects.find(object_ref) == objects.end()) {
        cerr << "Object not found: " << object_ref << endl;
        return;
    }

    // Get window size
    float wWidth = Core::Engine::getInstance().getWindowSize().x;
    float wHeight = Core::Engine::getInstance().getWindowSize().y;

    // Get camera object
    Object cam_obj = objects[object_ref];
    // Create a pointer to camera class
    camera = std::make_unique<Camera>();
    // Init camera
    camera->init(cam_obj, wWidth, wHeight);

}

// Adds a mesh / model into the model map
void Render::Manager::registerModel(const std::string& model_ref, const std::string& path_to_mesh) {
    Model* model = new Model();
    if (model->loadMesh(path_to_mesh)) {
        models[model_ref] = model;
    }
    else {
        delete model;
        cerr << "Failed to load model: " << model_ref << endl;
    }
}

// Constructs an object and places it into the object map
void Render::Manager::createObject(const std::string& object_ref, const std::string& model_ref, const Vector3& color, const Vector2& position, const Vector2& scale, float orientation, float rotation, float velocity) {
    // Check if the model exists before creating the object
    if (models.find(model_ref) == models.end()) {
        cerr << "Model not found: " << model_ref << endl;
        return;
    }

    // !TODO each object should have its own shader ref
    Object obj(model_ref, "base", color, position, scale, orientation, rotation, velocity );

    objects[object_ref] = std::move(obj);
   
}

// Returns a pointer to an object in the map
Object* Render::Manager::getObject(const std::string& object_ref) {
    if (objects.find(object_ref) != objects.end()) {
        return &objects[object_ref];
    }
    cerr << "Object not found: " << object_ref << endl;
    return nullptr;
}

// Update all objects transform matrix
void Render::Manager::updateObjects() {
    float deltaTime = Core::Engine::getInstance().getDeltaTime();

    for (auto& [object_ref, object] : objects) {
        if (object_ref == "camera") {
            camera.get()->update(object);
        }
        object.update(deltaTime, camera->getWorldToNDCXform());
    }
}

// Draw all objects
void Render::Manager::drawObjects() {

    for (const auto& [object_ref, object] : objects) {
        shaderManager.useShader(object.getShaderRef());

        shaderManager.setUniform(object.getShaderRef(), "f_color", object.getColor());
        shaderManager.setUniform(object.getShaderRef(), "model_to_ndc", object.getXform());
       
        object.draw();

    }
    shaderManager.unuseShader();
}

