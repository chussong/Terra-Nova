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

// namespace TerraNova::Audio { ////// change to this in C++17
namespace TerraNova {
namespace Audio {

	void FillAudio(void* udata, Uint8* stream, int len);

	bool Initialize();
	void Deactivate();

	void PlayMusic(const std::string& filename);
	Mix_Music* LoadMusic(const std::string& filename);
	void TurnOffMusic();
	void FreeMusic();
	void ReplaceMusic(Mix_Music* newMusic);
	void ReplaceMusicCallback();
	void PlaySound(const std::string& filename, const int loops = 0);
	Mix_Chunk* LoadSound(const std::string& filename);
	void Pause();
	void Resume(const int loops = 0);
} // namespace Audio
} // namespace TerraNova

#endif
