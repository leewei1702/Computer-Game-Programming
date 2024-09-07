#pragma once
#include "fmod.hpp"

class AudioManager
{
public:
	FMOD::System *system; //pointer to Virtual Sound card
	FMOD::Sound *sound1, *sound2, *sound3; //sound files
	FMOD::Channel *channel, *channel2, *channel3; //sound files are played and mixed
	FMOD_RESULT result;
	void *extradriverdata = 0;

	void InitializeAudio(); //initializing FMOD sound card
	void PlaySound1();
	//play sound 1
	void PlaySoundTrack(); //play sound 2
	void PlaySad();
	void LoadSounds(); //read sound file from Hdd, load to sound card
	void UpdateSound(); //update any sound parameters - call EVERY loop
	
	AudioManager();
	~AudioManager();
};

