#pragma once

#include"lib.hpp"
#include <string>
#include <filesystem>

namespace our {

    class Audio {
       

    public:
        ma_result result;
        bool isPlaying = false;
        ma_engine engine;
        Audio();
        ~Audio();
        void play(std::string audio_file);
        void stop();
        void playLoop(std::string audio_file);
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);


    };
}
