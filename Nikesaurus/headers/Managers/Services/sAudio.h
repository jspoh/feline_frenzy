/*****************************************************************//**
 * \file   sAudio.h
 * \brief  Audio system manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (35%)
 * \co-author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (35%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (30%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SERVICE_AUDIO_HPP
#define SERVICE_AUDIO_HPP

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

			//Release Audio
			virtual void release() = 0;

			//Get audio file path
			virtual std::string getFilePath() const = 0;

			//Get length of audio ( Milliseconds )
			virtual unsigned int getLength() const = 0;

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
			virtual void setLoopPoints(unsigned int start, unsigned int end) = 0;

			//Get audio loop point ( Milliseconds )
			virtual Vector2<unsigned int> getLoopPoints() const = 0;

			//Set channel mute
			virtual void setMute(bool state) = 0;

			//Get channel mute state
			virtual bool getMute() const = 0;

			//Set channel mode
			virtual void setMode(NIKE_AUDIO_MODE mode) = 0;

			//Get channel mode
			virtual NIKE_AUDIO_MODE getMode() const = 0;

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

			//Create Stream Audio ( For music )
			virtual std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) = 0;

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

			void release() override;

			std::string getFilePath() const override;

			unsigned int getLength() const override;

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

			void setLoopPoints(unsigned int start, unsigned int end) override;

			Vector2<unsigned int> getLoopPoints() const override;

			void setMute(bool state) override;

			bool getMute() const override;

			void setMode(NIKE_AUDIO_MODE mode) override;

			NIKE_AUDIO_MODE getMode() const override;

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

			std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) override;

			std::shared_ptr<Audio::IChannelGroup> createChannelGroup(std::string const& identifier) override;

			std::shared_ptr<Audio::IChannel> playSound(std::shared_ptr<Audio::IAudio> audio, std::shared_ptr<Audio::IChannelGroup> channel_group, bool start_paused) override;

			void update() override;

			void shutdown() override;
		};

		#endif //Expose implementation only to NIKE Engine

		/*****************************************************************//**
		* Audio Service
		*********************************************************************/

		//Audio Service
		class NIKE_API Service {
		private:
			// Delete Copy Constructor & Copy Assignment
			Service(Service const& rhs) = delete;
			void operator=(Service const& copy) = delete;

			//Audio System
			std::shared_ptr<Audio::IAudioSystem> audio_system;

			//Map of channels
			std::unordered_map<std::string, std::shared_ptr<Audio::IChannel>> channels;

			//Map of groups
			static std::unordered_map<std::string, std::shared_ptr<Audio::IChannelGroup>> channel_groups;

			// Playlist Management
			struct Playlist {
				std::deque<std::string> tracks;
				bool loop = false;
			};

			//Queue for each channel's playlist
			std::unordered_map<std::string , Playlist> channel_playlists;

		public:

			//Default Constructor
			Service() = default;

			//Destructor
			~Service() = default;

			//Set Audio system
			void setAudioSystem(std::shared_ptr<Audio::IAudioSystem> audio_sys);

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

			//Play Audio
			//Channel retrieval: channel_id has to be specified & bool loop has to be true ( channel_id = "" or loop = false, if retrieval is not needed )
			//Channel ID will override each other if the same id is specified more than once
			void playAudio(std::string const& audio_id, std::string const& channel_id, std::string const& channel_group_id, float vol, float pitch, bool loop, bool is_music, bool start_paused = false);

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

		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!SERVICE_AUDIO_HPP
