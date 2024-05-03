#define MINIAUDIO_IMPLEMENTATION
#include"audio.hpp"
#include<iostream>
#include <thread>
our::Audio::Audio() {
        // Initialize the audio engine
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            printf("Failed to initialize audio engine.");
            return;
        }

}
void our::Audio::play(std::string audio_file){

    std::thread audioThread([this, audio_file]() {
    
        // Get the full path of the audio file
        std::filesystem::path audioPath = std::filesystem::current_path().append("source").append("audio").append(audio_file).string();
        std::string audioName = audioPath.string();
        // Play the audio file
        ma_engine_play_sound(&engine, audioName.c_str(), NULL);
    });

    // Detach the thread to allow it to run independently
    audioThread.detach();
}

void our::Audio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
    (void)pInput;
}
our::Audio::~Audio() {
    ma_engine_uninit(&engine);
}