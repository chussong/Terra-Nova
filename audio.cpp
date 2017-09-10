#include "audio.hpp"

namespace Audio {

namespace {
	//SDL_AudioSpec desiredSpec;
	//SDL_AudioSpec actualSpec;

	//SDL_AudioDeviceID device;

	//Uint8* audioChunk;
	//Uint32 audioLength;
	//Uint8* audioPos;

	Mix_Chunk* soundChunk;
} // anonymous namespace

// This obscure function is required by SDL -- *udata is the data to be read,
// *stream is the audio buffer to be filled, and len is the length is the buffer
// in bytes.
/*void FillAudio(void* udata, Uint8* stream, int len) {
	if ( audioLength == 0 ) return;

	len = std::min(len, static_cast<int>(audioLength));
	SDL_MixAudio(stream, audioPos, len, SDL_MIX_MAXVOLUME);
	audioPos += len;
	audioLength -= len;
}*/

/*void Initialize() {
	desiredSpec.freq = 48000;
	desiredSpec.format = AUDIO_F32;
	desiredSpec.channels = 2;
	desiredSpec.samples = 4096;
	//spec.callback = std::bind<void(void*, Uint8*, int)>(FillAudio, this, 1_, 2_, 3_);
	//spec.callback = [this](void* A, Uint8* B, int C){this->FillAudio(A, B, C);};
	desiredSpec.callback = FillAudio;
	desiredSpec.userdata = nullptr;

	device = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &actualSpec, 0);
	if (device == 0) {
		SDL_Log("Failed to open audio: %s", SDL_GetError());
	} else {
		if (actualSpec.format != desiredSpec.format) {
			SDL_Log("Actual audio format differs from requested one.");
		}
	}
}*/

/*void Deactivate() {
	if(device != 0) SDL_CloseAudioDevice(device);
	device = 0;
}*/

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
	if (soundChunk) {
		Mix_FreeChunk(soundChunk);
		soundChunk = nullptr;
	}
	Mix_Quit();
}

void Play(const std::string& filename, const int loops) {
	Load(filename);
	if(Mix_PlayChannel(-1, soundChunk, loops) == -1) {
		std::cerr << "Error: could not play audio from " << filename << ", "
			<< "getting error " << Mix_GetError() << std::endl;
		return;
	}
	//std::cout << "Playing " << filename << " " << loops << " time(s)." 
		//<< std::endl;
	//Resume();
}

// despite its name, Mix_LoadWAV can load WAV, AIFF, RIFF, OGG, and VOC
//
// it would be nice to have an array of soundChunks corresponding to different
// channels which could be loaded and freed separately
void Load(const std::string& filename) {
	Mix_Chunk* tempSound = Mix_LoadWAV(File::AbsolutePath(filename).c_str());

	if (tempSound == nullptr) {
		std::cerr << "Error: failed to loud audio file at " << filename
			<< ", getting error " << Mix_GetError() << std::endl;
		return;
	}

	if (soundChunk) {
		Mix_FreeChunk(soundChunk);
		soundChunk = nullptr;
	}
	soundChunk = tempSound;
}

void FreeIfStopped() {
	if (!Mix_Playing(0)) {
		if(soundChunk) {
			Mix_FreeChunk(soundChunk);
			soundChunk = nullptr;
		}
	}
}

void Pause() {
	//if(device != 0) SDL_PauseAudioDevice(device, 1);
}

void Resume(const int) {
	//if(device != 0) SDL_PauseAudioDevice(device, 0);
	//Mix_PlayChannel(0, soundChunk, loops);
}

} // Audio namespace
