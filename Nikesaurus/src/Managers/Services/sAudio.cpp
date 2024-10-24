/*****************************************************************//**
 * \file   sAudio.cpp
 * \brief  Audio manager function definitions 
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \co-author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sAudio.h"
#include "Managers/Services/Assets/sAssets.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* NIKE AUDIO
	*********************************************************************/
	Audio::NIKEAudio::NIKEAudio(FMOD::Sound* sound)
		:sound{ sound } {}

	FMOD::Sound* Audio::NIKEAudio::getAudio() {
		return sound;
	}

	void Audio::NIKEAudio::release() {
		sound->release();
	}

	unsigned int Audio::NIKEAudio::getLength() const {
		unsigned int length;
		sound->getLength(&length, NIKE_AUDIO_TIMEUNIT_MS);
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

	void Audio::NIKEChannel::setLoopPoints(unsigned int start, unsigned int end) {
		channel->setLoopPoints(start, NIKE_AUDIO_TIMEUNIT_MS, end, NIKE_AUDIO_TIMEUNIT_MS);
	}

	Vector2<unsigned int> Audio::NIKEChannel::getLoopPoints() const {
		Vector2<unsigned int> points;
		channel->getLoopPoints(&points.x, NIKE_AUDIO_TIMEUNIT_MS, &points.y, NIKE_AUDIO_TIMEUNIT_MS);
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
		group->getVolume(&pitch);
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
			cerr << "CREATION OF FMOD STUDIO SYSTEM FAILED!" << endl;
			exit(-1);
		}

		result = fmod_system->init(512, FMOD_INIT_NORMAL, nullptr);
		// Check if fmod studio system is initialized
		if (result != FMOD_OK)
		{
			cerr << "INITIALIZATION OF STUDIO FMOD SYSTEM FAILED!" << endl;
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
			throw std::runtime_error("INVALID FILE PATH.");
		}

		return std::make_shared<Audio::NIKEAudio>(temp_audio);
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
			throw std::runtime_error("INVALID FILE PATH.");
		}

		return std::make_shared<Audio::NIKEAudio>(temp_audio);
	}

	std::shared_ptr<Audio::IChannelGroup> Audio::NIKEAudioSystem::createChannelGroup(std::string const& identifier) {
		//FMOD Group loading Variables
		FMOD::ChannelGroup* temp = nullptr;
		FMOD_RESULT result;
		result = fmod_system->createChannelGroup(identifier.c_str(), &temp);
		if (result != FMOD_OK)
		{
			throw std::runtime_error("AUDIO GROUP NOT INITIALIZED");
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
	std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>> NIKESAURUS::Audio::Service::channel_groups;

	void NIKESAURUS::Audio::Service::setAudioSystem(std::shared_ptr<Audio::IAudioSystem> audio_sys) {
		audio_system = audio_sys;
	}

	std::shared_ptr<Audio::IAudioSystem> NIKESAURUS::Audio::Service::getAudioSystem() const {
		return audio_system;
	}

	void Audio::Service::createChannelGroup(std::string const& channel_group_id) {
		// Check if the group already exists in the map
		if (channel_groups.find(channel_group_id) != channel_groups.end())
		{
			throw std::runtime_error("AUDIO GROUP ALREADY EXISTS");
		}

		//Emplace into audio group list
		channel_groups.emplace(std::piecewise_construct, std::forward_as_tuple(channel_group_id), std::forward_as_tuple(std::move(std::static_pointer_cast<Audio::NIKEChannelGroup>(audio_system->createChannelGroup(channel_group_id)))));
	}

	void Audio::Service::unloadChannelGroup(std::string const& channel_group_id) {
		auto it = channel_groups.find(channel_group_id);

		//Check if group exists
		if (it == channel_groups.end())
		{
			throw std::runtime_error("AUDIO GROUP DOES NOT EXISTS");
		}

		//Unload channel group
		auto group = std::static_pointer_cast<Audio::NIKEChannelGroup>(it->second)->getChannelGroup();
		if (group) {
			group->stop();
			group->release();
		}
		//Erase channel group from map
		channel_groups.erase(it);
	}

	void Audio::Service::destroyChannelGroups() {
		//Clear channel groups
		for (auto it = channel_groups.begin(); it != channel_groups.end(); ) {
			std::static_pointer_cast<Audio::NIKEChannelGroup>(it->second)->getChannelGroup()->stop();
			std::static_pointer_cast<Audio::NIKEChannelGroup>(it->second)->getChannelGroup()->release();
		}

		//Clear groups
		channel_groups.clear();
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
			throw std::runtime_error("AUDIO GROUP DOES NOT EXISTS");
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
			throw std::runtime_error("CHANNEL DOES NOT EXISTS, OR HAS BEEN DELETED");
		}

		//Return channel group
		return it->second;
	}

	void Audio::Service::playAudio(std::string const& audio_id, std::string const& channel_id, std::string const& channel_group_id, float vol, float pitch, bool loop, bool start_paused) {
		
		//Get assets services
		auto assets_service = NIKEEngine.getService<Assets::Service>();

		//Play sound & get channel that sound is playing under
		std::shared_ptr<Audio::IChannel> new_channel = audio_system->playSound(assets_service->getAudio(audio_id), getChannelGroup(channel_group_id), start_paused);

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
			cout << "Error playing audio in channel!" << endl;
		}
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
}
