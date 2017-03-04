#ifndef TEMPLATES_HPP
#define TEMPLATES_HPP

#include <iostream>
#include <SDL.h>

template<class T>
void CleanExpired(std::vector<std::weak_ptr<T>> vec){
	typename std::vector<std::weak_ptr<T>>::iterator it = vec.begin();
	while(it != vec.end()){
		if(it->expired()) {
			vec.erase(it);
		} else {
			++it;
		}
	}
}

inline void LogSDLError(std::ostream& os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}


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
