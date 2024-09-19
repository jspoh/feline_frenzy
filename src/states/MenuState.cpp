/*****************************************************************//**
 * \file   MenuState.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/


#include "stdafx.h"
#include "MenuState.h"
#include "StateManager.h"
#include "RenderManager.h"
#include "AudioSystem.h"


MenuState::MenuState() {
	StateManager::getInstance().register_state("main_menu", this);
}

MenuState::~MenuState() {
}

MenuState& MenuState::getInstance() {
	static MenuState instance;
	return instance;
}

void MenuState::load() {
	RenderManager::getInstance().registerMesh("square", "assets/meshes/square.txt");
	
}

void MenuState::init() {
	// Audio system testing
	NIKEEngine.accessSystem("audio")->getActiveState();
	NE_AUDIO_GROUP test_group = AudioSystem::AudioSystem::getInstance().CreateAudioGroup("group 1");
	// NE_AUDIO test_sound = AudioSystem::AudioSystem::getInstance().NEAudioLoadSound("assets/Audio/test_sound.ogg", "test");
	NE_AUDIO test_music = AudioSystem::AudioSystem::getInstance().NEAudioLoadMusic("assets/Audio/test_music.wav", "test");
	AudioSystem::AudioSystem::getInstance().NEAudioPlay(test_music, test_group, 1.f, 1.f, 0);

}

void MenuState::update() {
}

void MenuState::render() {
	RenderManager::getInstance().drawModel("square");
}

void MenuState::exit() {
}

void MenuState::unload() {
}
