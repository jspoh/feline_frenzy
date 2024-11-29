/*****************************************************************//**
 * \file   cAudio.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cAudio.h"

namespace NIKE {
	void Audio::registerComponents() {
		//Register audio components
		NIKE_ECS_MANAGER->registerComponent<Audio::SFX>();

		//Register SFX for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Audio::SFX>(
			//Serialize
			[](Audio::SFX const& comp) -> nlohmann::json {
				return	{
						{ "B_Play_SFX", comp.b_play_sfx },
						{ "Audio_ID", comp.audio_id },
						{ "Channel_Group_ID", comp.channel_group_id },
						{ "Volume", comp.volume },
						{ "Pitch", comp.pitch }
						};
			},

			//Deserialize
			[](Audio::SFX& comp, nlohmann::json const& data) {
				comp.b_play_sfx = data.at("B_Play_SFX").get<bool>();
				comp.audio_id = data.at("Audio_ID").get<std::string>();
				comp.channel_group_id = data.at("Channel_Group_ID").get<std::string>();
				comp.volume = data.at("Volume").get<float>();
				comp.pitch = data.at("Pitch").get<float>();
			}
		);

		//Register SFX for level editor UI
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Audio::SFX>(
			[](LevelEditor::ComponentsPanel& comp_panel, Audio::SFX& comp) {

				//Set audio ID
				{
					// Display a combo box for selecting a audio
					ImGui::Text("Select SFX:");

					// Hold the current and previous audio selection
					static std::string previous_audio_id = comp.audio_id;
					std::string current_audio_id = comp.audio_id;

					// Get all loaded audios
					const auto& all_loaded_sounds = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Sound);

					// Find the index of the currently selected audio in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_sounds.size(); ++i) {
						if (current_audio_id == all_loaded_sounds[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for audio selection
					if (ImGui::Combo("##SelectSFX", &current_index, all_loaded_sounds.data(), static_cast<int>(all_loaded_sounds.size()))) {
						// Validate the selected index and get the new audio ID
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_sounds.size())) {
							std::string new_audio_id = all_loaded_sounds[current_index];
							if (new_audio_id != comp.audio_id) {
								// Save action
								LevelEditor::Action change_audio_action;
								change_audio_action.do_action = [&, audio_id = new_audio_id]() {
									comp.audio_id = audio_id;
									};

								// Undo action
								change_audio_action.undo_action = [&, audio_id = previous_audio_id]() {
									comp.audio_id = audio_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_audio_action));

								// Update the previous audio ID
								previous_audio_id = new_audio_id;
							}
						}
					}
				}

				//Set Channel Group ID
				{
					// Display a combo box for selecting a audio
					ImGui::Text("Select Channel Group:");

					// Hold the current and previous audio selection
					static std::string previous_channel_group_id = comp.channel_group_id;
					std::string current_channel_group_id = comp.channel_group_id;

					// Get all loaded audios
					const auto& all_loaded_channel_groups = NIKE_AUDIO_SERVICE->getAllChannelGroups();

					// Get the string from the map and change to const char* for imgui combo
					std::vector<const char*> all_channel_group_names;
					for (auto& elem : all_loaded_channel_groups)
					{
						all_channel_group_names.push_back(elem.first.c_str());
					}

					// Find the index of the currently selected audio in the list
					int current_index = -1;
					for (size_t i = 0; i < all_channel_group_names.size(); ++i) {
						if (current_channel_group_id == all_channel_group_names[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for audio selection
					if (ImGui::Combo("##SelectChannelGroup", &current_index, all_channel_group_names.data(), static_cast<int>(all_channel_group_names.size()))) {
						// Validate the selected index and get the new audio ID
						if (current_index >= 0 && current_index < static_cast<int>(all_channel_group_names.size())) {
							std::string new_channel_group_id = all_channel_group_names[current_index];
							if (new_channel_group_id != comp.channel_group_id) {
								// Save action
								LevelEditor::Action change_channel_group_id_action;
								change_channel_group_id_action.do_action = [&, channel_group_id = new_channel_group_id]() {
									comp.channel_group_id = channel_group_id;
									};

								// Undo action
								change_channel_group_id_action.undo_action = [&, channel_group_id = previous_channel_group_id]() {
									comp.channel_group_id = channel_group_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_channel_group_id_action));

								// Update the previous audio ID
								previous_channel_group_id = new_channel_group_id;
							}
						}
					}
				}

				//Set volume
				if (!comp.audio_id.empty() && !comp.channel_group_id.empty())
				{
					ImGui::Text("Edit Audio Volume & Pitch:");
					{
						//Volume before change
						static float vol_before_change;

						//Flag for signalling vol changed
						static bool vol_changed = false;

						// Set volume
						if (ImGui::SliderFloat("Volume", &comp.volume, 0.f, 1.f, "%.2f")) {
							vol_changed = true;
						}

						//Check if volume has finished editing
						if (!ImGui::IsItemActive()) {

							//Check if volume changed
							if (vol_changed) {
								LevelEditor::Action change_volume;

								//Change volume do action
								change_volume.do_action = [&, vol = comp.volume]() {
									comp.volume = vol;
									};

								//Change volume undo action
								change_volume.undo_action = [&, vol = vol_before_change]() {
									comp.volume = vol;
									};

								//Execute action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_volume));

								vol_changed = false;
							}

							vol_before_change = comp.volume;
						}
					}

					//Set pitch
					{
						//Pitch before change
						static float pitch_before_change;

						//Flag for signalling pitch changed
						static bool pitch_changed = false;

						//Set pitch
						if (ImGui::SliderFloat("Pitch", &comp.pitch, 0.5f, 2.f, "%.2f")) {
							pitch_changed = true;
						}

						//Check if pitch has finished editing
						if (!ImGui::IsItemActive()) {

							//Check if volume changed
							if (pitch_changed) {
								LevelEditor::Action change_pitch;

								//Change pitch do action
								change_pitch.do_action = [&, pitch = comp.pitch]() {
									comp.pitch = pitch;
									};

								//Change [volume]itch undo action
								change_pitch.undo_action = [&, pitch = pitch_before_change]() {
									comp.pitch = pitch;
									};

								//Execute action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_pitch));

								pitch_changed = false;
							}

							pitch_before_change = comp.pitch;
						}
					}

					ImGui::Text("Play SFX:");

					// A button to play SFX
					if (ImGui::ArrowButton("PlayAudio", ImGuiDir_Right)) {
						if (comp.audio_id != "" && comp.channel_group_id != "") {
							comp.b_play_sfx = true;
						}
						else {
							comp_panel.setPopUpErrorMsg("Please input valid audio & channel group!");
							comp_panel.openPopUp("Error");
						}
					}
				}
				
			}
		);
	}
}