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
constexpr int RESOURCE_X = SCREEN_WIDTH-400;
constexpr int RESOURCE_Y = SCREEN_HEIGHT-100;
constexpr int RESOURCE_WIDTH = 90;
const std::string COLONY_BACKGROUND = "Space-Colony.png";
constexpr auto DEFAULT_FONT = "/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf";
constexpr int DEFAULT_FONT_SIZE = 20;
static TTF_Font* defaultFont;

enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };
enum clickable_t { NONCLICKABLE = 0, COLONIST = 1000, ENDTURN = 2000 };

class gfxObject {

	SDL_Rect layout;
	SDL_Texture* image;
	clickable_t clickType;
//	SDL_Rect spritebox;
//	std::string text; should we just make this an image so we don't render?
//	int layer; should this be stored here or in gamewindow?

	public:
		gfxObject() = delete;
		explicit gfxObject(SDL_Renderer* ren, const std::string& filename,
				const int x = 0, const int y = 0);
		explicit gfxObject(SDL_Renderer* ren, const std::string& text,
				const SDL_Color color, TTF_Font* font = defaultFont,
				const int x = 0, const int y = 0);
		~gfxObject();

		gfxObject(const gfxObject& that) = delete;
		gfxObject(gfxObject&& that) noexcept;
		gfxObject& operator=(const gfxObject that) = delete;

		void SetClickType(const clickable_t newType);
		void MoveTo(int x, int y);
		void Resize(int w, int h);
		void RenderTo(SDL_Renderer* ren) const;

		int LeftEdge() const;
		int RightEdge() const;
		int TopEdge() const;
		int BottomEdge() const;
		inline bool IsButton() const { return (clickType/1000 == 2); }
};

class gameWindow {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		std::vector<gfxObject> background;
		std::vector<gfxObject> clickable;
		std::vector<gfxObject> objects;
		std::vector<gfxObject> buttons;

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

		void MakeColonyScreen(const std::shared_ptr<colony> col);

		void AddResourceElements(); // eventually these will be baked in
		void AddInnerRing(const std::shared_ptr<colony> col);
		void AddOuterRing(const std::shared_ptr<colony> col);
		void AddColonists(const std::shared_ptr<colony> col);
		void AddColonyMisc(const std::shared_ptr<colony> col);

		static bool InitSDL();
		static void QuitSDL();
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
