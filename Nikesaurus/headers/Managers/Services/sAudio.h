/*****************************************************************//**
 * \file   sAudio.h
 * \brief  Audio system manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (35%)
 * \co-author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (35%)
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu (30%)
 * \date   September 2024
 *  All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SERVICE_AUDIO_HPP
#define SERVICE_AUDIO_HPP

#include "Components/cAudio.h"

namespace NIKE {
	namespace Audio {
		//Temporary Disable DLL Export Warning
#pragma warning(disable: 4251)

/*****************************************************************//**
* Abstract Audio Classes
*********************************************************************/

//Abstract audio class
		class IAudio {
		private:
		public:
			//Defaults
			IAudio() = default;
			virtual ~IAudio() = default;

			//Lock audio
			virtual void lock(unsigned int offset, unsigned int length, void** ptr1, void** ptr2, unsigned int* len1, unsigned int* len2) = 0;

			//Unlock audio
			virtual void unlock(void* ptr1, void* ptr2, unsigned int len1, unsigned int len2) = 0;

			//Release Audio
			virtual void release() = 0;

			//Get audio file path
			virtual std::string getFilePath() const = 0;

			//Get length of audio
			virtual unsigned int getLength(NIKE_AUDIO_TIMEUNIT time_unit) const = 0;

			//Set audio mode
			virtual void setMode(NIKE_AUDIO_MODE mode) = 0;

			//Get audio mode
			virtual NIKE_AUDIO_MODE getMode() const = 0;

			//Set audio loop count
			virtual void setLoopCount(int count) = 0;

			//Get audio loop count
			virtual int getLoopCount() const = 0;

			//Set audio loop point ( Milliseconds )
			virtual void setLoopPoints(unsigned int start, unsigned int end) = 0;

			//Get audio loop point ( Milliseconds )
			virtual Vector2<unsigned int> getLoopPoints() const = 0;
		};

		//Abstract channel group class
		class IChannelGroup {
		private:
		public:
			//Defaults
			IChannelGroup() = default;
			virtual ~IChannelGroup() = default;

			//Stop audio playing in channel group
			virtual void stop() = 0;

			//Set state of channel group
			virtual void setPaused(bool state) = 0;

			//Get state of channel group
			virtual bool getPaused() = 0;

			//Check if channel group is playing
			virtual bool isPlaying() = 0;

			//Set channel group volume
			virtual void setVolume(float vol) = 0;

			//Get channel group volume
			virtual float getVolume() const = 0;

			//Set channel group pitch
			virtual void setPitch(float vol) = 0;

			//Get channel group pitch
			virtual float getPitch() const = 0;

			//Set channel group vol ramp
			virtual void setVolumeRamp(bool ramp) = 0;

			//Get channel group vol ramp
			virtual bool getVolumeRamp() const = 0;

			//Set channel group mute
			virtual void setMute(bool state) = 0;

			//Get channel group mute state
			virtual bool getMute() const = 0;

			//Set channel group mode
			virtual void setMode(NIKE_AUDIO_MODE mode) = 0;

			//Get channel group mode
			virtual NIKE_AUDIO_MODE getMode() const = 0;

			//Get number of channels in channel group
			virtual int getNumChannels() const = 0;

			//Add child channel group
			virtual void addChildGroup(std::shared_ptr<Audio::IChannelGroup> child_group) = 0;

			//Get parent channel group
			virtual std::shared_ptr<Audio::IChannelGroup> getChildGroup(int index) const = 0;

		};

		//Abstract channel class
		class IChannel {
		private:
		public:
			//Defaults
			IChannel() = default;
			virtual ~IChannel() = default;

			//Stop audio playing in channel
			virtual void stop() = 0;

			//Set state of channel
			virtual void setPaused(bool state) = 0;

			//Get state of channel
			virtual bool getPaused() = 0;

			//Check if channel is playing
			virtual bool isPlaying() = 0;

			//Set channel volume
			virtual void setVolume(float vol) = 0;

			//Get channel volume
			virtual float getVolume() const = 0;

			//Set channel pitch
			virtual void setPitch(float vol) = 0;

			//Get channel pitch
			virtual float getPitch() const = 0;

			//Set channel loop count ( -1 loop indefinitely )
			virtual void setLoopCount(int count) = 0;

			//Get channel loop count
			virtual int getLoopCount() const = 0;

			//Set channel loop point ( Milliseconds )
			virtual void setLoopPoints(unsigned int start, unsigned int end, NIKE_AUDIO_TIMEUNIT time_unit) = 0;

			//Get audio loop point ( Milliseconds )
			virtual Vector2<unsigned int> getLoopPoints(NIKE_AUDIO_TIMEUNIT time_unit) const = 0;

			//Set channel mute
			virtual void setMute(bool state) = 0;

			//Get channel mute state
			virtual bool getMute() const = 0;

			//Set channel mode
			virtual void setMode(NIKE_AUDIO_MODE mode) = 0;

			//Get channel mode
			virtual NIKE_AUDIO_MODE getMode() const = 0;

			//Set channel position
			virtual void setPosition(unsigned int position, NIKE_AUDIO_TIMEUNIT time_unit) = 0;

			//Get channel position
			virtual unsigned int getPosition(NIKE_AUDIO_TIMEUNIT time_unit) const = 0;

			//Get sound in channel
			virtual std::shared_ptr<IAudio> getSound() const = 0;

			//Set channel group
			virtual void setChannelGroup(std::shared_ptr<Audio::IChannelGroup> group) = 0;

			//Get channel group
			virtual std::shared_ptr<Audio::IChannelGroup> getChannelGroup() const = 0;
		};

		//Abstract audio system class
		class IAudioSystem {
		private:
		public:
			//Defaults
			IAudioSystem() = default;
			virtual ~IAudioSystem() = default;

			//Create Sound Audio
			virtual std::shared_ptr<Audio::IAudio> createSound(std::string const& file_path) = 0;

			//Create Sound audio ( Manual )
			virtual std::shared_ptr<Audio::IAudio> createSound(const char* name_or_data, unsigned int mode, void* exinfo) = 0;

			//Create Stream Audio ( For music )
			virtual std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) = 0;

			//Create stream audio ( Manual )
			virtual std::shared_ptr<Audio::IAudio> createStream(const char* name_or_data, unsigned int mode, void* exinfo) = 0;

			//Create channel group
			virtual std::shared_ptr<Audio::IChannelGroup> createChannelGroup(std::string const& identifier) = 0;

			//Play audio
			virtual std::shared_ptr<Audio::IChannel> playSound(std::shared_ptr<Audio::IAudio> audio, std::shared_ptr<Audio::IChannelGroup> channel_group, bool start_paused = false) = 0;

			//Update audio system regularly
			virtual void update() = 0;

			//Close & release audio system resources
			virtual void shutdown() = 0;
		};

		/*****************************************************************//**
		* DLL Build Implementation
		*********************************************************************/
#ifdef NIKE_BUILD_DLL //Expose implementation only to NIKE Engine

		//NIKE Audio
		class NIKEAudio : public IAudio {
		private:
			FMOD::Sound* sound{ nullptr };
			std::string file_path;

		public:
			NIKEAudio(FMOD::Sound* sound, const std::string& path);
			~NIKEAudio() = default;

			FMOD::Sound* getAudio();

			void lock(unsigned int offset, unsigned int length, void** ptr1, void** ptr2, unsigned int* len1, unsigned int* len2) override;

			void unlock(void* ptr1, void* ptr2, unsigned int len1, unsigned int len2) override;

			void release() override;

			std::string getFilePath() const override;

			unsigned int getLength(NIKE_AUDIO_TIMEUNIT time_unit) const override;

			void setMode(NIKE_AUDIO_MODE) override;

			NIKE_AUDIO_MODE getMode() const override;

			void setLoopCount(int count) override;

			int getLoopCount() const override;

			void setLoopPoints(unsigned int start, unsigned int end) override;

			Vector2<unsigned int> getLoopPoints() const override;
		};

		//NIKE Audio Group
		class NIKEChannelGroup : public IChannelGroup {
		private:
			FMOD::ChannelGroup* group{ nullptr };
		public:
			NIKEChannelGroup(FMOD::ChannelGroup* group);
			~NIKEChannelGroup() = default;

			FMOD::ChannelGroup* getChannelGroup();

			void stop() override;

			void setPaused(bool state) override;

			bool getPaused() override;

			bool isPlaying() override;

			void setVolume(float vol) override;

			float getVolume() const override;

			void setPitch(float pitch) override;

			float getPitch() const override;

			void setVolumeRamp(bool ramp) override;

			bool getVolumeRamp() const override;

			void setMute(bool state) override;

			bool getMute() const override;

			void setMode(NIKE_AUDIO_MODE mode) override;

			NIKE_AUDIO_MODE getMode() const override;

			int getNumChannels() const override;

			void addChildGroup(std::shared_ptr<Audio::IChannelGroup> child_group) override;

			std::shared_ptr<Audio::IChannelGroup> getChildGroup(int index) const override;

		};

		//NIKE Audio Group
		class NIKEChannel : public IChannel {
		private:
			FMOD::Channel* channel{ nullptr };

			//Pointer to sound its currently playing
			std::shared_ptr<IAudio> sound;
		public:
			NIKEChannel(FMOD::Channel* channel, std::shared_ptr<IAudio> sound);

			FMOD::Channel* getChannel();

			void stop() override;

			void setPaused(bool state) override;

			bool getPaused() override;

			bool isPlaying() override;

			void setVolume(float vol) override;

			float getVolume() const override;

			void setPitch(float pitch) override;

			float getPitch() const override;

			void setLoopCount(int count) override;

			int getLoopCount() const override;

			void setLoopPoints(unsigned int start, unsigned int end, NIKE_AUDIO_TIMEUNIT time_unit) override;

			Vector2<unsigned int> getLoopPoints(NIKE_AUDIO_TIMEUNIT time_unit) const override;

			void setMute(bool state) override;

			bool getMute() const override;

			void setMode(NIKE_AUDIO_MODE mode) override;

			NIKE_AUDIO_MODE getMode() const override;

			void setPosition(unsigned int position, NIKE_AUDIO_TIMEUNIT time_unit) override;

			unsigned int getPosition(NIKE_AUDIO_TIMEUNIT time_unit) const override;

			std::shared_ptr<IAudio> getSound() const override;

			void setChannelGroup(std::shared_ptr<Audio::IChannelGroup> group) override;

			std::shared_ptr<Audio::IChannelGroup> getChannelGroup() const override;
		};

		//NIKE Audio System
		class NIKEAudioSystem : public IAudioSystem {
		private:
			//Fmod System
			FMOD::System* fmod_system{ nullptr };

			//FMOD Config
			void configAudio();
		public:
			NIKEAudioSystem();
			~NIKEAudioSystem() = default;

			std::shared_ptr<Audio::IAudio> createSound(std::string const& file_path) override;

			std::shared_ptr<Audio::IAudio> createSound(const char* name_or_data, unsigned int mode, void* exinfo) override;

			std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) override;

			std::shared_ptr<Audio::IAudio> createStream(const char* name_or_data, unsigned int mode, void* exinfo) override;

			std::shared_ptr<Audio::IChannelGroup> createChannelGroup(std::string const& identifier) override;

			std::shared_ptr<Audio::IChannel> playSound(std::shared_ptr<Audio::IAudio> audio, std::shared_ptr<Audio::IChannelGroup> channel_group, bool start_paused) override;

			void update() override;

			void shutdown() override;
		};

#endif //Expose implementation only to NIKE Engine

		/*****************************************************************//**
		* Audio Service
		*********************************************************************/

		class NIKE_API Service
			: public Events::IEventListener<Audio::PausedEvent> {

		private:
			// Delete Copy Constructor & Copy Assignment
			Service(Service const& rhs) = delete;
			void operator=(Service const& copy) = delete;

			// On Pause event;
			void onEvent(std::shared_ptr<Audio::PausedEvent> event) override;

			//Audio System
			std::shared_ptr<Audio::IAudioSystem> audio_system;

			//Map of channels
			std::unordered_map<std::string, std::shared_ptr<Audio::IChannel>> channels;

			//Map of groups
			static std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>> channel_groups;

			//Static channel group ID
			std::string bgm_channel_group_id;
			std::string sfx_channel_group_id;
			// Turns out "channel group" is just a singular channel so need to add another "channel" group to have 2 tracks playing together
			std::string bgmc_channel_group_id;

			// Define the global volume variables.
			float gGlobalBGMVolume = 0.5f;  // Default volume for BGM (range 0.0 - 1.0)
			float gGlobalSFXVolume = 0.5f;  // Default volume for SFX (range 0.0 - 1.0)
			// BGM fading variables
			// In Audio::Service private section:
			float bgmFadeDuration = 0.0f;       // Total fade duration in seconds.
			float bgmFadeTimeRemaining = 0.0f;    // Time remaining in the fade.
			bool  bgmFadeInProgress = false;      // True if a fade is active.
			bool  bgmFadingIn = false;            // True for fade in, false for fade out.
			float bgmFadeStartVolume = 0.0f;      // Volume when fade started.
			float bgmFadeTargetVolume = 0.0f;     // Volume we want to reach at the end.

			// Playlist Management
			struct Playlist {
				std::deque<std::string> tracks;
				bool loop = false;
			};

			//Queue for each channel's playlist
			std::unordered_map<std::string, Playlist> channel_playlists;

		public:

			//Default Constructor
			Service() = default;

			//Destructor
			~Service() = default;

			//Init Audio Service
			void init(nlohmann::json const& config);

			//Get Audio System
			std::shared_ptr<Audio::IAudioSystem> getAudioSystem() const;

			//Create channel group
			void createChannelGroup(std::string const& channel_group_id);

			//Unload channel group
			void unloadChannelGroup(std::string const& channel_group_id);

			//Clean channel groups
			void clearAllChannelGroups();

			//Conversion from raw to shared pointer
			static std::shared_ptr<Audio::IChannelGroup> convertChannelGroup(Audio::IChannelGroup*&& group);

			//Get channel group
			std::shared_ptr<Audio::IChannelGroup> getChannelGroup(std::string const& channel_group_id);

			//Get channel
			std::shared_ptr<Audio::IChannel> getChannel(std::string const& channel_id);

			// Get all channels
			std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>>& getAllChannelGroups();

			// Checkers
			bool checkChannelGroupExist(std::string const& channel_id);
			bool checkChannelExist(std::string const& channel_id);

			//Get BGM Channel Group
			std::string getBGMChannelGroupID() const;

			//Get SFX Channel Group
			std::string getSFXChannelGroupID() const;

			//Getter for BGMC channel group ID.
			std::string getBGMCChannelGroupID() const;

			//Play Audio
			//Channel retrieval: channel_id has to be specified & bool loop has to be true ( channel_id = "" or loop = false, if retrieval is not needed )
			//Channel ID will override each other if the same id is specified more than once
			void playAudio(std::string const& audio_id, std::string const& channel_id, std::string const& channel_group_id, float vol, float pitch, bool loop, bool is_music, bool start_paused = false);
			
			float getGlobalBGMVolume() const;
			void setGlobalBGMVolume(float vol);

			float getGlobalSFXVolume() const;
			void setGlobalSFXVolume(float vol);

			// Fading BGM between scenes functions
			void BGMFadeIn(float fadeTime);
			void BGMFadeOut(float fadeTime);

			// Save volume settings...
			void saveAudioConfig(nlohmann::json& config_data);

			/**
			 * pauses all audio.
			 *
			 */
			void pauseAllChannels();

			/**
			 * resumes all audio.
			 *
			 */
			void resumeAllChannels();

			/*****************************************************************//**
			* Playlist Management
			*********************************************************************/
			// Create channel playlist and add to map
			void createChannelPlaylist(const std::string& channel_id);

			// Get individual channel's playlist
			const Playlist& getChannelPlaylist(const std::string& channel_id);

			void assignTracksToPlaylist(const std::string& channel_id, const std::deque<std::string>& new_tracks);

			// Queue audio to playlist
			void queueAudioToPlaylist(const std::string& channel_id, const std::string& audio_id);

			// Pop audio from playlist
			void popAudioFromPlaylist(const std::string& channel_id);

			// Enable or disable looping for a playlist
			void setPlaylistLoop(const std::string& channel_id, bool loop);

			// Check if a playlist is looping
			bool isPlaylistLooping(const std::string& channel_id) const;

			// Clear playlist
			void clearPlaylist(const std::string& channel_id);

			//Update Loop
			void update();

			// Serialize
			nlohmann::json serializeAudioChannels() const;

			// Deserialize
			void deserializeAudioChannels(nlohmann::json const& data);

			// Get the current BGM track for the scene.
			std::string getBGMTrackForScene();

			// Get current BGMC track for scene
			std::string getBGMCTrackForScene();

		};

		//Re-enable DLL Export warning
#pragma warning(default: 4251)
	}
}

#endif //!SERVICE_AUDIO_HPP
