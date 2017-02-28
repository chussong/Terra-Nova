#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <iostream>
#include <string>
#include <utility>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// !! This should be a class that holds all of this stuff so we don't need
// !! to keep track of one million different things

constexpr int SCREEN_WIDTH	= 1024;
constexpr int SCREEN_HEIGHT	= 768;
constexpr int TILE_SIZE = 40;

void LogSDLError(std::ostream& os, const std::string &msg);

bool RenderingInit();
void RenderingQuit();
SDL_Window* CreateWindow(const std::string title, const int x, const int y,
	const int width, const int height);
SDL_Renderer* CreateRenderer(SDL_Window* win);
SDL_Texture* LoadTexture(const std::string& file, SDL_Renderer* ren);

void RenderTexture(SDL_Texture* tex, SDL_Renderer* ren,
		const int x, const int y, const int w, const int h);
void RenderTexture(SDL_Texture* tex, SDL_Renderer* ren,
		const int x, const int y);
void TileTexture(SDL_Texture* tex, SDL_Renderer* ren);

inline std::string GetSpritePath(const std::string& subDir = ""){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	static std::string baseDir;
	if(baseDir.empty()){
		char* basePath = SDL_GetBasePath();
		if(basePath){
			baseDir = basePath;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting resource path: " << SDL_GetError()
				<< std::endl;
			return "";
		}
		size_t pos = baseDir.rfind("bin");
		baseDir = baseDir.substr(0, pos);
	}

	return subDir.empty() ? baseDir : baseDir + subDir + PATH_SEP;
}

template<typename T, typename... Args>
inline void SDL_Cleanup(T* t, Args&&... args){
	SDL_Cleanup(t);
	SDL_Cleanup(std::forward<Args>(args)...);
}

template<>
inline void SDL_Cleanup<SDL_Window>(SDL_Window* win){
	if(win == nullptr){
		return;
	}
	SDL_DestroyWindow(win);
}

template<>
inline void SDL_Cleanup<SDL_Renderer>(SDL_Renderer* ren){
	if(ren == nullptr){
		return;
	}
	SDL_DestroyRenderer(ren);
}

template<>
inline void SDL_Cleanup<SDL_Texture>(SDL_Texture* tex){
	if(tex == nullptr){
		return;
	}
	SDL_DestroyTexture(tex);
}

template<>
inline void SDL_Cleanup<SDL_Surface>(SDL_Surface* surf){
	if(surf == nullptr){
		return;
	}
	SDL_FreeSurface(surf);
}
#endif
