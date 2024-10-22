/*****************************************************************//**
 * \file   sysAudio.h
 * \brief  Audio system manager function declarations
 *
 * \author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SERVICE_AUDIO_HPP
#define SERVICE_AUDIO_HPP

/************************************
* SOME NOTES TO TAKE NOTE OF:
* - AUDIO WILL BE PLAYED LIKE AE
* - THERE WILL BE AUIDIO GROUP AND AUDIO
/****************************************/

namespace NIKESAURUS {
	namespace Audio {
		/*****************************************************************//**
		* Abstract Audio Classes
		*********************************************************************/

		//Abstract audio class
		class NIKESAURUS_API IAudio {
		private:
		public:
			//Defaults
			IAudio() = default;
			virtual ~IAudio() = default;

			//Release Audio
			virtual void release() = 0;

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

		//Abstract channel class
		class NIKESAURUS_API IChannel {
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

			//Set channel group
			virtual void setChannelGroup(Audio::IChannelGroup* group) = 0;

			//Get channel group
			virtual Audio::IChannelGroup getChannelGroup() const = 0;
		};

		//Abstract channel group class
		class NIKESAURUS_API IChannelGroup {
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

			//Add child channel group
			virtual void addChildGroup(std::shared_ptr<FMOD::ChannelGroup> group) = 0;

			//Get parent channel group
			virtual std::shared_ptr<FMOD::ChannelGroup> getChildGroup(int index) const = 0;
		};

		//Abstract audio system class
		class NIKESAURUS_API IAudioSystem {
		private:
		public:
			//Defaults
			IAudioSystem() = default;
			virtual ~IAudioSystem() = default;

			//Create Sound Audio
			virtual std::shared_ptr<Audio::IAudio> createSound(std::string const& file_path) = 0;

			//Create Stream Audio ( For music )
			virtual std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) = 0;
		};

		/*****************************************************************//**
		* DLL Build Implementation
		*********************************************************************/
		#ifdef NIKE_BUILD_DLL //Expose implementation only to NIKE Engine

		//NIKE Audio
		class NIKEAudio : public IAudio {
		private:
			FMOD::Sound* sound{ nullptr };
		public:
			NIKEAudio(FMOD::Sound* sound);

			FMOD::Sound* getSound();

			void release() override;

			unsigned int getLength() const override;

			void setMode(FMOD_MODE) override;

			FMOD_MODE getMode() const override;

			void setLoopCount(int count) override;

			int getLoopCount() const override;

			void setLoopPoints(unsigned int start, unsigned int end) override;

			Vector2<unsigned int> getLoopPoints() const override;
		};

		//NIKE Audio Group
		class NIKEChannel : public IChannel {
		private:
			FMOD::Channel* channel{ nullptr };
		public:
			NIKEChannel(FMOD::Channel* channel);

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

			void setChannelGroup(Audio::IChannelGroup* group) override;

			Audio::IChannelGroup getChannelGroup() const override;
		};

		//NIKE Audio Group
		class NIKEChannelGroup : public IChannelGroup {
		private:
			FMOD::ChannelGroup* group{ nullptr };
		public:
			NIKEChannelGroup(FMOD::ChannelGroup* group);

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

			void addChildGroup(std::shared_ptr<FMOD::ChannelGroup> group) override;

			std::shared_ptr<FMOD::ChannelGroup> getChildGroup(int index) const override;
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
			~NIKEAudioSystem();

			std::shared_ptr<Audio::IAudio> createSound(std::string const& file_path) override;

			std::shared_ptr<Audio::IAudio> createStream(std::string const& file_path) override;
		};

		#endif //Expose implementation only to NIKE Engine

		/*****************************************************************//**
		* Audio Service
		*********************************************************************/

		//Audio Service
		class NIKESAURUS_API Service {
		private:
			// Delete Copy Constructor & Copy Assignment
			Service(Service const& rhs) = delete;
			void operator=(Service const& copy) = delete;

			//Map of groups
			static std::unordered_map<std::string, std::shared_ptr<Audio::NIKEChannelGroup>> groups;
		public:

			//Default Constructor
			Service() = default;

			//Destructor
			~Service();

			//Conversion from raw to shared pointer
			static std::shared_ptr<Audio::NIKEChannelGroup> convertToShared(Audio::NIKEChannelGroup*);

			std::shared_ptr<Audio::NIKEChannelGroup> getGroup() const;

			// Play music
			void audioPlay(NE_AUDIO audio, NE_AUDIO_GROUP, float vol, float pitch, bool);

			// Stop sound
			void audioStopGroup(NE_AUDIO_GROUP group);

			// Play sound
			void audioPauseGroup(NE_AUDIO_GROUP group);

			// Resume music
			void audioResumeGroup(NE_AUDIO_GROUP group);

			// Set pitch
			void audioSetGroupPitch(NE_AUDIO_GROUP group, float pitch);

			// Set volume
			void audioSetGroupVolume(NE_AUDIO_GROUP group, float vol);

			// Unload audio group
			void audioUnloadGroup(NE_AUDIO_GROUP group);

			// Unload audio
			void audioUnloadAudio(NE_AUDIO audio);

			// Check if audio playimg
			void IsPlaying(NE_AUDIO audio);
		};
	}
}

#endif //!SERVICE_AUDIO_HPP
