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
				//Render call for all entity
				NIKE_RENDER_SERVICE->renderComponents(NIKE_ECS_MANAGER->getAllEntityComponents(entity), false);
#endif
			}
		}

		//Complete rendering process
		NIKE_RENDER_SERVICE->completeRender();

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
		static Transform::Transform fps_transform = { Vector2f((worldsize.x / 2.0f) - 200.0f, (worldsize.y / 2.0f) - 30.0f), Vector2f(1.0f, 1.0f), 0.0f };
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