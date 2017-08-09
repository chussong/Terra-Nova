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

constexpr std::array<std::array<int, 2>, 18> twoHexAdj{{
		{{0, 2}}, {{-1, 1}}, {{-1, -1}}, {{0, -2}}, {{1, -1}}, {{1, 1}},
		{{0, 4}}, {{-1, 3}}, {{-2, 2}}, {{-2, 0}}, {{-2, -2}}, {{-1, -3}},
		{{0, -4}}, {{1, -3}}, {{2, -2}}, {{2, 0}}, {{2, 2}}, {{1, 3}}
}};

/*template<typename T>
class matrix {
	std::vector<std::vector<T>> m;

	public:
		// put operator[] and all the std::vector member functions in here
		// we can enforce squareness in the constructor and resizing function
};*/

// if an element is expired, remove it; if not, lock and add to returned vector
template<class T>
std::vector<T*> CheckAndLock(std::vector<std::weak_ptr<T>>& in){
	std::vector<T*> ret;
	auto it = in.begin();
	while(it != in.end()){
		std::shared_ptr<T> locked(it->lock());
		if(locked){
			ret.push_back(locked.get());
			++it;
		} else {
			it = in.erase(it);
		}
	}
	return ret;
}

// This class can be used to hold pointers to objects which might expire.
// You MUST call StartTurn and EndTurn on this every turn, or it becomes unsafe!
// Inserting new data puts it into the weak_ptr vector, while accesses and 
// iterators always refer to the vector of raw pointers (which are safe as long
// as StartTurn() and EndTurn() are being called).
template<class T>
class WeakVector{
	std::vector<std::weak_ptr<T>> weak;
	std::vector<T*> safe;

	public:
		void StartTurn() { safe = CheckAndLock(weak); }
		void EndTurn() noexcept { safe.clear(); }

		T*& operator[](const int i) { return safe[i]; }
		const T*& operator[](const int i) const { return safe[i]; }

		std::size_t size() const noexcept { return safe.size(); }
		typename std::vector<T*>::const_iterator	begin() const noexcept
				{ return safe.begin(); }
		typename std::vector<T*>::iterator			begin() noexcept
				{ return safe.begin(); }
		typename std::vector<T*>::const_iterator	end()	const noexcept
				{ return safe.end(); }
		typename std::vector<T*>::iterator			end()	noexcept
				{ return safe.end(); }

		void push_back(const std::weak_ptr<T> newEl) { weak.push_back(newEl); }
		void push_back(const std::shared_ptr<T> newEl) { weak.push_back(newEl); }
};

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
	if(centerRow < 0 || static_cast<unsigned int>(centerRow) >= array.size() 
			|| centerColumn < 0
			|| static_cast<unsigned int>(centerColumn) >= array[0].size()){
		std::cerr << "Error: SetSurrounding on invalid coordinate (" << centerRow 
			<< "," << centerColumn << ")." << std::endl;
		return;
	}
	unsigned int row;
	unsigned int colm;
	for(auto i = 0; i < 6; ++i){
		row = centerRow + hexAdj[i][0];
		colm = centerColumn + hexAdj[i][1];
		if(row >= array.size() || colm >= array[row].size()) continue;
		//std::cout << "(" << row << "," << colm << ") = " << values[i] << std::endl;
		array[row][colm] = values[i];
	}
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

template<typename T, typename Predicate>
void ForTwoSurrounding(std::function<T(int,int)> Fetch, const int centerRow,
		const int centerColumn, Predicate Func){
	for(auto i = 0u; i < twoHexAdj.size(); ++i){
		Func(Fetch(centerRow + twoHexAdj[i][0], centerColumn + twoHexAdj[i][1]));
	}
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
