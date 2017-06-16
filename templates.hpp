#ifndef TEMPLATES_HPP
#define TEMPLATES_HPP

#include <string>
#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <SDL.h>

constexpr std::array<std::array<int, 2>, 6> hexAdj{{
	{{0, 2}}, {{-1, 1}}, {{-1, -1}}, {{0, -2}}, {{1, -1}}, {{1, 1}}
}};

// beware: this is NOT case sensitive!
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

template<typename T, typename U>
std::array<U,6> GetSurrounding(const T& container, const int centerRow,
		const int centerColumn){
	std::array<U,6> ret = {{
		container[centerRow    ][centerColumn + 2],
		container[centerRow - 1][centerColumn + 1],
		container[centerRow - 1][centerColumn - 1],
		container[centerRow    ][centerColumn - 2],
		container[centerRow + 1][centerColumn - 1],
		container[centerRow + 1][centerColumn + 1]
	}};
	return ret;
}

template<typename T>
void SetSurrounding(std::vector<std::vector<T>>& array, const int centerRow, 
		const int centerColumn, const std::array<T,6>& values){
	array[centerRow    ][centerColumn + 2] = values[0];
	array[centerRow - 1][centerColumn + 1] = values[1];
	array[centerRow - 1][centerColumn - 1] = values[2];
	array[centerRow    ][centerColumn - 2] = values[3];
	array[centerRow + 1][centerColumn - 1] = values[4];
	array[centerRow + 1][centerColumn + 1] = values[5];
}

template<typename T, typename U>
std::array<U,6> ForSurrounding(std::function<T(int,int)> Fetch, const int centerRow,
		const int centerColumn, std::function<U(T)> Func){
	std::array<U,6> ret = {{
		Func(Fetch(centerRow    , centerColumn + 2)),
		Func(Fetch(centerRow - 1, centerColumn + 1)),
		Func(Fetch(centerRow - 1, centerColumn - 1)),
		Func(Fetch(centerRow    , centerColumn - 2)),
		Func(Fetch(centerRow + 1, centerColumn - 1)),
		Func(Fetch(centerRow + 1, centerColumn + 1))
	}};
	return ret;
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
