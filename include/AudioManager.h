#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map>

class AudioManager {
public:
    enum SoundType {
        CHOMP,      // Âm thanh ăn pellet
        EATGHOST,   // Âm thanh ăn ma
        DEATH,      // Âm thanh chết
        BEGIN       // Âm thanh bắt đầu
    };

    AudioManager();
    ~AudioManager();

    bool init();
    void cleanup();
    
    void playSound(SoundType type);
    void stopAllSounds();
    
private:
    std::map<SoundType, Mix_Chunk*> sounds;
    bool loaded = false;
    
    Mix_Chunk* loadSound(const std::string& path);
};

#endif
