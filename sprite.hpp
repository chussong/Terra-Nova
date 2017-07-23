#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "templates.hpp"
#include "gamevars.hpp"

class Sprite {
	SDL_Texture* image;
//	SDL_Rect spritebox;
//	std::string text; should we just make this an image so we don't render?
//	int layer; should this be stored here or in Gamewindow?

	public:
		static TTF_Font* defaultFont;
		static SDL_Color SDLifyTextColor(const textcolor_t color);

		Sprite() = delete;
		explicit Sprite(SDL_Renderer* ren, const std::string filename,
				SDL_Rect layout);
		explicit Sprite(SDL_Renderer* ren, const std::string text,
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
	static std::shared_ptr<Sprite> LoadSprite(const std::string& name);

	public:
		GFXManager() = delete;
		static void Initialize(SDL_Renderer* newRen);
		static std::shared_ptr<Sprite> RequestSprite(const std::string& name);

		static SDL_Renderer* Ren() { return ren; }
};

#endif
