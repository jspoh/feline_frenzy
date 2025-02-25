/*****************************************************************//**
 * \file   sysRender.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"
#include "Components/cTransform.h"
#include "Components/cPhysics.h"
#include "Components/cRender.h"
#include "Math/Mtx33.h"


// !TODO: jspoh reorg
namespace {

	Matrix_33 getWorldToScreenMtx() {
		// transform to screen coords
		NIKE::Render::Cam cam = NIKE_CAMERA_SERVICE->getActiveCamera();

		Matrix_33 view_xform{
			1, 0,  -cam.position.x,
			0, 1,  -cam.position.y,
			0, 0, 1
		};

		const float cam_height = NIKE_CAMERA_SERVICE->getCameraHeight();

		Matrix_33 cam_to_ndc_xform{
			2.0f / NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio() / (cam_height * cam.zoom), 0, 0,
			0, 2.0f / (cam_height * cam.zoom), 0,
			0, 0, 1
		};

		const float screenWidth = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x);
		const float screenHeight = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);

		Matrix_33 screen_xform = Matrix_33{
			screenWidth * 0.5f, 0, screenWidth * 0.5f,
			0, screenHeight * 0.5f, screenHeight * 0.5f,
			0, 0, 1
		} *cam_to_ndc_xform * view_xform;

		return screen_xform;
	};

	Vector2f worldToScreen(const Vector2f& world_pos) {
		Matrix_33 screen_xform = getWorldToScreenMtx();
		return screen_xform * world_pos;
	}
}

namespace NIKE {

	void Render::Manager::init() {

		NIKE_RENDER_SERVICE->init();

		//GL enable opacity blending option
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	void Render::Manager::update() {

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

#ifndef NDEBUG
		//Bind frame buffer for rendering to editor ( Binding occurs when editor is active )
		NIKE_LVLEDITOR_SERVICE->bindEditorFrameBuffer();
#endif

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

			//SKip inactive layer
			if (!layer->getLayerState())
				continue;
			
			if (layer->getLayerYSort()) {
				layer->sortEntitiesBasedOnYPosition();
			}


			for (auto& entity : layer->getEntitites()) {

				//Skip entity not registered to this system
				if (entities.find(entity) == entities.end()) continue;

#ifndef NDEBUG
				//Render call for all entity
				NIKE_RENDER_SERVICE->renderComponents(NIKE_ECS_MANAGER->getAllEntityComponents(entity), NIKE_LVLEDITOR_SERVICE->getDebugState());
#endif

#ifdef NDEBUG
				//Render call for all enttity
				NIKE_RENDER_SERVICE->renderComponents(NIKE_ECS_MANAGER->getAllEntityComponents(entity), false);
#endif
			}
		}

		//Complete rendering process
		NIKE_RENDER_SERVICE->completeRender();

		//!!! RENDER UI HERE - SH

		//// !TODO: jspoh move this update function
		//NIKE::SysParticle::Manager::getInstance().update();

		//Vector2f mouse_pos = NIKE_INPUT_SERVICE->getMouseWindowPos();
		//Vector2f window_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();
		//Vector2f mouse_particle_pos = { mouse_pos.x, window_size.y - mouse_pos.y };

		//using namespace NIKE::SysParticle;
		//NIKE_RENDER_SERVICE->renderParticleSystem("cluster", mouse_particle_pos);

		//auto& PM = NIKE::SysParticle::Manager::getInstance();
		//for (const auto& [ref, ps] : PM.getActiveParticleSystemsMap()) {
		//	//const unsigned int vao = PM.getVAO(ps.preset);
		//	const unsigned int vbo = PM.getVBO(ps.preset);

		//	std::vector<Particle> particles = ps.particles;

		//	// assume world pos
		//	if (ps.using_world_pos) {
		//		std::for_each(particles.begin(), particles.end(), [&](Particle& p) {
		//			p.pos = worldToScreen(p.pos);
		//		});
		//	}

		//	err = glGetError();
		//	if (err != GL_NO_ERROR) {
		//		NIKEE_CORE_ERROR("OpenGL error before updating particle system vbo {0}: {1}", __FUNCTION__, err);
		//	}
		//	glNamedBufferSubData(vbo, 0, particles.size() * sizeof(Particle), particles.data());
		//	err = glGetError();
		//	if (err != GL_NO_ERROR) {
		//		NIKEE_CORE_ERROR("OpenGL error after updating particle system vbo {0}: {1}", __FUNCTION__, err);
		//	}

		//	const int num_particles = max(1, static_cast<int>(particles.size()));

		//	NIKE_RENDER_SERVICE->renderParticleSystem(static_cast<int>(ps.preset), ps.origin, static_cast<int>(ps.render_type), num_particles, ref == "mouseps1");
		//}

		// mouse particles
		//NIKE::SysParticle::Manager::getInstance().setParticleSystemOrigin("mouseps1", mouse_particle_pos);
		//NIKE::SysParticle::Manager::getInstance().setParticleSystemOrigin("mouseps2", mouse_particle_pos);
		//NIKE::SysParticle::Manager::getInstance().setParticleSystemOrigin("mouseps3", mouse_particle_pos);
		//NIKE_RENDER_SERVICE->renderParticleSystem(static_cast<int>(Data::ParticlePresets::BASE), mouse_particle_pos);

		//FPS Rendering variables
		static auto worldsize = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();
		static Render::Text fps = { "Skranji-Bold.ttf", "FPS:", { 1.f, 1.f, 1.f, 1.f }, 1.0f , TextOrigin::LEFT };

		//Calculate avg FPS
		static std::vector<float> fps_history(300);
		static float elapsed_time = 0.0f;
		elapsed_time += NIKE_WINDOWS_SERVICE->getDeltaTime();
		float curr_fps = NIKE_WINDOWS_SERVICE->getCurrentFPS();
		if (fps_history.empty() || std::abs(curr_fps - fps_history.back()) < 10000) { //Ignore outlier FPS jumps of by 10000
			fps_history.push_back(curr_fps);
		}

		//Update avg fps every second
		if (elapsed_time > 1.f) {

			//Calculate average fps
			elapsed_time = 0.f;
			float sum_fps = 0.f;
			std::for_each(fps_history.begin(), fps_history.end(), [&sum_fps](float& fps) { sum_fps += fps; });
			const float avg_fps = sum_fps / fps_history.size();
			fps_history.clear();

			//Update fps
			std::stringstream ss;
			ss << "FPS: " << std::round(avg_fps);
			fps.text = ss.str();
		}

		//Render FPS Display
		Matrix_33 matrix;
		static bool show_fps = true;
		static Transform::Transform fps_transform = { Vector2f((worldsize.x / 2.0f) - 200.0f, (worldsize.y / 2.0f) - 30.0f), Vector2f(1.0f, 1.0f), 0.0f};
		fps_transform.use_screen_pos = true;

		//Toggle showing of FPS
		show_fps = NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_F1) ? !show_fps : show_fps;
		if (show_fps) {
			NIKE_RENDER_SERVICE->transformMatrix(fps_transform, matrix, NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform());
			NIKE_RENDER_SERVICE->renderText(matrix, fps);
		}

#ifndef NDEBUG
		//Unbind when editor is active for rendering
		NIKE_LVLEDITOR_SERVICE->unbindEditorFrameBuffer();
#endif

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}
}