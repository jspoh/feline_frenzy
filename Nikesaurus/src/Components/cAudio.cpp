/*****************************************************************//**
 * \file   cAudio.cpp
 * \brief  Audio components for ecs architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cAudio.h"

namespace NIKE {
    void Audio::registerComponents() {
        // Register audio components
        NIKE_ECS_MANAGER->registerComponent<Audio::SFX>();

        // Register SFX for serialization
        NIKE_SERIALIZE_SERVICE->registerComponent<Audio::SFX>(
            // Serialize lambda: Serialize primary fields and additional_sfx if present.
            [](Audio::SFX const& comp) -> nlohmann::json {
                nlohmann::json j = {
                    { "B_Play_SFX", comp.b_play_sfx },
                    { "Audio_ID", comp.audio_id },
                    { "Channel_Group_ID", comp.channel_group_id },
                    { "Volume", comp.volume },
                    { "Pitch", comp.pitch }
                };
                // If additional SFX entries exist, add them
                if (!comp.additional_sfx.empty()) {
                    nlohmann::json addArray = nlohmann::json::array();
                    for (auto const& entry : comp.additional_sfx) {
                        addArray.push_back({
                            { "B_Play_SFX", entry.b_play_sfx },
                            { "Audio_ID", entry.audio_id },
                            { "Channel_Group_ID", entry.channel_group_id },
                            { "Volume", entry.volume },
                            { "Pitch", entry.pitch }
                            });
                    }
                    j["Additional_SFX"] = addArray;
                }
                return j;
            },
            // Deserialize lambda:
            [](Audio::SFX& comp, nlohmann::json const& data) {
                comp.b_play_sfx = data.value("B_Play_SFX", false);
                comp.audio_id = data.value("Audio_ID", "");
                comp.channel_group_id = data.value("Channel_Group_ID", "");
                comp.volume = data.value("Volume", 1.0f);
                comp.pitch = data.value("Pitch", 1.0f);
                comp.additional_sfx.clear();
                if (data.contains("Additional_SFX")) {
                    for (auto const& entry_json : data["Additional_SFX"]) {
                        Audio::SFXEntry entry;
                        entry.b_play_sfx = entry_json.value("B_Play_SFX", false);
                        entry.audio_id = entry_json.value("Audio_ID", "");
                        entry.channel_group_id = entry_json.value("Channel_Group_ID", "");
                        entry.volume = entry_json.value("Volume", 1.0f);
                        entry.pitch = entry_json.value("Pitch", 1.0f);
                        comp.additional_sfx.push_back(entry);
                    }
                }
            },
            // Override Serialize lambda:
            [](Audio::SFX const& comp, Audio::SFX const& other_comp) -> nlohmann::json {
                nlohmann::json delta;
                if (comp.b_play_sfx != other_comp.b_play_sfx) {
                    delta["B_Play_SFX"] = comp.b_play_sfx;
                }
                if (comp.audio_id != other_comp.audio_id) {
                    delta["Audio_ID"] = comp.audio_id;
                }
                if (comp.channel_group_id != other_comp.channel_group_id) {
                    delta["Channel_Group_ID"] = comp.channel_group_id;
                }
                if (comp.volume != other_comp.volume) {
                    delta["Volume"] = comp.volume;
                }
                if (comp.pitch != other_comp.pitch) {
                    delta["Pitch"] = comp.pitch;
                }
                if (comp.additional_sfx != other_comp.additional_sfx) {
                    nlohmann::json addArray = nlohmann::json::array();
                    for (auto const& entry : comp.additional_sfx) {
                        addArray.push_back({
                            { "B_Play_SFX", entry.b_play_sfx },
                            { "Audio_ID", entry.audio_id },
                            { "Channel_Group_ID", entry.channel_group_id },
                            { "Volume", entry.volume },
                            { "Pitch", entry.pitch }
                            });
                    }
                    delta["Additional_SFX"] = addArray;
                }
                return delta;
            },
            // Override Deserialize lambda:
            [](Audio::SFX& comp, nlohmann::json const& delta) {
                if (delta.contains("B_Play_SFX")) {
                    comp.b_play_sfx = delta["B_Play_SFX"];
                }
                if (delta.contains("Audio_ID")) {
                    comp.audio_id = delta["Audio_ID"];
                }
                if (delta.contains("Channel_Group_ID")) {
                    comp.channel_group_id = delta["Channel_Group_ID"];
                }
                if (delta.contains("Volume")) {
                    comp.volume = delta["Volume"];
                }
                if (delta.contains("Pitch")) {
                    comp.pitch = delta["Pitch"];
                }
                if (delta.contains("Additional_SFX")) {
                    comp.additional_sfx.clear();
                    for (auto const& entry_json : delta["Additional_SFX"]) {
                        Audio::SFXEntry entry;
                        entry.b_play_sfx = entry_json.value("B_Play_SFX", false);
                        entry.audio_id = entry_json.value("Audio_ID", "");
                        entry.channel_group_id = entry_json.value("Channel_Group_ID", "");
                        entry.volume = entry_json.value("Volume", 1.0f);
                        entry.pitch = entry_json.value("Pitch", 1.0f);
                        comp.additional_sfx.push_back(entry);
                    }
                }
            }
        );

        NIKE_SERIALIZE_SERVICE->registerComponentAdding<Audio::SFX>();
    }

    void Audio::registerEditorComponents() {
#ifndef NDEBUG
        // TODO ADD PANELS FOR MULTIPLE SFX LATER
        NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Audio::SFX>(
            [](LevelEditor::ComponentsPanel& comp_panel, Audio::SFX& comp) {
                // Static variables for string input management
                static std::string channel_group;
                if (ImGui::IsItemActivated() || comp_panel.isEntityChanged()) {
                    channel_group = comp.channel_group_id;
                }
                ImGui::Text("Select SFX:");
                static std::string previous_audio_id = comp.audio_id;
                std::string current_audio_id = comp.audio_id;
                const auto& all_loaded_sounds = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Sound);
                int current_index = -1;
                for (size_t i = 0; i < all_loaded_sounds.size(); ++i) {
                    if (current_audio_id == all_loaded_sounds[i]) {
                        current_index = static_cast<int>(i);
                        break;
                    }
                }
                if (ImGui::Combo("##SelectSFX", &current_index, all_loaded_sounds.data(), static_cast<int>(all_loaded_sounds.size()))) {
                    if (current_index >= 0 && current_index < static_cast<int>(all_loaded_sounds.size())) {
                        std::string new_audio_id = all_loaded_sounds[current_index];
                        if (new_audio_id != comp.audio_id) {
                            LevelEditor::Action change_audio_action;
                            change_audio_action.do_action = [&, audio_id = new_audio_id]() {
                                comp.audio_id = audio_id;
                                };
                            change_audio_action.undo_action = [&, audio_id = previous_audio_id]() {
                                comp.audio_id = audio_id;
                                };
                            NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_audio_action));
                            previous_audio_id = new_audio_id;
                        }
                    }
                }
                ImGui::Text("Select Channel Group:");
                static std::string previous_channel_group_id = comp.channel_group_id;
                std::string current_channel_group_id = comp.channel_group_id;
                const auto& all_loaded_channel_groups = NIKE_AUDIO_SERVICE->getAllChannelGroups();
                std::vector<const char*> all_channel_group_names;
                for (auto& elem : all_loaded_channel_groups) {
                    all_channel_group_names.push_back(elem.first.c_str());
                }
                int current_index_group = -1;
                for (size_t i = 0; i < all_channel_group_names.size(); ++i) {
                    if (current_channel_group_id == all_channel_group_names[i]) {
                        current_index_group = static_cast<int>(i);
                        break;
                    }
                }
                if (ImGui::Combo("##SelectChannelGroup", &current_index_group, all_channel_group_names.data(), static_cast<int>(all_channel_group_names.size()))) {
                    if (current_index_group >= 0 && current_index_group < static_cast<int>(all_channel_group_names.size())) {
                        std::string new_channel_group_id = all_channel_group_names[current_index_group];
                        if (new_channel_group_id != comp.channel_group_id) {
                            LevelEditor::Action change_channel_group_id_action;
                            change_channel_group_id_action.do_action = [&, channel_group_id = new_channel_group_id]() {
                                comp.channel_group_id = channel_group_id;
                                };
                            change_channel_group_id_action.undo_action = [&, channel_group_id = previous_channel_group_id]() {
                                comp.channel_group_id = channel_group_id;
                                };
                            NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_channel_group_id_action));
                            previous_channel_group_id = new_channel_group_id;
                        }
                    }
                }
                if (!comp.audio_id.empty() && !comp.channel_group_id.empty()) {
                    ImGui::Text("Edit Audio Volume & Pitch:");
                    static float vol_before_change;
                    static bool vol_changed = false;
                    if (ImGui::SliderFloat("Volume", &comp.volume, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                        vol_changed = true;
                    }
                    if (!ImGui::IsItemActive()) {
                        if (vol_changed) {
                            LevelEditor::Action change_volume;
                            change_volume.do_action = [&, vol = comp.volume]() {
                                comp.volume = vol;
                                };
                            change_volume.undo_action = [&, vol = vol_before_change]() {
                                comp.volume = vol;
                                };
                            NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_volume));
                            vol_changed = false;
                        }
                        vol_before_change = comp.volume;
                    }
                    static float pitch_before_change;
                    static bool pitch_changed = false;
                    if (ImGui::SliderFloat("Pitch", &comp.pitch, 0.5f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                        pitch_changed = true;
                    }
                    if (!ImGui::IsItemActive()) {
                        if (pitch_changed) {
                            LevelEditor::Action change_pitch;
                            change_pitch.do_action = [&, pitch = comp.pitch]() {
                                comp.pitch = pitch;
                                };
                            change_pitch.undo_action = [&, pitch = pitch_before_change]() {
                                comp.pitch = pitch;
                                };
                            NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_pitch));
                            pitch_changed = false;
                        }
                        pitch_before_change = comp.pitch;
                    }
                    ImGui::Text("Play SFX:");
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
#endif
    }
}
