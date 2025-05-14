/*
Adam Kavanagh
D00247069@
Coded Commited out while i work on fitting files into project and bug fixing



#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <list>
#include <vector>
#include <memory>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <basetsd.h>

enum class SoundEvent : UINT8 {
 ButtonClick,
 CannonFire,
 PlayerDamage,
 PlayerDeath
        
};

class AudioManager
{

    static AudioManager& GetInstance();

    void PlayMenuMusic();
    void PlayGameMusic();
    void StopMusic();
    void SetMusicVolume(float volume);


    //Sound effects
    void PlaySound(SoundEvent event);


    void Update();

private:

    AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void LoadAllSounds();

    std::unique_ptr<sf::Music> menuMusic;
    std::unique_ptr<sf::Music> currentMusic;
    float m_musicVolume;

    std::unordered_map<SoundEvent, sf::SoundBuffer> m_soundBuffers;
    std::list<sf::Sound> m_activeSounds;

    std::vector<std::Sound> m_CannonSoundPool;
    size_t m_CannonSoundPool = 0;

    // Constants
    static constexpr size_t CANNON_SOUND_POOL_SIZE = 8;
};
*/