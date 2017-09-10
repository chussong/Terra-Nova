#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <string>
#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include "file.hpp"

// Should have one channel for bgm with its own functions that loop 
// automatically, then another channel for incidental noise with its own 
// functions which do not loop.

namespace Audio {

	void FillAudio(void* udata, Uint8* stream, int len);

	bool Initialize();
	void Deactivate();

	void Play(const std::string& filename, const int loops = 0);
	void Load(const std::string& filename);
	void Pause();
	void Resume(const int loops = 0);

	void FreeIfStopped();
};

#endif
