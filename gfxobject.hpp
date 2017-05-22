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
#include "gamevars.hpp"

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
