/*****************************************************************//**
 * \file   sysAudio.cpp
 * \brief  Audio manager function definitions 
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sAudio.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* NIKE AUDIO
	*********************************************************************/
	Audio::NIKEAudio::NIKEAudio(FMOD::Sound* sound)
		:sound{ sound } {}

	FMOD::Sound* Audio::NIKEAudio::getSound() {
		return sound;
	}

	void Audio::NIKEAudio::release() {
		sound->release();
	}

	unsigned int Audio::NIKEAudio::getLength() const {
		unsigned int length;
		sound->getLength(&length, FMOD_TIMEUNIT_MS);
		return length;
	}

	void Audio::NIKEAudio::setMode(FMOD_MODE mode) {
		sound->setMode(mode);
	}

	unsigned int Audio::NIKEAudio::getMode() const{
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
	Audio::NIKEChannel::NIKEChannel(FMOD::Channel* channel)
		: channel{channel} {}

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

	void Audio::NIKEChannel::setMode(NIKE_AUDIO_MODE mode) {
		channel->setMode(mode);
	}

	NIKE_AUDIO_MODE Audio::NIKEChannel::getMode() const {
		NIKE_AUDIO_MODE mode;
		channel->getMode(&mode);
		return mode;
	}

	void Audio::NIKEChannel::setChannelGroup(Audio::IChannelGroup* group) {
		channel->setChannelGroup(static_cast<Audio::NIKEChannelGroup*>(group)->getChannelGroup());
	}

	Audio::IChannelGroup Audio::NIKEChannel::getChannelGroup() const {
		FMOD::ChannelGroup* group;
		channel->getChannelGroup(&group);
		return Audio::NIKEChannelGroup(group);
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

	void Audio::NIKEChannelGroup::addChildGroup(std::shared_ptr<FMOD::ChannelGroup> group) {
		group->addGroup(group.get(), false);
	}

	std::shared_ptr<FMOD::ChannelGroup> Audio::NIKEChannelGroup::getChildGroup(int index) const {
		FMOD::ChannelGroup* parent_group;
		group->getGroup(index, &parent_group);
		return std::shared_ptr<FMOD::ChannelGroup>(parent_group);
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
		else {
			cout << "FMOD SYS CREATED!" << endl;
		}

		result = fmod_system->init(512, FMOD_INIT_NORMAL, 0);
		// Check if fmod studio system is initialized
		if (result != FMOD_OK)
		{
			cerr << "INITIALIZATION OF STUDIO FMOD SYSTEM FAILED!" << endl;
			exit(-1);
		}
		else {
			cout << "FMOD INITIALIZED!" << endl;
		}
	}

	std::shared_ptr<Audio::IAudio> Audio::NIKEAudioSystem::createSound(std::string const& file_path) {
		
		//FMOD Loading Variables
		FMOD::Sound* temp_audio = nullptr;
		FMOD_RESULT result;

		// Create sound to be pushed into container
		result = fmod_system->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &temp_audio);

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
		result = fmod_system->createStream(file_path.c_str(), FMOD_CREATESTREAM, nullptr, &temp_audio);

		// Check for audio file validadity
		if (result != FMOD_OK)
		{
			throw std::runtime_error("INVALID FILE PATH.");
		}

		return std::make_shared<Audio::NIKEAudio>(temp_audio);
	}

	Audio::NIKEAudioSystem::~NIKEAudioSystem() {
		// Release fmod studio
		fmod_system->release();
	}

	/*****************************************************************//**
	* NIKE AUDIO SERVICE
	*********************************************************************/
	//Definition of static groups
	std::unordered_map<std::string, std::shared_ptr<Audio::NIKEChannelGroup>> NIKESAURUS::Audio::Service::groups;

	std::shared_ptr<Audio::NIKEChannelGroup> Audio::Service::convertToShared(Audio::NIKEChannelGroup* group) {
		for (auto& grp : groups) {
			if (grp.second->getChannelGroup() == group->getChannelGroup())
				return grp.second;
		}

		return nullptr;
	}

	void Audio::NIKEAudioSystem::playAudio() {
		fmod_system->playSound(audio.get(), group.get(), 0, nullptr);
		cout << "AUDIO PLAY" << endl;

		group->setVolume(vol);
		group->setPitch(pitch);
		if (loop)
		{
			group->setMode(FMOD_LOOP_NORMAL);
		}
		else {
			group->setMode(FMOD_LOOP_OFF);
		}
	}

	void Audio::Service::NEAudioPlay(NE_AUDIO audio, NE_AUDIO_GROUP group, float vol, float pitch, bool loop)
	{
		fmod_system->playSound(audio.get(), group.get(), 0, nullptr);
		cout << "AUDIO PLAY" << endl;

		group->setVolume(vol);
		group->setPitch(pitch);
		if (loop)
		{
			group->setMode(FMOD_LOOP_NORMAL);
		}
		else {
			group->setMode(FMOD_LOOP_OFF);
		}
	}

	void Audio::Service::NEAudioStopGroup(NE_AUDIO_GROUP group)
	{
		// This is same as UNREFERENCED_PARAMETER
		cout << "AUDIO STOPPED" << endl;
		group->stop();
	}

	void Audio::Service::NEAudioPauseGroup(NE_AUDIO_GROUP group)
	{
		group->setPaused(true);
	}

	void Audio::Service::NEAudioResumeGroup(NE_AUDIO_GROUP group)
	{
		group->setPaused(false);
	}

	void Audio::Service::NEAudioSetGroupPitch(NE_AUDIO_GROUP group, float pitch)
	{
		group->setPitch(pitch);
	}

	void Audio::Service::NEAudioSetGroupVolume(NE_AUDIO_GROUP group, float vol)
	{
		group->setVolume(vol);
	}

	void Audio::Service::NEAudioUnloadGroup(NE_AUDIO_GROUP group)
	{
		group->release();
	}

	void Audio::Service::NEAudioUnloadAudio(NE_AUDIO audio)
	{
		audio->release();
	}

	void Audio::Service::IsPlaying(NE_AUDIO audio)
	{
		static_cast<void>(audio);
	}
}
