#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include "entity.hpp"
#include "game.hpp"

enum button_t { END_TURN = 0 };

extern TTF_Font* defaultFont;
enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };

class uiElement : public entity {
	std::unique_ptr<gfxObject> textSprite;
	SDL_Rect textLayout;

	bool button = false;
	button_t type = END_TURN;

	public:
		uiElement() = delete;
		uiElement(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y) : 
			entity(ren, spriteFile, x, y) {}
		uiElement(const uiElement& other) = delete;
		uiElement(uiElement&& other) noexcept : 
			entity(std::move(other)), textSprite(std::move(other.textSprite)),
			textLayout(std::move(other.textLayout)) {}
		uiElement& operator=(const uiElement& other) = delete;

		void AddText(const std::string& text, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);

		void EnableButton(const button_t type);
		void DisableButton();
		signal_t Select();

		void Render() const;
};

#endif
