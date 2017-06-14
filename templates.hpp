#ifndef TEMPLATES_HPP
#define TEMPLATES_HPP

#include <string>
#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <SDL.h>


// beware: this is case insensitive!
template<class T>
std::shared_ptr<T> FindByName(std::vector<std::shared_ptr<T>> vec,
		std::string name){
	boost::trim(name);
	for(auto& entry : vec){
		if(boost::iequals(name, entry->Name())){
			//std::cout << "Found an object named " << name << "." << std::endl;
			return entry;
		}
	}
	if(!boost::iequals(name, "none")){
		std::cout << "Failed to find an object named \"" << name << "\"." 
			<< std::endl;
	}
	return nullptr;
}

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
