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

    //Init audio frame data
    plm_samples_t* audio_frame = plm_decode_audio(video.mpeg);
    if (audio_frame) {

        //Gather audio/video info from plmpeg
        int channels = 2;
        int sample_rate = plm_get_samplerate(video.mpeg);
        double fps = plm_get_framerate(video.mpeg);

        //Calculate samples per frame
        unsigned int samples_per_frame = (unsigned int)ceil((sample_rate / fps) * channels);

        //Configure FMOD sound buffer
        FMOD_CREATESOUNDEXINFO sound_info = FMOD_CREATESOUNDEXINFO();
        sound_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        sound_info.format = FMOD_SOUND_FORMAT_PCMFLOAT;
        sound_info.defaultfrequency = sample_rate;
        sound_info.numchannels = channels;
        sound_info.decodebuffersize = samples_per_frame;
        sound_info.length = audio_frame->count * sizeof(float) * channels;

        //Create channel group if not created yet
        if (!video.channel_group) {
            video.channel_group = NIKE_AUDIO_SERVICE->getAudioSystem()->createChannelGroup("Video Channels");
            NIKE_AUDIO_SERVICE->createChannelPlaylist("Video Channels");
        }

        //Check if there is a previous audio
        if (video.audio) {
            if (video.channel) {
                if (video.channel->isPlaying()) {
                    video.channel->stop();
                }

                video.channel = nullptr;
            }
            
            video.audio->release();
            video.audio = nullptr;
        }

        //Create stream
        video.audio = NIKE_AUDIO_SERVICE->getAudioSystem()->createSound(nullptr, NIKE_AUDIO_OPENUSER | NIKE_AUDIO_LOOP_NORMAL | NIKE_AUDIO_2D, &sound_info);

        //Play audio
        video.channel = NIKE_AUDIO_SERVICE->getAudioSystem()->playSound(video.audio, video.channel_group);

        //Buffer audio
        void* buffer1 = nullptr;
        void* buffer2 = nullptr;
        unsigned int length1 = 0;
        unsigned int length2 = 0;
        video.audio->lock(0, sound_info.length, &buffer1, &buffer2, &length1, &length2);
        memcpy(buffer1, audio_frame->interleaved, length1);
        if (buffer2 && length2 > 0) {
            memcpy(buffer2, audio_frame->interleaved + (length1 / sizeof(float)), length2);
        }
        video.audio->unlock(buffer1, buffer2, length1, length2);
    }

    //Set init flag to false
    video.b_init = false;

    //Update video data
    video.duration = static_cast<float>(plm_get_duration(video.mpeg));
}

void NIKE::VideoPlayer::Manager::update(Render::Video& video) {

    //Check if video needs to be updated
    if (video.b_init && !video.video_id.empty()) init(video);

    //Return if mpeg is still not set
    if (!video.mpeg) return;

    //Restart video
    if (video.video_mode == Render::VideoMode::RESTART) {
        plm_rewind(video.mpeg);
        video.video_mode = Render::VideoMode::PLAYING;
    }

    //End video
    if (video.video_mode == Render::VideoMode::END || plm_has_ended(video.mpeg)) {
        video.video_mode = Render::VideoMode::END;
        plm_rewind(video.mpeg);
    }

    //Update loop mode
    if (video.b_loop != static_cast<bool>(plm_get_loop(video.mpeg))) {
        plm_set_loop(video.mpeg, video.b_loop);

        //Set back to playing if video mode end
        if (video.video_mode == Render::VideoMode::END) {
            plm_rewind(video.mpeg);
            video.video_mode = Render::VideoMode::PLAYING;
        }
    }

    //Check if video is active
    if (video.video_mode == Render::VideoMode::PLAYING) {

        //Update video data
        video.curr_time = static_cast<float>(plm_get_time(video.mpeg));

        //Get delta time
        auto delta_time = NIKE_WINDOWS_SERVICE->getDeltaTime();

        //Increment timer
        video.frame_timer += delta_time;

        //Calulate frame rate
        float frame_rate = 1.0f / static_cast<float>(plm_get_framerate(video.mpeg));

        //Decode frame only when necessary
        if (video.frame_timer >= frame_rate) {

            //Decode new video frame
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

            video.frame_timer = 0.0f;
        }

        //Increment timer
        video.audio_timer += delta_time;

        //Adjust playback speed
        float play_back_speed = 1.0f + delta_time;
        if(video.channel) video.channel->setPitch(play_back_speed);

        //Get sample rate
        int sample_rate = plm_get_samplerate(video.mpeg);

        //Calulate frame rate
        float audio_rate = static_cast<float>(PLM_AUDIO_SAMPLES_PER_FRAME) / static_cast<float>(sample_rate);

        //Decode new audio frame only when needed
        if (video.audio_timer >= audio_rate) {

            //Decode new audio frame
            auto* audio_frame = plm_decode_audio(video.mpeg);
            if (!video.channel->getPaused() && audio_frame) {

                //Get current FMOD playback position (in PCM bytes)
                unsigned int play_position = video.channel->getPosition(NIKE_AUDIO_TIMEUNIT_PCMBYTES) * sizeof(float) * 2;

                //Check if we need to write more PCM data
                unsigned int buffer_size = video.audio->getLength(FMOD_TIMEUNIT_PCMBYTES);
                unsigned int write_position = (play_position + buffer_size / 2) % buffer_size;

                //Buffer audio
                void* buffer1 = nullptr;
                void* buffer2 = nullptr;
                unsigned int length1 = 0;
                unsigned int length2 = 0;
                unsigned int audio_frame_size = audio_frame->count * sizeof(float) * 2;
                video.audio->lock(write_position, audio_frame_size, &buffer1, &buffer2, &length1, &length2);
                memcpy(buffer1, audio_frame->interleaved, length1);
                if (buffer2 && length2 > 0) {
                    memcpy(buffer2, audio_frame->interleaved + (length1 / sizeof(float)), length2);
                }
                video.audio->unlock(buffer1, buffer2, length1, length2);
            }

            video.audio_timer = 0.0f;
        }
    }
}

#undef PL_MPEG_IMPLEMENTATION
