#pragma once
#include "fmod.hpp"

class AudioManager
{
public:
	FMOD::System *system; //pointer to Virtual Sound card
	FMOD::Sound *sound1, *sound2, *sound3, *sound4, *sound5, *sound6, *sound7, *sound8; //sound files
	FMOD::Channel *channel, *channel2, *channel3, *channel4, *channel5, *channel6, *channel7, *channel8; //sound files are played and mixed
	FMOD_RESULT result;
	void *extradriverdata = 0;

	void InitializeAudio(); //initializing FMOD sound card
	void PlaySound1();
	//play sound 1
	void PlayShoot(int screenWidth, int spaceshipPositionX); //play sound 2
	void PlaySad();
	void PlayHit();
	void PlayBoom();
	void PlayTheWorld();
	void PlayPickUp();
	void PlayButtonClick();
	void LoadSounds(); //read sound file from Hdd, load to sound card
	void UpdateSound(); //update any sound parameters - call EVERY loop
	
	AudioManager();
	~AudioManager();
};

