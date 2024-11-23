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

				//Static variables for audio string input management
				static std::string audio_id;
				static std::string channel_group_id;

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated() || comp_panel.isEntityChanged()) {
					audio_id = comp.audio_id;
					channel_group_id = comp.channel_group_id;
				}

				//Set audio ID
				{
					//Channel id input
					ImGui::Text("Enter Audio ID:");
					if(ImGui::InputText("##AudioID", audio_id.data(), audio_id.capacity() + 1)) {
						audio_id.resize(strlen(audio_id.c_str()));
					}

					ImGui::SameLine();

					//Save audio ID Button
					if (ImGui::Button("Save##AudioID")) {
						if (NIKE_ASSETS_SERVICE->checkSfxExist(audio_id)) {
							LevelEditor::Action save_audio;

							//Save audio action
							save_audio.do_action = [&, id = audio_id]() {
								comp.audio_id = id;
								audio_id = comp.audio_id;
							};

							//Undo save audio action
							save_audio.undo_action = [&, id = comp.audio_id]() {
								comp.audio_id = id;
								audio_id = comp.audio_id;
							};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_audio));
							comp_panel.setPopUpSuccessMsg("Audio Saved!");
							comp_panel.openPopUp("Success");
						}
						else {
							comp_panel.setPopUpErrorMsg("Audio Does Not Exist!");
							comp_panel.openPopUp("Error");
							audio_id = comp.audio_id;
						}
					}
				}

				//Set Channel Group ID
				{
					//Channel group id input
					ImGui::Text("Enter Channel Group ID:");
					if (ImGui::InputText("##ChannelGroupID", channel_group_id.data(), channel_group_id.capacity() + 1)) {
						channel_group_id.resize(strlen(channel_group_id.c_str()));
					}

					ImGui::SameLine();

					//Save audio ID Button
					if (ImGui::Button("Save##ChannelGroupID")) {
						if (NIKE_AUDIO_SERVICE->checkChannelGroupExist(channel_group_id)) {
							LevelEditor::Action save_channel_group;

							//Save audio action
							save_channel_group.do_action = [&, id = channel_group_id]() {
								comp.channel_group_id = id;
								channel_group_id = comp.channel_group_id;
								};

							//Undo save audio action
							save_channel_group.undo_action = [&, id = comp.channel_group_id]() {
								comp.channel_group_id = id;
								channel_group_id = comp.channel_group_id;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_channel_group));
							comp_panel.setPopUpSuccessMsg("Audio Channel Group Saved!");
							comp_panel.openPopUp("Success");
						}
						else {
							comp_panel.setPopUpErrorMsg("Channel Group Does Not Exist!");
							comp_panel.openPopUp("Error");
							channel_group_id = comp.channel_group_id;
						}
					}
				}

				ImGui::Text("Edit Audio Volume & Pitch:");

				//Set volume
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
					if (ImGui::SliderFloat("Pitch", &comp.pitch, 0.f, 1.f, "%.2f")) {
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
		);
	}
}