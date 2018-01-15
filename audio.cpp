#include "audio.hpp"

//namespace TerraNova::Audio {  ////// this is the C++17 version

namespace TerraNova {
namespace Audio {

namespace {
	Mix_Music* activeMusic;
	Mix_Music* enqueuedMusic = nullptr;
	Mix_Chunk* soundChunk;
} // anonymous namespace

bool Initialize() {
	int flags = MIX_INIT_OGG;
	if ((Mix_Init(flags) & flags) != flags) {
		std::cerr << "Error: failed to initialize SDL_mixer." << std::endl;
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		std::cerr << "Error: Mix_OpenAudio failed with this error: " 
			<< Mix_GetError() << std::endl;
		Mix_Quit();
		return false;
	}
	return true;
}

void Deactivate() {
	Mix_CloseAudio();
	if (activeMusic) {
		FreeMusic();
	}
	if (soundChunk) {
		Mix_FreeChunk(soundChunk);
		soundChunk = nullptr;
	}
	Mix_Quit();
}

void PlayMusic(const std::string& filename) {
	Mix_Music* loadedMusic = LoadMusic(filename);
	if (loadedMusic == nullptr) return;

	if (activeMusic) {
		ReplaceMusic(loadedMusic);
		//Mix_FreeMusic(musicChunk);
	} else {
		activeMusic = loadedMusic;
		if (Mix_FadeInMusic(activeMusic, -1, 2000) == -1) {
			std::cerr << "Error: could not play music from " << filename << ", "
				<< "getting error " << Mix_GetError() << std::endl;
			return;
		}
	}
}

// load and return a pointer to music, *not* other sounds!
Mix_Music* LoadMusic(const std::string& filename) {
	Mix_Music* loadedMusic = Mix_LoadMUS(File::AbsolutePath(
				"music/" + filename + ".ogg").c_str());

	if (loadedMusic == nullptr) {
		std::cerr << "Error: failed to load music file at " << filename
			<< ", getting error " << Mix_GetError() << std::endl;
		return nullptr;
	}

	return loadedMusic;
}

// fade out new music, then free it once it's done
void TurnOffMusic() {
	Mix_FadeOutMusic(2000);
	Mix_HookMusicFinished(FreeMusic);
}

// free music which is already off
void FreeMusic() {
	Mix_FreeMusic(activeMusic);
	activeMusic = nullptr;
}

// fade out current music, fade in new music
void ReplaceMusic(Mix_Music* newMusic) {
	Mix_FadeOutMusic(2000);
	enqueuedMusic = newMusic;
	Mix_HookMusicFinished(ReplaceMusicCallback);
}

// this exists to be handed to Mix; don't call it
void ReplaceMusicCallback() {
	Mix_FreeMusic(activeMusic);
	activeMusic = enqueuedMusic;
	enqueuedMusic = nullptr;
	if (Mix_FadeInMusic(activeMusic, -1, 2000) == -1) {
		std::cerr << "Error: could not fade in new music, getting error "
			<< Mix_GetError() << std::endl;
		return;
	}
}

// this function should only be used to play miscellaneous sounds; for music,
// use PlayMusic
void PlaySound(const std::string& filename, const int loops) {
	Mix_Chunk* loadedChunk = LoadSound(filename);
	if (loadedChunk == nullptr) return;

	if (soundChunk) Mix_FreeChunk(soundChunk);
	soundChunk = loadedChunk;

	if (Mix_PlayChannel(-1, soundChunk, loops) == -1) {
		std::cerr << "Error: could not play audio from " << filename << ", "
			<< "getting error " << Mix_GetError() << std::endl;
		return;
	}

	//std::cout << "Playing " << filename << " " << loops << " time(s)." 
		//<< std::endl;
	//Resume();
}

// load and return a pointer to a sound file, *not* music!
//
// despite its name, Mix_LoadWAV can load WAV, AIFF, RIFF, OGG, and VOC
Mix_Chunk* LoadSound(const std::string& filename) {
	Mix_Chunk* loadedSound = Mix_LoadWAV(File::AbsolutePath(filename).c_str());

	if (loadedSound == nullptr) {
		std::cerr << "Error: failed to load audio file at " << filename
			<< ", getting error " << Mix_GetError() << std::endl;
		return nullptr;
	}

	return loadedSound;
}

void Pause() {
	//if(device != 0) SDL_PauseAudioDevice(device, 1);
}

void Resume(const int) {
	//if(device != 0) SDL_PauseAudioDevice(device, 0);
	//Mix_PlayChannel(0, soundChunk, loops);
}

} // namespace Audio
} // namespace TerraNova
