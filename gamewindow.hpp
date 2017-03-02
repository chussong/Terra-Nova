#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "colony.hpp"

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
constexpr int MAPDISP_ORIGIN_X = 300;
constexpr int MAPDISP_ORIGIN_Y = 300;
constexpr int TILE_WIDTH = 87;
constexpr int TILE_HEIGHT = 75;

class gfxObject {
	SDL_Rect layout;
	SDL_Texture* image;
//	SDL_Rect spritebox;
//	std::string text; should we just make this an image so we don't render?
//	int layer; should this be stored here or in gamewindow?

	public:
		gfxObject() = delete;
		explicit gfxObject(SDL_Renderer* ren, const std::string& filename,
				const int x = 0, const int y = 0);
		~gfxObject();

		gfxObject(const gfxObject& that) = delete;
		gfxObject(gfxObject&& that) noexcept;
		gfxObject& operator=(const gfxObject that) = delete;

		void MoveTo(int x, int y);
		void Resize(int w, int h);
		void RenderTo(SDL_Renderer* ren) const;

		int LeftEdge() const;
		int RightEdge() const;
		int TopEdge() const;
		int BottomEdge() const;
};

class gameWindow {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		std::unique_ptr<gfxObject> background;
		std::vector<gfxObject> objects;

	public:
		gameWindow() = delete;
		explicit gameWindow(const std::string& title, const int x, const int y,
			const int w, const int h);
		~gameWindow();

		gameWindow(const gameWindow& other) = delete;
		gameWindow& operator=(const gameWindow other) = delete;

		void Render();
		void AddObject(std::string filename, const int x = 0, const int y = 0);

		gfxObject* Object(const int num);
		gfxObject* ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;

		static bool InitSDL();
		static void QuitSDL();
};

class colony;
class colonyWindow : public gameWindow {
	std::shared_ptr<colony> col;

	std::vector<gfxObject> innerRing; // 0 is right tile, proceed CCW
	std::vector<gfxObject> outerRing; // 0 is right tile, proceed CCW

	public:
		explicit colonyWindow(std::shared_ptr<colony>, const int x, const int y,
			const int w, const int h);
		void Render();
};

void LogSDLError(std::ostream& os, const std::string &msg);

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
