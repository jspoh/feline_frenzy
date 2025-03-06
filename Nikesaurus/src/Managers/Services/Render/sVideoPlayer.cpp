/*****************************************************************//**
 * \file   sVideoPlayer.cpp
 * \brief
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   March 2025
 *
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Render/sVideoPlayer.h"

#pragma warning(push, 0) 
#pragma warning(disable: 4706)
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg/pl_mpeg.h"
#pragma warning(pop)

void NIKE::VideoPlayer::Manager::init(Render::Video& video) {
    //Validate video path
    if (!NIKE_ASSETS_SERVICE->isAssetRegistered(video.video_id) || NIKE_ASSETS_SERVICE->getAssetType(video.video_id) != Assets::Types::Video) {
        NIKEE_CORE_ERROR("Invalid video path!");
        return;
    }

    //Get file path
    auto file_path = NIKE_ASSETS_SERVICE->getAssetPath(video.video_id);

    //Init mpeg
    video.mpeg = plm_create_with_filename(file_path.string().c_str());
    if (!video.mpeg) {
        NIKEE_CORE_ERROR("Error with mpeg init!");
        return;
    }

    //Set basic video functions
    plm_set_loop(video.mpeg, true);
    plm_set_audio_enabled(video.mpeg, true);
    plm_set_video_enabled(video.mpeg, true);

    //Decode first frame
    auto frame = plm_decode_video(video.mpeg);

    //Create OpenGL texture for video frame
    glGenTextures(1, &video.texture_id);
    glBindTexture(GL_TEXTURE_2D, video.texture_id);
    if (frame) {
        //Init rgb data
        video.rgb_data = new uint8_t[frame->width * frame->height * 3];
        video.texture_size = { static_cast<float>(frame->width) , static_cast<float>(frame->height) };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    else {
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Get audio sample rate from video
    int sample_rate = plm_audio_get_samplerate(video.mpeg->audio_decoder);
    if (sample_rate == 0) {
        sample_rate = 44100; //Default to 44.1kHz if unknown
    }

    //Configure FMOD sound buffer
    FMOD_CREATESOUNDEXINFO sound_info = {};
    sound_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    sound_info.format = FMOD_SOUND_FORMAT_PCMFLOAT;
    sound_info.defaultfrequency = sample_rate;
    sound_info.numchannels = 2;
    sound_info.length = sample_rate * 2 * sizeof(float);

    //Create channel group if not created yet
    if (!video.channel_group) {
        video.channel_group = NIKE_AUDIO_SERVICE->getAudioSystem()->createChannelGroup("Video Channels");
    }

    //Create stream
    video.audio = NIKE_AUDIO_SERVICE->getAudioSystem()->createStream(nullptr, NIKE_AUDIO_OPENUSER | NIKE_AUDIO_LOOP_OFF | NIKE_AUDIO_2D, &sound_info);

    //Buffer initial audio
    plm_samples_t* audio_frame = plm_decode_audio(video.mpeg);
    if (audio_frame) {
        void* buffer;
        unsigned int length;
        video.audio->lock(0, audio_frame->count * sizeof(float) * 2, &buffer, nullptr, &length, nullptr);
        memcpy(buffer, audio_frame->interleaved, length);
        video.audio->unlock(buffer, nullptr, length, length);
    }

    //Play audio
    video.channel = NIKE_AUDIO_SERVICE->getAudioSystem()->playSound(video.audio, video.channel_group);

    //Set init flag to false
    video.b_init = false;
}

void NIKE::VideoPlayer::Manager::update(Render::Video& video) {

    //Check if video needs to be updated
    if (video.b_init && !video.video_id.empty()) init(video);

    //Return if mpeg is still not set
    if (!video.mpeg) return;

    //Increment timer
    video.timer += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
    
    //Calulate frame rate
    float frame_rate = 1.0f / static_cast<float>(plm_get_framerate(video.mpeg));

    //Get current video time
    float video_time_stamp = static_cast<float>(plm_get_time(video.mpeg));

    //Get current audio time from for audio synchronization
    if (video.channel) {
        unsigned int audio_time = video.channel->getPosition(NIKE_AUDIO_TIMEUNIT_MS);
        float audio_time_stamp = static_cast<float>(audio_time) / 1000.0f; // Convert ms to seconds

        //Synchronize audio
        float sync_treshold = 0.1f;
        if (fabs(video_time_stamp - audio_time_stamp) > sync_treshold) {
            video.channel->setPosition(static_cast<unsigned int>(video_time_stamp * 1000.0f), FMOD_TIMEUNIT_MS);
        }
    }

    //Decode only when necessary
    while (video.timer >= frame_rate) {
        auto* frame = plm_decode_video(video.mpeg);
        if (frame) {
            video.texture_size = { static_cast<float>(frame->width) , static_cast<float>(frame->height) };

            //Bind texture and update texture
            if (frame->y.data) {

                //Convert plane to rgb
                plm_frame_to_rgb(frame, video.rgb_data, frame->width * 3);

                //Flip RGB Buffer (Manually Swap Rows)
                int row_size = frame->width * 3;
                uint8_t* temp_row = new uint8_t[row_size];

                for (unsigned int y = 0; y < frame->height / 2; ++y) {
                    uint8_t* top_row = video.rgb_data + (y * row_size);
                    uint8_t* bottom_row = video.rgb_data + ((frame->height - 1 - y) * row_size);

                    // Swap rows
                    memcpy(temp_row, top_row, row_size);
                    memcpy(top_row, bottom_row, row_size);
                    memcpy(bottom_row, temp_row, row_size);
                }
                delete[] temp_row;// Free temp buffer

                //Bind texture & generate sub image
                glBindTexture(GL_TEXTURE_2D, video.texture_id);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->width, frame->height, GL_RGB, GL_UNSIGNED_BYTE, video.rgb_data);
            }
        }
        video.timer -= frame_rate;
    }
}

#undef PL_MPEG_IMPLEMENTATION
