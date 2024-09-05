#include "AudioManager.h"

void AudioManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
}

void AudioManager::PlaySound1()
{
	result = system->playSound(sound1, 0, false, &channel);
	channel->setVolume(0);
	//channel->setPan(0);
	channel->setPaused(false);
}

void AudioManager::PlaySoundTrack()
{
	result = system->playSound(sound2, 0, true, &channel);
	channel->setVolume(0);
	//channel->setPan(0);
	channel->setPaused(false);
}

void AudioManager::LoadSounds()
{
	result = system->createSound("Assets/Sound/sunflower.mp3", FMOD_DEFAULT, 0, &sound1);
	result = sound1->setMode(FMOD_LOOP_OFF);

	result = system->createStream("Assets/Sound/sunflower.mp3", FMOD_DEFAULT, 0, &sound2);
	result = sound2->setMode(FMOD_LOOP_OFF);
}

void AudioManager::UpdateSound()
{
	result = system->update();
}

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
}

