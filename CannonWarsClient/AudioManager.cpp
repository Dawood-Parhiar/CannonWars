/*
Adam Kavanagh 
D00247069@
Coded Commited out while i work on fitting files into project and bug fixing 



#include "AudioManager.hpp"

AudioManager& AudioManager::GetInstance() 
{
	static AudioManager instance;
	return instance;
}

AudioManager::AudioManager() :
    m_musicVolume(50.f) ,
    m_CannonSoundPoolIndex(0)
{
    LoadAllSounds();

    // Initialize cannon sound pool
    m_CannonSoundPool.resize(CANNON_SOUND_POOL_SIZE);
    for (auto& sound : m_CannonSoundPool) {
        sound.setBuffer(m_soundBuffers.at(SoundEvent::CannonFire));
        sound.setRelativeToListener(true); // non-positional audio as it is 2D top down
    }
}


void AudioManager:: LoadAllSounds()
{

    m_soundBuffers[SoundEvent::ButtonClick].loadFromFile("Path to Sound |Menu Interact SFX");
    m_soundBuffers[SoundEvent::CannonFire].loadFromFile("Path to Sound |Cannon SFX");
    m_soundBuffers[SoundEvent::PlayerDamage].loadFromFile("Path to Sound |Damage SFX");
    m_soundBuffers[SoundEvent::PlayerDeath].loadFromFile("Path to Sound |Death SFX");
}

void AudioManager::PlayMenuMusic()
{
    if (m_currentMusic == "Menu") return;

    if (m_music) {
        m_music->stop();
    }

    m_music = std::make_unique<sf::Music>();
    if (m_music->openFromFile("../Assets/Media/Music/Menu.ogg")) {
        m_music->setLoop(true);
        m_music->setVolume(m_musicVolume);
        m_music->play();
        m_currentMusic = "Menu";
    }
}

void AudioManager::PlayGameMusic()
{
    if (m_currentMusic == "Game") return;

    if (m_music) {
        m_music->stop();
    }

    m_music = std::make_unique<sf::Music>();
    if (m_music->openFromFile("../Assets/Media/Music/Game.ogg")) {
        m_music->setLoop(true);
        m_music->setVolume(m_musicVolume);
        m_music->play();
        m_currentMusic = "Game";
    }
}

void AudioManager::PlaySound(SoundEvent event)
{
    if (event == SoundEvent::CannonFire) {
        m_CannonSoundPool[m_CannonSoundPool].play();
        m_CannonSoundPoolIndex = (m_CannonSoundPool + 1) % CANNON_SOUND_POOL_SIZE;
    }
    else {
        sf::Sound sound;
        sound.setBuffer(m_soundBuffers.at(event));
        sound.setVolume(m_musicVolume);
        sound.play();
        m_activeSounds.push_back(sound);
    }
}

void AudioManager::Update()
{
	m_activeSounds.remove_if([](const sf::Sound& sound) {
		return sound.getStatus() == sf::Sound::Stopped;
		});
    }

	void AudioManager::StopMusic()
	{
		if (m_music) {
			m_music->stop();
			m_currentMusic = "";
		}
	}

	void AudioManager::SetMusicVolume(float volume)
	{
		m_musicVolume = volume;
		if (m_music) {
			m_music->setVolume(volume);
		}
	}

    */