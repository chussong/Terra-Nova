#ifndef GFXOBJECT_HPP
#define GFXOBJECT_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "templates.hpp"

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

class gfxObject {
	SDL_Texture* image;
//	SDL_Rect spritebox;
//	std::string text; should we just make this an image so we don't render?
//	int layer; should this be stored here or in gamewindow?

	public:
		static TTF_Font* defaultFont;

		gfxObject() = delete;
		explicit gfxObject(SDL_Renderer* ren, const std::string& filename,
				SDL_Rect& layout);
		explicit gfxObject(SDL_Renderer* ren, const std::string& text,
				SDL_Rect& layout, const SDL_Color color, 
				TTF_Font* font = defaultFont);
		~gfxObject();

		gfxObject(const gfxObject& that) = delete;
		gfxObject(gfxObject&& that) noexcept;
		gfxObject& operator=(const gfxObject that) = delete;

		void RenderTo(SDL_Renderer* ren, const SDL_Rect& layout) const;
};

#endif
