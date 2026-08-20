#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() = default;

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::init() {
    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to init Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tải các file âm thanh từ assets/
    sounds[CHOMP] = loadSound("assets/chomp.wav");
    sounds[EATGHOST] = loadSound("assets/eatghosh.wav");
    sounds[DEATH] = loadSound("assets/death.wav");
    sounds[BEGIN] = loadSound("assets/begin.wav");
    sounds[WIN] = loadSound("assets/win.wav");

    sounds[BEGIN] = loadSound("assets/begin.wav");
    if (sounds[BEGIN]) {
        Mix_VolumeChunk(sounds[BEGIN], 30); // 64/128 = 50% volume
    }

    // Kiểm tra xem tất cả âm thanh đã được tải thành công
    bool allLoaded = true;
    for (auto& [type, chunk] : sounds) {
        if (!chunk) {
            allLoaded = false;
        }
    }

    loaded = allLoaded;
    return loaded;
}

Mix_Chunk* AudioManager::loadSound(const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
    }
    return chunk;
}

void AudioManager::cleanup() {
    for (auto& [type, chunk] : sounds) {
        if (chunk) {
            Mix_FreeChunk(chunk);
        }
    }
    sounds.clear();
    Mix_CloseAudio();
}

void AudioManager::playSound(SoundType type) {
    if (!loaded) return;
    
    auto it = sounds.find(type);
    if (it != sounds.end() && it->second) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioManager::stopAllSounds() {
    Mix_HaltChannel(-1);
}
