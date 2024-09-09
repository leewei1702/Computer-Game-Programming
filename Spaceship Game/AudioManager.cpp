#include "AudioManager.h"
void AudioManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
}

void AudioManager::PlaySound1()
{
	result = system->playSound(sound1, 0, false, &channel);
}

void AudioManager::PlayShoot(int screenWidth, int spaceshipPositionX)
{
	result = system->playSound(sound2, 0, false, &channel2);
	//channel->setVolume(1);
	if (spaceshipPositionX < screenWidth/4) {
		channel2->setPan(-1);
	} 
	else if (spaceshipPositionX > screenWidth*3/4) {
		channel2->setPan(1);
	}
	else {
		channel2->setPan(0);
	}
	//channel->setPaused(false);
}

void AudioManager::PlaySad()
{
	result = system->playSound(sound3, 0, false, &channel3);
	//channel->setVolume(1);
	//channel->setPan(0);
	//channel->setPaused(false);
}

void AudioManager::PlayHit()
{
	result = system->playSound(sound4, 0, false, &channel4);
	//channel->setVolume(1);
	//channel->setPan(0);
	//channel->setPaused(false);
}

void AudioManager::PlayBoom()
{
	result = system->playSound(sound5, 0, false, &channel5);
	channel5->setVolume(2);
	//channel->setPan(0);
	//channel->setPaused(false);
}
void AudioManager::PlayTheWorld()
{
	result = system->playSound(sound6, 0, false, &channel6);
	channel6->setVolume(0.8);
	//channel->setPan(0);
	//channel->setPaused(false);
}
void AudioManager::PlayPickUp()
{
	result = system->playSound(sound7, 0, false, &channel7);
	channel7->setVolume(3);
	//channel->setPan(0);
	//channel->setPaused(false);
}


void AudioManager::LoadSounds()
{
	
	result = system->createSound("Assets/Sound/fallen-down.mp3", FMOD_DEFAULT, 0, &sound1);
	result = sound1->setMode(FMOD_LOOP_NORMAL);

	result = system->createSound("Assets/Sound/shoot.wav", FMOD_DEFAULT, 0, &sound2);
	result = sound2->setMode(FMOD_LOOP_OFF);

	result = system->createSound("Assets/Sound/sad.mp3", FMOD_DEFAULT, 0, &sound3);
	result = sound3->setMode(FMOD_LOOP_OFF);

	result = system->createSound("Assets/Sound/hit.wav", FMOD_DEFAULT, 0, &sound4);
	result = sound4->setMode(FMOD_LOOP_OFF);

	result = system->createSound("Assets/Sound/boom.wav", FMOD_DEFAULT, 0, &sound5);
	result = sound5->setMode(FMOD_LOOP_OFF);

	result = system->createSound("Assets/Sound/theworld.mp3", FMOD_DEFAULT, 0, &sound6);
	result = sound6->setMode(FMOD_LOOP_OFF);

	result = system->createSound("Assets/Sound/pickup.mp3", FMOD_DEFAULT, 0, &sound7);
	result = sound7->setMode(FMOD_LOOP_OFF);
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

