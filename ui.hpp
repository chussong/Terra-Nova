#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "unit.hpp"
#include "building.hpp"

namespace TerraNova{

class UIElement : public GFXObject {
	std::unique_ptr<Sprite> textSprite = nullptr;
	SDL_Rect textLayout;

	// dynamic text should probably be a class/struct of its own that has
	// formatting codes and knows how to render itself
	int const* dynamicTextSource;
	TTF_Font* dynamicTextFont;
	SDL_Color dynamicTextColor;
	void UpdateDynamicText() const;
	mutable std::unique_ptr<Sprite> dynamicTextSprite = nullptr;
	mutable int dynamicTextCached = 0;
	mutable SDL_Rect dynamicTextLayout = SDL_Rect();

	public:
		UIElement(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y) : 
			GFXObject(ren, spriteFile, x, y) {}
		UIElement(const UIElement& other) = delete;
		/*UIElement(UIElement&& other) noexcept : 
			GFXObject(std::move(other)), textSprite(std::move(other.textSprite)),
			textLayout(std::move(other.textLayout)), 
			dynamicTextSource(std::move(other.dynamicTextSource)),
			dynamicTextFont(std::move(other.dynamicTextFont)),
			dynamicTextColor(std::move(other.dynamicTextColor)) {}*/
			//values(std::move(other.values)){}
		UIElement(UIElement&& other) noexcept = default;
		UIElement& operator=(const UIElement& other) = delete;

		void AddText(const std::string& text, const SDL_Rect boundingBox,
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetText(const std::string& text, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);
		
		// WARNING: you have to make sure the source outlives this object!
		void AddDynamicText(const int& source, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetDynamicText(const int& source, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
};

class Button : public UIElement {
	std::function<void()> onClick;

	public:
		Button(SDL_Renderer* ren, const std::string spriteFile, const int x,
				const int y, std::function<void()> onClick): 
			UIElement(ren, spriteFile, x, y), onClick(onClick) {}

		bool IsButton() const { return true; }
		bool Click() { if(!onClick) std::cerr << "Error! Button contains an "
			<< "invalid function!" << std::endl; onClick(); return true; }
};

// this class is dumb, it should just be a GFXObject
class UIAggregate {
	protected:
		SDL_Renderer* ren;
		int x, y;

	public:
		UIAggregate(SDL_Renderer* ren, const int x, const int y): ren(ren),
			x(x), y(y){}
		virtual ~UIAggregate() = default;

		virtual void Render() const;

		virtual bool IsInfoPanel() const { return false; }
		virtual bool IsUnitInfoPanel() const { return false; }
};

class InfoPanel : public UIAggregate {
	public:
		SDL_Renderer* Renderer() const { return ren; }

		InfoPanel(SDL_Renderer* ren, const int x, const int y):
			UIAggregate(ren, x, y) {}
		virtual ~InfoPanel() = default; // should be protected or pure virtual?

		virtual void Render() const;

		bool IsInfoPanel() const { return true; }

		static void UpdateFromSource(InfoPanel& toUpdate, 
				const GFXObject& source);

	protected:
		virtual bool Update(const GFXObject& source);
};

class UnitInfoPanel : public InfoPanel {
	public:
		UnitInfoPanel(SDL_Renderer* ren, const Unit& source);
		~UnitInfoPanel() = default;

		void Render() const;
		bool Update(const GFXObject& source);

		// should use dynamic text instead of doing UpdateHealth explicitly
		void UpdateHealth(const Unit& source);
		static void UpdateHealthFromSource(UnitInfoPanel& toUpdate, 
				const Unit& source);

		bool IsUnitInfoPanel() const { return true; }

	private:
		std::unique_ptr<UIElement> background;
		std::unique_ptr<UIElement> portrait;
		std::unique_ptr<UIElement> factionIcon;
		std::unique_ptr<UIElement> healthIcon;
		std::unique_ptr<UIElement> attackIcon;

		void UpdateFromUnit(const Unit& source);
};

class BuildingInfoPanel : public InfoPanel {
	public:
		BuildingInfoPanel(SDL_Renderer* ren, const Building& source);
		BuildingInfoPanel(SDL_Renderer* ren, const BuildingPrototype& source);
		~BuildingInfoPanel() = default;

		void Render() const;
		bool Update(const GFXObject& source);

	private:
		std::unique_ptr<UIElement> background;
		std::unique_ptr<UIElement> portrait;
		std::unique_ptr<UIElement> factionIcon;
		std::unique_ptr<UIElement> powerIcon;
		std::unique_ptr<std::array<UIElement,LAST_RESOURCE>> costIcons;

		void UpdateFromBuilding(const Building& source);
		void UpdateFromBuildingPrototype(const BuildingPrototype& source);

		static std::unique_ptr<UIElement> MakeBackground(SDL_Renderer* ren, 
				const std::array<int,2>& panelCoords);
		static std::unique_ptr<UIElement> MakePortrait(SDL_Renderer* ren, 
				const std::string& name, const std::array<int,2>& panelCoords);
		static std::unique_ptr<UIElement> MakeFactionIcon(SDL_Renderer* ren, 
				char faction, const std::array<int,2>& panelCoords);
		static std::unique_ptr<UIElement> MakePowerIcon(SDL_Renderer* ren, 
				const Building& source, const std::array<int,2>& panelCoords);
		static std::unique_ptr<UIElement> MakePowerIcon(SDL_Renderer* ren, 
				const BuildingPrototype& source, 
				const std::array<int,2>& panelCoords);
		static std::unique_ptr<std::array<UIElement,LAST_RESOURCE>> MakeCostIcons(
				SDL_Renderer* ren, const std::array<int,LAST_RESOURCE>& costs,
				const std::array<int,2>& panelCoords);
};

class UnitOrderPanel : public GFXObject {
	//UIElement background;
	std::vector<Button> buttons;
	unsigned int activeButton;

	public:
		UnitOrderPanel(SDL_Renderer* ren, Unit* source);

		void Render() const;

		void Update(Unit& source);
		void AddButton(Button&& newButton);
		bool InsideQ(const int x, const int y); 
		bool Click();

		void MoveTo(int x, int y);

};

} // namespace TerraNova
#endif
