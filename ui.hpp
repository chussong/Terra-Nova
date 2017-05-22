#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>
#include "gamevars.hpp"
#include "entity.hpp"

class uiElement : public entity {
	std::unique_ptr<gfxObject> textSprite;
	SDL_Rect textLayout;

	bool button = false;
	button_t type = END_TURN;

	std::vector<int> values;

	public:
		uiElement() = delete;
		uiElement(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y) : 
			entity(ren, spriteFile, x, y) {}
		uiElement(const uiElement& other) = delete;
		uiElement(uiElement&& other) noexcept : 
			entity(std::move(other)), textSprite(std::move(other.textSprite)),
			textLayout(std::move(other.textLayout)),
			values(std::move(other.values)){}
//		uiElement(uiElement&& other) noexcept = default;
		uiElement& operator=(const uiElement& other) = delete;

		void AddValue(const int val);
		void SetValue(const unsigned int entry, const int newVal);
		int Value(const unsigned int entry) const;

		void AddText(const std::string& text, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetText(const std::string& text, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);

		void EnableButton(const button_t type);
		void DisableButton();
		int Select();

		void Render() const;
};

#endif
