#include "AudioManager.h"

void AudioManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
}

void AudioManager::PlaySound1()
{
	result = system->playSound(sound1, 0, false, &channel);
	channel->setVolume(1.5);
	//channel->setPan(0);
	//channel->setPaused(false);
}

void AudioManager::PlaySoundTrack()
{
	result = system->playSound(sound2, 0, false, &channel2);
	//channel->setVolume(1);
	//channel->setPan(0);
	//channel->setPaused(false);
}

void AudioManager::PlaySad()
{
	channel->setPaused(true);
	channel2->setPaused(true);
	result = system->playSound(sound3, 0, false, &channel3);
	//channel->setVolume(1);
	//channel->setPan(0);
	//channel->setPaused(false);
}


void AudioManager::LoadSounds()
{
	
	result = system->createSound("Assets/Sound/fallen-down.mp3", FMOD_DEFAULT, 0, &sound1);
	result = sound1->setMode(FMOD_LOOP_OFF);

	result = system->createStream("Assets/Sound/shoot.wav", FMOD_DEFAULT, 0, &sound2);
	result = sound2->setMode(FMOD_LOOP_OFF);

	result = system->createStream("Assets/Sound/sad.mp3", FMOD_DEFAULT, 0, &sound3);
	result = sound3->setMode(FMOD_LOOP_NORMAL);
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

