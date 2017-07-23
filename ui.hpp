#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include "gamevars.hpp"
#include "entity.hpp"
#include "person.hpp"

class person;
class uiElement : public entity {
	std::unique_ptr<gfxObject> textSprite = nullptr;
	SDL_Rect textLayout;

	int const* dynamicTextSource;
	TTF_Font* dynamicTextFont;
	SDL_Color dynamicTextColor;
	void UpdateDynamicText() const;
	mutable std::unique_ptr<gfxObject> dynamicTextSprite = nullptr;
	mutable int dynamicTextCached;
	mutable SDL_Rect dynamicTextLayout;

	// want to deprecate these two
	//bool button = false;
	//button_t type = END_TURN;

	// probably this should be moved to Button; it represents things like which
	// type of building it is, not things like how many resources are displayed
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
		
		// WARNING: you have to make sure the source outlives this object!
		void AddDynamicText(const int& source, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetDynamicText(const int& source, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);

		// we want to deprecate these three functions
		/*void EnableButton(const button_t type);
		void DisableButton();
		int Select();*/

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
};

class Button : public uiElement {
	std::function<void()> onClick;

	public:
		Button(SDL_Renderer* ren, const std::string spriteFile, const int x,
				const int y, std::function<void()> onClick): 
			uiElement(ren, spriteFile, x, y), onClick(onClick) {}

		bool IsButton() const { return true; }
		bool Click() { if(!onClick) std::cerr << "Error! Button contains an "
			<< "invalid function!" << std::endl; onClick(); return true; }
};

class UIAggregate {
	protected:
		SDL_Renderer* ren;
		int x, y;

	public:
		UIAggregate(SDL_Renderer* ren, const int x, const int y): ren(ren),
			x(x), y(y){}

		virtual void Render() const
			{ std::cerr << "UIAggregate::Render() shouldn't run!" << std::endl;}
};

class UnitInfoPanel : public UIAggregate {
	std::unique_ptr<uiElement> background;
	std::unique_ptr<uiElement> portrait;
	std::unique_ptr<uiElement> factionIcon;
	std::unique_ptr<uiElement> healthIcon;
	std::unique_ptr<uiElement> attackIcon;

	public:
		UnitInfoPanel(SDL_Renderer* ren, const person* unit);

		void Render() const;

		void UpdateHealth(const person* unit);
};

class UnitOrderPanel : public UIAggregate {
	uiElement background;
	std::vector<Button> buttons;

	public:
		UnitOrderPanel(SDL_Renderer* ren, const person* unit);

		void Render() const;

		void UpdatePanel(const person* unit);
};

#endif
