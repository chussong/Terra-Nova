#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>     // std::shared_ptr, std::unique_ptr
#include <typeinfo>
#include <functional> // std::function, std::bind
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "ui.hpp"
#include "unit.hpp"
#include "building.hpp"
#include "tile.hpp"
#include "colony.hpp"
#include "map.hpp"

class GameWindow {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;
		int playerNumber;

		int viewCenterRow = 0;
		int viewCenterCol = 0;

		template<typename T>
		bool PlayerOwns(const T* thing) const{
			return thing && thing->Owner() == playerNumber;
		}

		// GameWindow owns the pure UI stuff; the others are pointers to
		// objects owned by someone else, e.g. Map owns Tiles, Game owns Units
		std::vector<std::shared_ptr<UIAggregate>> topLevelUI;
		std::vector<std::unique_ptr<GFXObject>> UI;
		std::vector<std::weak_ptr<GFXObject>> weakClickables;
		std::vector<GFXObject*> clickables;
		std::vector<std::weak_ptr<GFXObject>> weakObjects;
		std::vector<GFXObject*> objects;
		std::vector<std::unique_ptr<UIElement>> background;

		GFXObject* selected;

		// Colony screen -----------------------------------------------------
		Colony* newFocusColony = nullptr;
		Colony* currentFocusColony = nullptr;

		void FocusOnColony(const Map& theMap);
		void BreakFocusOnColony(Map& theMap);
		void DrawResources(const Colony* col);
		void DrawColonyMisc(const Colony* col);
		void LeaveColony() { std::cout << "Leaving colony." << std::endl;
			newFocusColony = nullptr; }
		void SetFocusColony(Colony* col) { std::cout << "Entering colony." 
			<< std::endl; newFocusColony = col; }

		void ClearOutOfBounds();

		std::unique_ptr<Button> endTurnButton;

		void ClickObject(GFXObject* toClick);
		void ClickTile(Tile* clickedTile);
		void ClickUnit(Unit* clickedUnit);

	public:
		GameWindow() = delete;
		explicit GameWindow(const std::string& title, const int x, const int y,
			const int w, const int h);
		~GameWindow();

		GameWindow(const GameWindow& other) = delete;
//		GameWindow(GameWindow&& other);
		GameWindow& operator=(const GameWindow other) = delete;

		void AddEndTurnButton(std::unique_ptr<Button> newButton);
		void Clean();
		void Render();

		SDL_Renderer* Renderer() const;
		GFXObject* Object(const int num);
		GFXObject* SelectedObject(const int x, const int y);
		GFXObject* ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;
		void ClearSelected();
		void ChangeSelected(GFXObject* newSelected); // use SelectNew instead
		void SelectNew(const int clickX, const int clickY);

		void ResetBackground(std::unique_ptr<UIElement> newThing);
		void AddToBackground(std::unique_ptr<UIElement> newThing);
		void ResetObjects();
		void AddTopLevelUI(std::shared_ptr<UIAggregate> newThing);
		void AddUI(std::unique_ptr<GFXObject> newThing);
		void AddClickable(std::shared_ptr<GFXObject> newThing);
		void AddObject(std::shared_ptr<GFXObject> newThing);

		static bool InitSDL();
		static void QuitSDL();

		signal_t HandleKeyPress(SDL_Keycode key);

		void StartTurn();
		void EndTurn();

		// Map screen ---------------------------------------------------------

		signal_t MapScreen(std::shared_ptr<Map> theMap, int centerRow,
				int centerColm);

		void MapScreenCenteredOn(Map& theMap, 
				const int centerRow, const int centerColm);
		void AddMapTiles(Map& theMap, const int centerRow, const int centerColm);
		void MakeInfoPanel(const GFXObject* source);
		void UpdateInfoPanel(const GFXObject* source);
		void SwapInfoPanel(const GFXObject* source);
		void RemoveInfoPanel();
		void MakeOrderPanel(GFXObject* source);
		void SwapOrderPanel(GFXObject* source);
		void RemoveOrderPanel();
};

#endif
