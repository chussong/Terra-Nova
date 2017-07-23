#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <typeinfo>
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

class GameWindow : public std::enable_shared_from_this<GameWindow> {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		// GameWindow owns the pure UI stuff; the others are pointers to
		// objects owned by someone else, e.g. Map owns Tiles, Game owns Units
		std::vector<std::shared_ptr<UIAggregate>> topLevelUI;
		std::vector<std::unique_ptr<GFXObject>> UI;
		std::vector<GFXObject*> clickables;
		std::vector<GFXObject*> objects;
		std::vector<std::unique_ptr<UIElement>> background;

		GFXObject* selected;

		// Colony screen -----------------------------------------------------
		void DrawTiles(const Colony* col);
		int  ColonyTileX(const Colony* col, const unsigned int row, 
				const unsigned int colm);
		int  ColonyTileY(const Colony* col, const unsigned int row);
		void DrawResources(const Colony* col);
		//void DrawColonists(const Colony* col);
		void DrawColonyMisc(const Colony* col);
		void LeaveColony() { std::cout << "Leaving Colony." << std::endl;
			leaveColony = true; }
		bool leaveColony;

		std::unique_ptr<Button> endTurnButton;

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
		//void AddObject(std::string filename, const int x = 0, const int y = 0);

		SDL_Renderer* Renderer() const;
		GFXObject* Object(const int num);
		GFXObject* SelectedObject(const int x, const int y);
		GFXObject* ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;

		void ResetBackground(std::unique_ptr<UIElement> newThing);
		void AddToBackground(std::unique_ptr<UIElement> newThing);
		void ResetObjects();
		void AddTopLevelUI(std::shared_ptr<UIAggregate> newThing);
		void AddUI(std::unique_ptr<GFXObject> newThing);
		void AddClickable(GFXObject* newThing);
		void AddClickable(std::shared_ptr<GFXObject> newThing);
		void AddObject(GFXObject* newThing);

		static bool InitSDL();
		static void QuitSDL();

		signal_t HandleKeyPress(SDL_Keycode key, std::shared_ptr<Map> theMap);

		// Colony management screen -------------------------------------------

		signal_t ColonyScreen(std::shared_ptr<Colony> col);

		void MakeColonyScreen(const Colony* col);
		
		// Map screen ---------------------------------------------------------

		signal_t MapScreen(std::shared_ptr<Map> theMap, int centerRow,
				int centerColm);

		void MapScreenCenteredOn(std::shared_ptr<Map> theMap, 
				const int centerRow, const int centerColm);
		void AddMapTiles(std::shared_ptr<Map> theMap,
				const int centerRow, const int centerColm);
		void MakeUnitInfoPanel(const GFXObject* Unit);
		void UpdateUnitInfoPanel(const GFXObject* Unit);
		void RemoveUnitInfoPanel();

		void MoveUpLeft(Unit* mover, std::shared_ptr<Map> theMap);
		void MoveUpRight(Unit* mover, std::shared_ptr<Map> theMap);
		void MoveLeft(Unit* mover, std::shared_ptr<Map> theMap);
		void MoveRight(Unit* mover, std::shared_ptr<Map> theMap);
		void MoveDownLeft(Unit* mover, std::shared_ptr<Map> theMap);
		void MoveDownRight(Unit* mover, std::shared_ptr<Map> theMap);
};

#endif
