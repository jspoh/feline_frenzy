/*****************************************************************//**
 * \file   mAssets.cpp
 * \brief  Assets manager function definitions
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu 
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Assets/sAssets.h"
#include "Managers/Services/sAudio.h"

namespace NIKE {

	Assets::Service::~Service() {

		//Clear fonts
		for (auto& font : fonts_list) {
			for (auto& chars : font.second->char_map) {
				glDeleteTextures(1, &chars.second.texture);
			}
		}

		//Clear shaders
		for (auto& shader : shaders_list) {
			glDeleteProgram(shader.second);
		}

		//Clear models
		for (auto& model : models_list) {
			glDeleteVertexArrays(1, &model.second->vaoid);
			glDeleteBuffers(1, &model.second->vboid);
			glDeleteBuffers(1, &model.second->eboid);
		}

		//Clear textures
		for (auto& texture : textures_list) {
			glDeleteTextures(1, &texture.second->gl_data);
		}

		//Clear audios
		audio_system = nullptr;
	}

	// Drop file event listener
	void Assets::Service::onEvent(std::shared_ptr<Assets::FileDropEvent> event) {
		int file_count = event->count;
		const char** file_paths = event->paths;

		const std::set<std::string> valid_tex_ext = { ".png", ".jpg", ".jpeg", ".tex"};
		const std::set<std::string> valid_audio_ext = { ".wav" };

		// If filepath does not exist create one
		if (!std::filesystem::exists("assets")) {
			std::filesystem::create_directories("assets");
			NIKEE_CORE_INFO("Created filepath assets");
		}

		if (NIKE_IMGUI_SERVICE->getImguiActive() || NIKE_LVLEDITOR_SERVICE->getEditorState()) {

			for (int i = 0; i < file_count; ++i) {
				std::filesystem::path src_file_path{ file_paths[i] };

				// Makes sure extension isnt caps sensitive
				std::string ext = src_file_path.extension().string();
				// Transform each character in string ext to lowercase
				for (char& c : ext) {
					c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
				}

				if (valid_tex_ext.find(ext) != valid_tex_ext.end()) {
					handleTextureDrop(src_file_path);

				}
				else if (valid_audio_ext.find(ext) != valid_audio_ext.end()) {
					handleAudioDrop(src_file_path);
				}
				else {
					NIKEE_CORE_ERROR("ERROR: Unsupported File Type for file: {}", file_paths[i]);
				}
			}
		}
	}

	void Assets::Service::configAssets(std::shared_ptr<Audio::IAudioSystem> audio_sys) {
		font_loader = std::make_unique<Assets::FontLoader>();
		render_loader = std::make_unique<Assets::RenderLoader>();
		audio_system = audio_sys;
	}

	/*****************************************************************//**
	* Font
	*********************************************************************/
	void Assets::Service::loadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes) {
		if (fonts_list.find(font_id) != fonts_list.end())
		{
			NIKEE_CORE_WARN("Font {} is already loaded. Skipping load", font_id);
			return;
		}

		NIKEE_CORE_INFO("Loading font to '" + font_id + "'");
		fonts_list.emplace(std::piecewise_construct, std::forward_as_tuple(font_id), std::forward_as_tuple(std::make_shared<Assets::Font>(std::static_pointer_cast<Assets::NIKEFontLib>(font_loader->getFontLib())->generateFont(file_path, pixel_sizes))));
	}

	void  Assets::Service::reloadFont(std::string const& font_id, std::string const& file_path, Vector2f const& pixel_sizes) {
		fonts_list.at(font_id)->char_map = std::static_pointer_cast<Assets::NIKEFontLib>(font_loader->getFontLib())->generateFont(file_path, pixel_sizes).char_map;
	}

	void Assets::Service::unloadFont(std::string const& font_id) {
		//Find shader
		auto it = fonts_list.find(font_id);

		if (it == fonts_list.end())
		{
			NIKEE_CORE_WARN("Font {} could not be found. Skipping unload", font_id);
			return;
		}

		//Unload font
		for (auto& chars : it->second->char_map) {
			glDeleteTextures(1, &chars.second.texture);
		}
		fonts_list.erase(it);
	}

	void Assets::Service::unloadAllFonts()
	{
		for (auto const& font : fonts_list)
		{
			unloadFont(font.first);
		}

		fonts_list.clear();
	}

	std::shared_ptr<Assets::Font> const& Assets::Service::getFont(std::string const& font_id) const {
		//Find shader
		auto it = fonts_list.find(font_id);


		if (it == fonts_list.end())
		{
			LOG_CRASH("Font could not be found.");
		}

		return it->second;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Font>>& Assets::Service::getLoadedFonts() const
	{
		return fonts_list;
	}

	bool Assets::Service::checkFontExist(std::string const& font_id)
	{
		if (fonts_list.find(font_id) == fonts_list.end())
		{
			return false;
		}
		return true;
	}

	/*****************************************************************//**
	* Rendering
	*********************************************************************/
	void Assets::Service::loadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
		if (shaders_list.find(shader_id) != shaders_list.end())
		{
			NIKEE_CORE_WARN("Shader {} is already loaded. Skipping load", shader_id);
			return;
		}

		NIKEE_CORE_INFO("Loading shader to '" + shader_id + "'");
		shaders_list.emplace(std::piecewise_construct, std::forward_as_tuple(shader_id), std::forward_as_tuple(render_loader->compileShader(shader_id, vtx_path, frag_path)));
	}

	void Assets::Service::reloadShader(std::string const& shader_id, const std::string& vtx_path, const std::string& frag_path) {
		shaders_list.at(shader_id) = render_loader->compileShader(shader_id, vtx_path, frag_path);
	}

	void Assets::Service::unloadShader(std::string const& shader_id) {
		//Find shader
		auto it = shaders_list.find(shader_id);

		// Check if the shader exists in map
		if (it == shaders_list.end())
		{
			NIKEE_CORE_WARN("Shader {} could not be found. Skipping unload", shader_id);
			return;
		}

		//Unload shader
		glDeleteProgram(it->second);
		shaders_list.erase(it);
	}

	void Assets::Service::unloadAllShaders()
	{
		for (const auto& shader : shaders_list)
		{
			unloadShader(shader.first);
		}

		shaders_list.clear();
	}

	unsigned int Assets::Service::getShader(std::string const& shader_id) {
		//Find shader
		auto it = shaders_list.find(shader_id);

		//Check if shader exists
		if (it == shaders_list.end())
		{
			LOG_CRASH("Shader could not be found.");
		}

		return it->second;
	}

	const std::unordered_map<std::string, unsigned int>& Assets::Service::getLoadedShaders()
	{
		return shaders_list;
	}

	void Assets::Service::loadModel(std::string const& model_id, std::string const& file_path, bool for_batched_rendering) {
		if (models_list.find(model_id) != models_list.end())
		{
			NIKEE_CORE_WARN("Model {} is already loaded. Skipping load", model_id);
			return;
		}

		NIKEE_CORE_INFO("Loading model to '" + model_id + "'");
		models_list.emplace(std::piecewise_construct, std::forward_as_tuple(model_id), std::forward_as_tuple(std::make_shared<Assets::Model>(render_loader->compileModel(file_path, for_batched_rendering))));
	}

	void Assets::Service::reloadModel(std::string const& model_id, std::string const& file_path, bool for_batched_rendering) {
		*models_list.at(model_id) = render_loader->compileModel(file_path, for_batched_rendering);
	}

	void Assets::Service::unloadModel(std::string const& model_id) {
		//Find model
		auto it = models_list.find(model_id);

		// Check if the model exists in map
		if (it == models_list.end())
		{
			NIKEE_CORE_WARN("Model {} could not be found. Skipping unload", model_id);
			return;
		}

		//Unload model
		glDeleteVertexArrays(1, &it->second->vaoid);
		glDeleteBuffers(1, &it->second->vboid);
		glDeleteBuffers(1, &it->second->eboid);
		models_list.erase(it);
	}

	void Assets::Service::unloadAllModels()
	{
		for (auto const& model : models_list)
		{
			unloadModel(model.first);
		}

		models_list.clear();
	}

	std::shared_ptr<Assets::Model> Assets::Service::getModel(std::string const& model_id) {
		//Find model
		auto it = models_list.find(model_id);

		//Check if model exists
		if (it == models_list.end())
		{
			LOG_CRASH("Model could not be found.");
		}

		return it->second;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Model>>& Assets::Service::getLoadedModels()
	{
		return models_list;
	}


	bool Assets::Service::checkModelExist(std::string const& model_id)
	{
		if (models_list.find(model_id) == models_list.end())
		{
			return false;
		}
		return true;
	}

	void Assets::Service::loadTexture(std::string const& texture_id, std::string const& file_path) {
		if (textures_list.find(texture_id) != textures_list.end())
		{
			NIKEE_CORE_WARN("Texture {} is already loaded. Skipping load", texture_id);
			return;
		}

		NIKEE_CORE_INFO("Loading texture '" + texture_id + "'");
		textures_list.emplace(std::piecewise_construct, std::forward_as_tuple(texture_id), std::forward_as_tuple(std::make_shared<Assets::Texture>(render_loader->compileTexture(file_path))));
	}

	void Assets::Service::reloadTexture(std::string const& texture_id, std::string const& file_path) {
		*textures_list.at(texture_id) = render_loader->compileTexture(file_path);
	}

	void Assets::Service::unloadTexture(std::string const& texture_id) {
		//Find texture
		auto it = textures_list.find(texture_id);

		// Check if the texture exists in map
		if (it == textures_list.end())
		{
			NIKEE_CORE_WARN("Texture {} could not be found. Skipping unload", texture_id);
			return;
		}

		//Unload texture
		glDeleteTextures(1, &it->second->gl_data);
		textures_list.erase(it);
	}

	void Assets::Service::unloadAllTextures()
	{
		for (const auto& texture : textures_list)
		{
			unloadTexture(texture.first);
		}

		textures_list.clear();
	}

	std::shared_ptr<Assets::Texture> Assets::Service::getTexture(std::string const& texture_id) {
		if (textures_list.find(texture_id) == textures_list.end())
		{
			LOG_CRASH("Texture could not be found.");
		}

		return textures_list.at(texture_id);
	}

	bool Assets::Service::checkTextureExist(std::string const& texture_id)
	{
		if (textures_list.find(texture_id) == textures_list.end())
		{
			return false;
		}
		return true;
	}

	const std::unordered_map<std::string, std::shared_ptr<Assets::Texture>>& Assets::Service::getLoadedTextures()
	{
		return textures_list;
	}

	void Assets::Service::handleTextureDrop(const std::filesystem::path& src_file_path) {
		std::filesystem::path tgt_file_path = "assets/textures" / src_file_path.filename();

		try {
			std::filesystem::copy(src_file_path, tgt_file_path, std::filesystem::copy_options::overwrite_existing);
			NIKEE_CORE_INFO("File {} successfully copied into assets/textures", src_file_path.string());
		}
		catch (const std::filesystem::filesystem_error& e) {
			NIKEE_CORE_ERROR("ERROR: Failed to copy {}: {}", src_file_path.string(), e.what());
		}

		// Check if texture exists
		if (checkTextureExist(src_file_path.filename().string())) {
			// If it exists reload the updated texture
			reloadTexture(src_file_path.filename().string(), tgt_file_path.string());
		}
		else {
			// If it does not exist load the texture
			loadTexture(src_file_path.filename().string(), tgt_file_path.string());
		}
	}
	/*****************************************************************//**
	* Audio
	*********************************************************************/
	void Assets::Service::loadSfx(std::string const& audio_id, std::string const& file_path)
	{
		// Check if the audio already exists in the map
		if (sfx_list.find(audio_id) != sfx_list.end())
		{
			NIKEE_CORE_WARN("Sound effect {} is already loaded. Skipping load", audio_id);
			return;
		}

		NIKEE_CORE_INFO("Loading SFX '" + audio_id + "'");
		//Emplace in audio list
		sfx_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_id), std::forward_as_tuple(std::move(audio_system->createSound(file_path))));
	}

	void Assets::Service::reloadSfx(std::string const& audio_id, std::string const& file_path) {
		*sfx_list.at(audio_id) = *audio_system->createSound(file_path);
	}

	void Assets::Service::unloadSfx(std::string const& audio_id) {
		//Find audio
		auto it = sfx_list.find(audio_id);

		// Check if the audio already exists in the map
		if (it == sfx_list.end())
		{
			NIKEE_CORE_WARN("Audio {} could not be found. Skipping unload", audio_id);
			return;
		}

		//Unload audio here
		//! STOP AUDIO HERE

		//Unload audio
		it->second->release();
		sfx_list.erase(it);
	}

	void Assets::Service::loadMusic(std::string const& audio_id, std::string const& file_path)
	{
		// Check if the audio already exists in the map
		if (music_list.find(audio_id) != music_list.end())
		{
			NIKEE_CORE_WARN("Music {} is already loaded. Skipping load", audio_id);
			return;
		}

		NIKEE_CORE_INFO("Loading music '" + audio_id + "'");
		//Emplace in audio list
		music_list.emplace(std::piecewise_construct, std::forward_as_tuple(audio_id), std::forward_as_tuple(std::move(audio_system->createStream(file_path))));
	}

	void Assets::Service::reloadMusic(std::string const& audio_id, std::string const& file_path) {
		*music_list.at(audio_id) = *audio_system->createStream(file_path);
	}

	void Assets::Service::unloadMusic(std::string const& audio_id) {
		//Find audio
		auto it = music_list.find(audio_id);

		// Check if the audio already exists in the map
		if (it == music_list.end())
		{
			NIKEE_CORE_WARN("Audio {} could not be found. Skipping unload", audio_id);
			return;
		}

		//Unload audio here
		//! STOP AUDIO HERE

		//Unload audio
		it->second->release();
		music_list.erase(it);
	}


	void Assets::Service::unloadAllAudios()
	{
		for (const auto& sfx : sfx_list)
		{
			unloadSfx(sfx.first);
		}

		sfx_list.clear();

		for (const auto& music : music_list)
		{
			unloadSfx(music.first);
		}

		sfx_list.clear();
	}

	std::shared_ptr<Audio::IAudio> Assets::Service::getSfx(std::string const& audio_tag)
	{
		//Find audio
		auto it = sfx_list.find(audio_tag);

		// Check if the audio already exists in the map
		if (it == sfx_list.end())
		{
			LOG_CRASH("Audio could not be found.");
		}

		return it->second;
	}


	std::shared_ptr<Audio::IAudio> Assets::Service::getMusic(std::string const& audio_tag)
	{
		//Find audio
		auto it = music_list.find(audio_tag);

		// Check if the audio already exists in the map
		if (it == music_list.end())
		{
			LOG_CRASH("Audio could not be found.");
		}

		return it->second;
	}

	const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& Assets::Service::getLoadedSfx()
	{
		return sfx_list;
	}

	const std::unordered_map<std::string, std::shared_ptr<Audio::IAudio>>& Assets::Service::getLoadedMusic()
	{
		return music_list;
	}

	bool Assets::Service::checkSfxExist(std::string const& audio_tag)
	{
		if (sfx_list.find(audio_tag) == sfx_list.end())
		{
			return false;
		}
		return true;
	}

	bool Assets::Service::checkMusicExist(std::string const& audio_tag)
	{
		if (sfx_list.find(audio_tag) == sfx_list.end())
		{
			return false;
		}
		return true;
	}

	void Assets::Service::handleAudioDrop(const std::filesystem::path& src_file_path) {
		std::filesystem::path tgt_file_path = "assets/Audios" / src_file_path.filename();

		try {
			// std::filesystem::copy(src_file_path, tgt_file_path, std::filesystem::copy_options::overwrite_existing);
			NIKEE_CORE_INFO("File {} successfully copied into assets/Audios", src_file_path.string());
		}
		catch (const std::filesystem::filesystem_error& e) {
			NIKEE_CORE_ERROR("ERROR: Failed to copy {}: {}", src_file_path.string(), e.what());
		}

		NIKEE_CORE_INFO("File {} successfully copied into assets/Audios", src_file_path.string());

		// Check if audio exists
		if (checkSfxExist(src_file_path.filename().string())) {

		}
		else {

		}
	}

	/*****************************************************************//**
	* Scene 
	*********************************************************************/

	void Assets::Service::loadScn(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_regular_file() && hasValidScnTxtExtension(entry.path())) {
			std::string file_name = entry.path().filename().string();

			// Only add the file if it doesn't already exist in the levels list
			if (levels_list.find(file_name) == levels_list.end()) {
				levels_list[file_name] = entry.path();
			}
		}
	}

	void Assets::Service::loadScnFiles()
	{
		// Ensure the levels list is fresh each time
		levels_list.clear();

		// Iterate through the directory and load valid scene files
		for (const auto& entry : std::filesystem::directory_iterator(getScenesPath())) {
			loadScn(entry);
		}
	}

	bool Assets::Service::checkScnFileExist(const std::string& entry)
	{
		// Check if the file path is already in the levels_list
		return levels_list.find(entry) != levels_list.end();
	}

	void Assets::Service::reloadScn(std::string const& scn_key, std::filesystem::path const& scn_file_path)
	{
		auto it = levels_list.find(scn_key);
		if (it != levels_list.end()) {
			// If the scene exists, update it with the new file path
			it->second = scn_file_path;
			std::filesystem::directory_entry entry(scn_file_path);
			loadScn(entry);
		}
	}
	

	std::unordered_map<std::string, std::filesystem::path>& Assets::Service::getLevelsList()
	{
		return levels_list;
	}

	/*****************************************************************//**
	* Prefab
	*********************************************************************/

	void Assets::Service::loadPrefab(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_regular_file() && hasValidPrefabExtension(entry.path())) {
			std::string file_name = entry.path().filename().string();

			// Only add the file if it doesn't already exist in the prefabs list
			if (prefabs_list.find(file_name) == prefabs_list.end()) {
				prefabs_list[file_name] = entry.path();
			}
		}
	}

	void Assets::Service::loadPrefabFiles()
	{
		// Ensure the levels list is fresh each time
		prefabs_list.clear();

		// Iterate through the directory and load valid scene files
		for (const auto& entry : std::filesystem::directory_iterator(getPrefabsPath())) {
			loadPrefab(entry);
		}
	}

	bool Assets::Service::checkPrefabFileExist(const std::string& entry)
	{
		return prefabs_list.find(entry) != prefabs_list.end();
	}

	void Assets::Service::reloadPrefab(std::string const& prefab_key, 
		std::filesystem::path const& prefab_file_path)
	{
		auto it = prefabs_list.find(prefab_key);
		if (it != prefabs_list.end()) {
			it->second = prefab_file_path;
			std::filesystem::directory_entry entry(prefab_file_path);
			loadScn(entry);
		}
	}

	std::unordered_map<std::string, std::filesystem::path>& Assets::Service::getLoadedPrefabs()
	{
		return prefabs_list;
	}

	void Assets::Service::loadScript(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_regular_file() && hasValidScriptExtension(entry.path())) {
			std::string file_name = entry.path().filename().string();

			// Only add the file if it doesn't already exist in the script list
			if (scripts_list.find(file_name) == scripts_list.end()) {
				scripts_list[file_name] = entry.path();
			}
		}
	}

	void Assets::Service::loadScriptFiles()
	{
		// Ensure the list is fresh each time
		scripts_list.clear();

		// Iterate through the directory and load valid files
		for (const auto& entry : std::filesystem::directory_iterator(getScriptsPath())) {
			loadScript(entry);
		}
	}

	bool Assets::Service::checkScriptFileExist(const std::string& entry)
	{
		// Check if the file path is already in the scripts_list
		return scripts_list.find(entry) != scripts_list.end();
	}

	void Assets::Service::reloadScript(std::string const& script_key, std::filesystem::path const& script_file_path)
	{
		auto it = scripts_list.find(script_key);
		if (it != scripts_list.end()) {
			it->second = script_file_path;
			std::filesystem::directory_entry entry(script_file_path);
			loadScript(entry);
		}
	}

	std::unordered_map<std::string, std::filesystem::path>& Assets::Service::getLoadedScripts()
	{
		return scripts_list;
	}

	std::string const& Assets::Service::getTexturePath() 
	{ 
		return texture_path; 
	}

	std::string const& Assets::Service::getAudioPath() 
	{
		return audio_path; 
	}

	std::string const& Assets::Service::getFontPath() 
	{
		return font_path;
	}

	std::string const& Assets::Service::getModelsPath() 
	{ 
		return models_path; 
	}

	std::string const& Assets::Service::getScenesPath() 
	{
		return scenes_path;
	}

	std::string const& Assets::Service::getShadersPath() 
	{
		return shaders_path; 
	}

	std::string const& Assets::Service::getPrefabsPath()
	{
		return prefabs_path;
	}

	std::string const& Assets::Service::getScriptsPath()
	{
		return scripts_path;
	}

	void Assets::Service::reloadAssets(const std::string& asset_type)
	{
		if (asset_type == "Textures") {
			// Load new textures
			for (const auto& texture_paths : std::filesystem::directory_iterator(getTexturePath())) {
				if (hasValidTextureExtension(texture_paths)) {
					std::string file_name = texture_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;
					std::string base_name = file_name.substr(start, size);

					// Check if the texture already exists before loading
					if (!checkTextureExist(file_name.substr(start, size))) {
						loadTexture(base_name, texture_paths.path().string());
					}
					else {
						reloadTexture(base_name, texture_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Audio") {

			// Load new audio
			for (const auto& audio_paths : std::filesystem::recursive_directory_iterator(getAudioPath())) {
				if (hasValidAudioExtension(audio_paths)) {
					std::string file_name = audio_paths.path().filename().string();

					// Extract file name without leading backslashes or extension
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the audio already exists before loading
					if (!checkSfxExist(file_name.substr(start, size))) {
						loadSfx(file_name.substr(start, size), audio_paths.path().string());
					}
					else {
						reloadSfx(file_name.substr(start, size), audio_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Fonts") {

			// Load new fonts
			for (const auto& font_paths : std::filesystem::directory_iterator(getFontPath())) {
				if (hasValidFontExtension(font_paths)) {
					std::string file_name = font_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the font already exists before loading
					if (!checkFontExist(file_name.substr(start, size))) {
						loadFont(file_name.substr(start, size), font_paths.path().string());
					}
					else {
						reloadFont(file_name.substr(start, size), font_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Models") {

			// Load new fonts
			for (const auto& model_paths : std::filesystem::directory_iterator(getModelsPath())) {
				if (hasValidModelExtension(model_paths)) {
					std::string file_name = model_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the font already exists before loading
					if (!checkModelExist(file_name.substr(start, size))) {
						loadModel(file_name.substr(start, size), model_paths.path().string());
					}
					else {
						reloadModel(file_name.substr(start, size), model_paths.path().string());
					}
				}
			}
		}
		else if (asset_type == "Levels") {

			// Load new fonts
			for (const auto& scn_paths : std::filesystem::directory_iterator(getScenesPath())) {
				if (hasValidScnTxtExtension(scn_paths)) {
					std::string file_name = scn_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the font already exists before loading
					if (!checkScnFileExist(file_name.substr(start, size))) {
						loadScn(scn_paths);
					}
					else {
						reloadScn(file_name.substr(start, size), scn_paths.path());
					}
				}
			}
		}
		else if (asset_type == "Prefabs") {

			// Load new fonts
			for (const auto& prefab_paths : std::filesystem::directory_iterator(getPrefabsPath())) {
				if (hasValidPrefabExtension(prefab_paths)) {
					std::string file_name = prefab_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the font already exists before loading
					if (!checkPrefabFileExist(file_name.substr(start, size))) {
						loadPrefab(prefab_paths);
					}
					else {
						reloadPrefab(file_name.substr(start, size), prefab_paths.path());
					}
				}
			}
			}
		else if (asset_type == "Shaders") {

			// Load new fonts
			int temp_count = 0;
			for (const auto& shader_paths : std::filesystem::directory_iterator(getShadersPath())) {
				temp_count++;
				//Skip loading shader twice
				if (temp_count % 2)
					continue;

				if (hasValidVertExtension(shader_paths)) {
					//Get file name
					std::string file_name = shader_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					//Get file path
					std::string path = shader_paths.path().string();
					std::string vtx = path.substr(0, path.find_first_of('.')) + ".vert";
					std::string frag = path.substr(0, path.find_first_of('.')) + ".frag";

					//Check if the file already exists before loading
					if (shaders_list.find(file_name.substr(start, size)) == shaders_list.end()) {
						loadShader(file_name.substr(start, size), vtx, frag);
					}
					else {
						reloadShader(file_name.substr(start, size), vtx, frag);
					}
				}
			}
		}
		// Others goes here
		else if (asset_type == "Scripts") {

			// Load new fonts
			for (const auto& script_paths : std::filesystem::directory_iterator(getScriptsPath())) {
				if (hasValidScriptExtension(script_paths)) {
					std::string file_name = script_paths.path().filename().string();

					//string variables
					size_t start = file_name.find_first_not_of('\\');
					size_t size = file_name.find_first_of('.', start) - start;

					// Check if the font already exists before loading
					if (!checkPrefabFileExist(file_name.substr(start, size))) {
						loadScript(script_paths);
					}	
					else {
						reloadScript(file_name.substr(start, size), script_paths.path());
					}
				}
			}
			}
	}

	bool Assets::Service::deleteFile(std::string const& file_path, const std::string& asset_type)
	{
		if (asset_type == "Audio") {
			// FMOD free before deleting the sound
			for (auto it = sfx_list.begin(); it != sfx_list.end(); it++) {
				const auto& audio = *it;
				if (audio.second && audio.second->getFilePath() == file_path) {
					audio.second->release();   // Release the sound
					sfx_list.erase(it);   // Remove from the audio list
					break;
				}
			}
		}

		// Delete the file
		try {

			if (std::filesystem::remove(file_path)) {
				return true;
			}
			else {
				NIKEE_CORE_WARN("File not found: " + file_path);
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			NIKEE_CORE_ERROR("Error deleting file: " + file_path + ". Exception: " + e.what());
		}

		return false;
	}

	bool Assets::Service::deleteAllFiles(std::string const& file_path)
	{
		bool all_deleted = true;
		if (std::filesystem::exists(file_path) && std::filesystem::is_directory(file_path)) {
			// Iterate through each file in the directory
			for (const auto& entry : std::filesystem::directory_iterator(file_path)) {
				// Check if the entry is a file (not a subdirectory)
				if (std::filesystem::is_regular_file(entry.path())) {
					// Attempt to delete the file and check success
					if (!std::filesystem::remove(entry.path())) {
						all_deleted = false;
					}
				}
			}
		}
		return all_deleted;
	}

}
