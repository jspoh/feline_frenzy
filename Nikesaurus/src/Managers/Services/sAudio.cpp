/*****************************************************************//**
 * \file   sAudio.cpp
 * \brief  Audio manager function definitions 
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (35%)
 * \co-author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (35%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (30%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sAudio.h"
#include "Managers/Services/Assets/sAssets.h"

namespace NIKE {
	/*****************************************************************//**
	* NIKE AUDIO
	*********************************************************************/
	Audio::NIKEAudio::NIKEAudio(FMOD::Sound* sound, const std::string& path)
		:sound{ sound }, file_path{path} {}

	FMOD::Sound* Audio::NIKEAudio::getAudio() {
		return sound;
	}

	void Audio::NIKEAudio::lock(unsigned int offset, unsigned int length, void** ptr1, void** ptr2, unsigned int* len1, unsigned int* len2) {
		auto result = sound->lock(offset, length, ptr1, ptr2, len1, len2);
		if (result != FMOD_OK) {
			NIKEE_CORE_WARN("Error with locking audio!");
		}
	}

	void Audio::NIKEAudio::unlock(void* ptr1, void* ptr2, unsigned int len1, unsigned int len2) {
		auto result = sound->unlock(ptr1, ptr2, len1, len2);
		if (result != FMOD_OK) {
			NIKEE_CORE_WARN("Error with unlocking audio!");
		}
	}

	void Audio::NIKEAudio::release() {
		sound->release();
	}

	std::string Audio::NIKEAudio::getFilePath() const{
		return file_path;
	}

	unsigned int Audio::NIKEAudio::getLength(NIKE_AUDIO_TIMEUNIT time_unit) const {
		unsigned int length;
		sound->getLength(&length, time_unit);
		return length;
	}

	void Audio::NIKEAudio::setMode(NIKE_AUDIO_MODE mode) {
		sound->setMode(mode);
	}

	NIKE_AUDIO_MODE Audio::NIKEAudio::getMode() const{
		FMOD_MODE mode;
		sound->getMode(&mode);
		return mode;
	}

	void Audio::NIKEAudio::setLoopCount(int count) {
		sound->setLoopCount(count);
	}

	int Audio::NIKEAudio::getLoopCount() const {
		int count;
		sound->getLoopCount(&count);
		return count;
	}

	void Audio::NIKEAudio::setLoopPoints(unsigned int start, unsigned int end) {
		sound->setLoopPoints(start, NIKE_AUDIO_TIMEUNIT_MS, end, NIKE_AUDIO_TIMEUNIT_MS);
	}

	Vector2<unsigned int> Audio::NIKEAudio::getLoopPoints() const {
		Vector2<unsigned int> points;
		sound->getLoopPoints(&points.x, NIKE_AUDIO_TIMEUNIT_MS, &points.y, NIKE_AUDIO_TIMEUNIT_MS);
		return points;
	}

	/*****************************************************************//**
	* NIKE CHANNEL
	*********************************************************************/
	Audio::NIKEChannel::NIKEChannel(FMOD::Channel* channel, std::shared_ptr<IAudio> sound)
		: channel{ channel }, sound{ sound } {}

	FMOD::Channel* Audio::NIKEChannel::getChannel() {
		return channel;
	}

	void Audio::NIKEChannel::stop() {
		channel->stop();
	}

	void Audio::NIKEChannel::setPaused(bool state) {
		channel->setPaused(state);
	}

	bool Audio::NIKEChannel::getPaused() {
		bool state;
		channel->getPaused(&state);
		return state;
	}

	bool Audio::NIKEChannel::isPlaying() {
		bool state;
		channel->isPlaying(&state);
		return state;
	}

	void Audio::NIKEChannel::setVolume(float vol) {
		channel->setVolume(vol);
	}

	float Audio::NIKEChannel::getVolume() const {
		float vol;
		channel->getVolume(&vol);
		return vol;
	}

	void Audio::NIKEChannel::setPitch(float pitch) {
		channel->setPitch(pitch);
	}

	float Audio::NIKEChannel::getPitch() const {
		float pitch;
		channel->getVolume(&pitch);
		return pitch;
	}

	void Audio::NIKEChannel::setLoopCount(int count) {
		channel->setLoopCount(count);
	}

	int Audio::NIKEChannel::getLoopCount() const {
		int count;
		channel->getLoopCount(&count);
		return count;
	}

	void Audio::NIKEChannel::setLoopPoints(unsigned int start, unsigned int end, NIKE_AUDIO_TIMEUNIT time_unit) {
		channel->setLoopPoints(start, time_unit, end, time_unit);
	}

	Vector2<unsigned int> Audio::NIKEChannel::getLoopPoints(NIKE_AUDIO_TIMEUNIT time_unit) const {
		Vector2<unsigned int> points;
		channel->getLoopPoints(&points.x, time_unit, &points.y, time_unit);
		return points;
	}

	void Audio::NIKEChannel::setMute(bool state) {
		channel->setMute(state);
	}

	bool Audio::NIKEChannel::getMute() const {
		bool state;
		channel->getMute(&state);
		return state;
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEChannel::getSound() const {
		return sound;
	}

	void Audio::NIKEChannel::setMode(NIKE_AUDIO_MODE mode) {
		channel->setMode(mode);
	}

	NIKE_AUDIO_MODE Audio::NIKEChannel::getMode() const {
		NIKE_AUDIO_MODE mode;
		channel->getMode(&mode);
		return mode;
	}

	void Audio::NIKEChannel::setPosition(unsigned int position, NIKE_AUDIO_TIMEUNIT time_unit) {
		channel->setPosition(position, time_unit);
	}

	unsigned int Audio::NIKEChannel::getPosition(NIKE_AUDIO_TIMEUNIT time_unit) const {
		unsigned int pos;
		channel->getPosition(&pos, time_unit);
		return pos;
	}

	void Audio::NIKEChannel::setChannelGroup(std::shared_ptr<Audio::IChannelGroup> group) {
		channel->setChannelGroup(std::static_pointer_cast<Audio::NIKEChannelGroup>(group)->getChannelGroup());
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::NIKEChannel::getChannelGroup() const {
		FMOD::ChannelGroup* group;
		channel->getChannelGroup(&group);

		return Audio::Service::convertChannelGroup(new Audio::NIKEChannelGroup(group));
	}

	/*****************************************************************//**
	* NIKE CHANNEL GROUP
	*********************************************************************/
	Audio::NIKEChannelGroup::NIKEChannelGroup(FMOD::ChannelGroup* group) 
		: group{ group } {}

	FMOD::ChannelGroup* Audio::NIKEChannelGroup::getChannelGroup() {
		return group;
	}

	void Audio::NIKEChannelGroup::stop() {
		group->stop();
	}

	void Audio::NIKEChannelGroup::setPaused(bool state) {
		group->setPaused(state);
	}

	bool Audio::NIKEChannelGroup::getPaused() {
		bool state;
		group->getPaused(&state);
		return state;
	}

	bool Audio::NIKEChannelGroup::isPlaying() {
		bool state;
		group->isPlaying(&state);
		return state;
	}

	void Audio::NIKEChannelGroup::setVolume(float vol) {
		group->setVolume(vol);
	}

	float Audio::NIKEChannelGroup::getVolume() const {
		float vol;
		group->getVolume(&vol);
		return vol;
	}

	void Audio::NIKEChannelGroup::setPitch(float pitch) {
		group->setPitch(pitch);
	}

	float Audio::NIKEChannelGroup::getPitch() const {
		float pitch;
		group->getPitch(&pitch);
		return pitch;
	}

	void Audio::NIKEChannelGroup::setVolumeRamp(bool ramp) {
		group->setVolumeRamp(ramp);
	}

	bool Audio::NIKEChannelGroup::getVolumeRamp() const {
		bool ramp;
		group->getVolumeRamp(&ramp);
		return ramp;
	}

	void Audio::NIKEChannelGroup::setMute(bool state) {
		group->setMute(state);
	}

	bool Audio::NIKEChannelGroup::getMute() const {
		bool state;
		group->getMute(&state);
		return state;
	}

	void Audio::NIKEChannelGroup::setMode(NIKE_AUDIO_MODE mode) {
		group->setMode(mode);
	}

	NIKE_AUDIO_MODE Audio::NIKEChannelGroup::getMode() const {
		NIKE_AUDIO_MODE mode;
		group->getMode(&mode);
		return mode;
	}

	int Audio::NIKEChannelGroup::getNumChannels() const {
		int channel_count;
		group->getNumChannels(&channel_count);
		return channel_count;
	}

	void Audio::NIKEChannelGroup::addChildGroup(std::shared_ptr<Audio::IChannelGroup> child_group) {
		group->addGroup(std::static_pointer_cast<Audio::NIKEChannelGroup>(child_group)->getChannelGroup(), false);
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::NIKEChannelGroup::getChildGroup(int index) const {
		FMOD::ChannelGroup* parent_group;
		group->getGroup(index, &parent_group);
		return Audio::Service::convertChannelGroup(new Audio::NIKEChannelGroup(parent_group));
	}

	/*****************************************************************//**
	* NIKE AUDIO SYSTEM
	*********************************************************************/
	Audio::NIKEAudioSystem::NIKEAudioSystem() {
		//Configure FMOD Setup
		configAudio();
	}

	void Audio::NIKEAudioSystem::configAudio() {
		FMOD_RESULT result;
		result = FMOD::System_Create(&fmod_system);
		// Check if fmod studio system is created
		if (result != FMOD_OK)
		{
			LOG_CRASH("CREATION OF FMOD STUDIO SYSTEM FAILED!");
			exit(-1);
		}

		result = fmod_system->init(512, FMOD_INIT_NORMAL, nullptr);
		// Check if fmod studio system is initialized
		if (result != FMOD_OK)
		{
			LOG_CRASH("INITIALIZATION OF STUDIO FMOD SYSTEM FAILED!");
			exit(-1);
		}
		else {
			NIKEE_CORE_INFO("FMOD init success");
		}
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEAudioSystem::createSound(std::string const& file_path) {
		
		//FMOD Loading Variables
		FMOD::Sound* temp_audio = nullptr;
		FMOD_RESULT result;

		// Create sound to be pushed into container
		result = fmod_system->createSound(file_path.c_str(), NIKE_AUDIO_DEFAULT, nullptr, &temp_audio);

		// Check for audio file validadity
		if (result != FMOD_OK)
		{
			LOG_CRASH("INVALID FILE PATH.");
		}

		NIKEE_CORE_INFO("Sucessfully loaded sound from " + file_path);

		return std::make_shared<Audio::NIKEAudio>(temp_audio, file_path);
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEAudioSystem::createSound(const char* name_or_data, unsigned int mode, void* exinfo) {
		//FMOD Loading Variables
		FMOD::Sound* temp_audio = nullptr;
		FMOD_RESULT result;

		// Create sound to be pushed into container
		result = fmod_system->createSound(name_or_data, mode, static_cast<FMOD_CREATESOUNDEXINFO*>(exinfo), &temp_audio);

		// Check for audio file validadity
		if (result != FMOD_OK)
		{
			LOG_CRASH("INVALID FILE PATH.");
		}

		return std::make_shared<Audio::NIKEAudio>(temp_audio, "");
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEAudioSystem::createStream(std::string const& file_path) {
		//FMOD Loading Variables
		FMOD::Sound* temp_audio = nullptr;
		FMOD_RESULT result;

		// Create sound to be pushed into container
		// !!! FMOD will handle the reading from file using createStream function
		result = fmod_system->createStream(file_path.c_str(), NIKE_AUDIO_CREATESTREAM, nullptr, &temp_audio);

		// Check for audio file validadity
		if (result != FMOD_OK)
		{
			LOG_CRASH("INVALID FILE PATH.");
		}

		NIKEE_CORE_INFO("Sucessfully loaded music from " + file_path);

		return std::make_shared<Audio::NIKEAudio>(temp_audio, file_path);
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEAudioSystem::createStream(const char* name_or_data, unsigned int mode, void* exinfo) {
		//FMOD Loading Variables
		FMOD::Sound* temp_audio = nullptr;
		FMOD_RESULT result;

		//Cast audio info
		auto audio_info = static_cast<FMOD_CREATESOUNDEXINFO*>(exinfo);

		// Create sound to be pushed into container
		result = fmod_system->createStream(name_or_data, mode, audio_info, &temp_audio);

		// Check for audio file validadity
		if (result != FMOD_OK)
		{
			LOG_CRASH("FAILED TO CREATE STREAM");
		}

		return std::make_shared<Audio::NIKEAudio>(temp_audio, "");
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::NIKEAudioSystem::createChannelGroup(std::string const& identifier) {
		//FMOD Group loading Variables
		FMOD::ChannelGroup* temp = nullptr;
		FMOD_RESULT result;
		result = fmod_system->createChannelGroup(identifier.c_str(), &temp);
		if (result != FMOD_OK)
		{
			LOG_CRASH("AUDIO GROUP NOT INITIALIZED");
		}
		
		return std::make_shared<Audio::NIKEChannelGroup>(temp);
	}

	std::shared_ptr<Audio::IChannel> Audio::NIKEAudioSystem::playSound(std::shared_ptr<Audio::IAudio> audio, std::shared_ptr<Audio::IChannelGroup> channel_group, bool start_paused) {
		
		//Channel & Audio Result checking
		FMOD::Channel* temp_channel = nullptr;
		FMOD_RESULT result;

		//Play audio
		result = fmod_system->playSound(
				std::static_pointer_cast<NIKEAudio>(audio)->getAudio(),
				std::static_pointer_cast<NIKEChannelGroup>(channel_group)->getChannelGroup(),
				start_paused,
				&temp_channel);

		//Check if audio successfully played and added to relevant channels & channel groups
		if (result == FMOD_OK) {
			return std::make_shared<NIKEChannel>(temp_channel, audio);
		}
		else {
			return nullptr;
		}
	}

	void Audio::NIKEAudioSystem::update() {
		fmod_system->update();
	}

	void Audio::NIKEAudioSystem::shutdown() {
		if (fmod_system) {
			fmod_system->close();
			fmod_system->release();
			fmod_system = nullptr;
		}
	}

	/*****************************************************************//**
	* NIKE AUDIO SERVICE
	*********************************************************************/
	//Definition of static groups
	std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>> NIKE::Audio::Service::channel_groups;

	void Audio::Service::onEvent(std::shared_ptr<Audio::PausedEvent> event) {
		
		if (event->b_game_state) {
			resumeAllChannels();
		}
		else {
			pauseAllChannels();
		}

		event->setEventProcessed(true);
	}

	void NIKE::Audio::Service::init(nlohmann::json const& config) {

		//Setup events listening
		std::shared_ptr<Audio::Service> audio_sys_wrapped(this, [](Audio::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Audio::PausedEvent>(audio_sys_wrapped);

		//Create audio system
		audio_system = std::make_shared<Audio::NIKEAudioSystem>();

		try {
			auto const& data = config.at("AudioConfig");

			bgm_channel_group_id = data.value("BGM Channel Group", "BGM");
			sfx_channel_group_id = data.value("SFX Channel Group", "SFX");

			createChannelGroup(bgm_channel_group_id);
			createChannelGroup(sfx_channel_group_id);
		}
		catch (const nlohmann::json::exception& e) {
			NIKEE_CORE_WARN(e.what());
			NIKEE_CORE_WARN("Audio config invalid!");
		}
	}

	std::shared_ptr<Audio::IAudioSystem> NIKE::Audio::Service::getAudioSystem() const {
		return audio_system;
	}

	void Audio::Service::createChannelGroup(std::string const& channel_group_id) {
		// Check if the group already exists in the map
		if (channel_groups.find(channel_group_id) != channel_groups.end())
		{
			NIKEE_CORE_ERROR("Error: Channel Group '{}' already exists! Skipping.", channel_group_id);
			return;
		}

		//Emplace into audio group list
		channel_groups.emplace(std::piecewise_construct, std::forward_as_tuple(channel_group_id), std::forward_as_tuple(std::move(std::static_pointer_cast<Audio::NIKEChannelGroup>(audio_system->createChannelGroup(channel_group_id)))));
		//Create a playlist for each group
		createChannelPlaylist(channel_group_id);
	}

	void Audio::Service::unloadChannelGroup(std::string const& channel_group_id) {
		auto it = channel_groups.find(channel_group_id);

		//Check if group exists
		if (it == channel_groups.end())
		{
			NIKEE_CORE_ERROR("Error: Channel Group '{}' does not exists! Skipping.", channel_group_id);
			return;
		}

		//Unload channel group
		auto group = std::static_pointer_cast<Audio::NIKEChannelGroup>(it->second)->getChannelGroup();
		if (group) {
			group->stop();
			group->release();
		}
		//Erase channel group from map
		it = channel_groups.erase(it);
		//Erase channel playlist
		channel_playlists.erase(channel_group_id);
	}

	void Audio::Service::clearAllChannelGroups() {
		//Clear channel groups
		for (auto it = channel_groups.begin(); it != channel_groups.end(); ) {

			//Skip static channels groups
			if (it->first == bgm_channel_group_id || it->first == sfx_channel_group_id) {
				++it;
				continue;
			}

			//Get channel group
			auto channel_group = std::static_pointer_cast<Audio::NIKEChannelGroup>(it->second)->getChannelGroup();
			if (channel_group) {
				channel_group->stop();
				channel_group->release();
			}
			//Erase channel playlist
			channel_playlists.erase(channel_playlists.find(it->first));

			// Erase the channel group from the map
			it = channel_groups.erase(it);
		}
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::Service::convertChannelGroup(Audio::IChannelGroup*&& group) {
		//Find channel group shared pointer & return the valid group shared pointer ( For usage within derived audio classes )
		for (auto& grp : channel_groups) {
			if (std::static_pointer_cast<Audio::NIKEChannelGroup>(grp.second)->getChannelGroup() == static_cast<Audio::NIKEChannelGroup*>(group)->getChannelGroup()) {
				delete group;
				return grp.second;
			}
		}

		delete group;
		return nullptr;
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::Service::getChannelGroup(std::string const& channel_group_id) {
		//Find Channel Group
		auto it = channel_groups.find(channel_group_id);

		// Check if the group already exists in the map
		if (it == channel_groups.end())
		{
			//NIKEE_CORE_WARN("AUDIO GROUP DOES NOT EXISTS");
			return nullptr;
		}

		//Return channel group
		return it->second;
	}

	std::shared_ptr<Audio::IChannel> Audio::Service::getChannel(std::string const& channel_id) {
		//Find channel
		auto it = channels.find(channel_id);

		// Check if the group already exists in the map
		if (it == channels.end())
		{
			NIKEE_CORE_ERROR("CHANNEL DOES NOT EXISTS, OR HAS BEEN DELETED");
			return nullptr;
		}

		//Return channel group
		return it->second;
	}

	std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>>& Audio::Service::getAllChannelGroups()
	{
		return channel_groups;
	}

	bool Audio::Service::checkChannelGroupExist(std::string const& channel_id)
	{
		if (channel_groups.find(channel_id) == channel_groups.end())
		{
			return false;
		}
		return true;
	}

	bool Audio::Service::checkChannelExist(std::string const& channel_id)
	{
		if (channels.find(channel_id) == channels.end())
		{
			return false;
		}
		return true;
	}

	std::string Audio::Service::getBGMChannelGroupID() const {
		return bgm_channel_group_id;
	}

	std::string Audio::Service::getSFXChannelGroupID() const {
		return sfx_channel_group_id;
	}

	void Audio::Service::playAudio(std::string const& audio_id, std::string const& channel_id, std::string const& channel_group_id, float vol, float pitch, bool loop, bool is_music, bool start_paused) {
		
		// Retrieve audio asset
		std::shared_ptr<Audio::IAudio> audio_asset = NIKE_ASSETS_SERVICE->getAsset<Audio::IAudio>(audio_id);

		// Check if the asset is valid
		if (!audio_asset) {
			NIKEE_CORE_ERROR("Failed to retrieve audio asset: {}", audio_id);
			return;
		}


		//Play sound & get channel that sound is playing under
		std::shared_ptr<Audio::IChannel> new_channel = audio_system->playSound(is_music ? audio_asset : audio_asset, getChannelGroup(channel_group_id), start_paused);

		//Add channel to the channel map
		if (new_channel) {
			//Set channel group pitch & volume
			new_channel->setVolume(vol);
			new_channel->setPitch(pitch);

			//Set channel looping mode
			loop ? new_channel->setMode(NIKE_AUDIO_LOOP_NORMAL) : new_channel->setMode(NIKE_AUDIO_LOOP_OFF);

			//Add channel into channel map if channel id path is specified & channel is in looping mode
			if(channel_id != "" && (new_channel->getMode() & NIKE_AUDIO_LOOP_NORMAL))
			channels[channel_id] = std::move(new_channel);
		}
		else {
			NIKEE_CORE_ERROR("Error playing audio in channel!");
		}

	}

	void Audio::Service::pauseAllChannels() {
		for (auto& pair : getAllChannelGroups()) {
			pair.second->setPaused(true);
		}
	}

	void Audio::Service::resumeAllChannels() {
		for (auto& pair : getAllChannelGroups()) {
			pair.second->setPaused(false);
		}
	}

	/*****************************************************************//**
	* Playlist Management
	*********************************************************************/

	void Audio::Service::createChannelPlaylist(const std::string& channel_id) {
		if (channel_playlists.find(channel_id) != channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Channel Playlist already exists! Skipping.");
			return;
		}
		channel_playlists[channel_id] = Playlist{ {}, false};
	}

	const Audio::Service::Playlist& Audio::Service::getChannelPlaylist(const std::string& channel_id) {
		// Find channel's playlist
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			LOG_CRASH("Error: Unable to find channel's playlist");
		}
		return it->second;
	}

	void Audio::Service::assignTracksToPlaylist(const std::string& channel_id, const std::deque<std::string>& new_tracks) {
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return;
		}
		it->second.tracks = new_tracks;
	}

	void Audio::Service::queueAudioToPlaylist(const std::string& channel_id, const std::string& audio_id) {
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return;
		}
		it->second.tracks.push_back(audio_id);
	}

	void Audio::Service::popAudioFromPlaylist(const std::string& channel_id) {
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return;
		}
		it->second.tracks.pop_front();
	}

	void Audio::Service::setPlaylistLoop(const std::string& channel_id, bool loop) {
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return;
		}
		it->second.loop = loop;
	}

	bool Audio::Service::isPlaylistLooping(const std::string& channel_id) const{
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return false;
		}
		return it->second.loop;
	}

	void Audio::Service::clearPlaylist(const std::string& channel_id) {
		auto it = channel_playlists.find(channel_id);
		if (it == channel_playlists.end()) {
			NIKEE_CORE_ERROR("Error: Unable to find Channel Playlist!");
			return;
		}
		it->second.tracks.clear();
	}

	void Audio::Service::update() {
		audio_system->update();

		//Iterate through the map and remove inactive channels
		for (auto it = channels.begin(); it != channels.end(); ) {
			if (!it->second->isPlaying()) {
				//Erase the inactive channel
				it = channels.erase(it);
			}
			else {
				++it;
			}
		}
	
	}
	// Serialize audio channels and data
	nlohmann::json Audio::Service::serializeAudioChannels() const {
		nlohmann::json channels_data;
		for (const auto& [channel_name, playlist] : channel_playlists) {
			nlohmann::json playlist_data;

			playlist_data["tracks"] = playlist.tracks;
			playlist_data["loop"] = playlist.loop;

			channels_data[channel_name] = playlist_data;
		}
		return channels_data;
	}

	// Deserialize audio channels and data
	void Audio::Service::deserializeAudioChannels(nlohmann::json const& data) {
		clearAllChannelGroups();

		// Deserialize new channels and playlists
		for (const auto& [channel_name, channel_data] : data.items()) {
			// Create new channel group
			createChannelGroup(channel_name);

			// Set playlist loop if specified
			if (channel_data.contains("loop")) {
				setPlaylistLoop(channel_name, channel_data["loop"].get<bool>());
			}

			// Assign tracks to the playlist if specified
			if (channel_data.contains("tracks")) {
				assignTracksToPlaylist(channel_name, channel_data["tracks"].get<std::deque<std::string>>());
			}
		}
	}

}
