#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "templates.hpp"
#include "gamevars.hpp"

namespace TerraNova {

// basically a 4-parameter constructor for SDL_Rect
SDL_Rect MakeSDLRect(const int x, const int y, const unsigned int w,
		const unsigned int h);

class Sprite {
	SDL_Texture* image;
//	SDL_Rect spritebox;

	static SDL_Rect PackIntoRect(const int x, const int y);
	public:
		//static TTF_Font* defaultFont;
		static SDL_Color SDLifyTextColor(const textcolor_t color);

		Sprite() = delete;
		explicit Sprite(SDL_Renderer* ren, std::string filename,
				SDL_Rect layout);
		explicit Sprite(SDL_Renderer* ren, const std::string& filename,
				const int x, const int y);
		explicit Sprite(SDL_Renderer* ren, const std::string& text,
				SDL_Rect layout, const SDL_Color color, 
				TTF_Font* font = defaultFont);
		~Sprite();

		Sprite(const Sprite& that) = delete;
		Sprite(Sprite&& that) noexcept;
		Sprite& operator=(const Sprite that) = delete;

		void RenderTo(SDL_Renderer* ren, const SDL_Rect& layout) const; // deprecated
		void RenderTo(const SDL_Rect& layout) const;
		void MakeDefaultSize(SDL_Rect& layout) const;
};

class GFXManager {
	static SDL_Renderer* ren; // despite appearances, GFXManager owns this
	static std::vector<std::string> loadedSpriteNames;
	static std::vector<std::shared_ptr<Sprite>> loadedSprites;

	static std::string GetSpritePath(const std::string& subDir = "");
	static std::shared_ptr<Sprite> LoadSprite(std::string name);

	public:
		GFXManager() = delete;
		static void Initialize(SDL_Renderer* newRen);
		static std::shared_ptr<Sprite> RequestSprite(std::string name);

		static SDL_Renderer* Ren() { return ren; }
};

} // namespace TerraNova
#endif
