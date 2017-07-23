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
#include "ui.hpp"
#include "person.hpp"
#include "building.hpp"
#include "tile.hpp"
#include "colony.hpp"
#include "map.hpp"

class map;
class colony;
class uiElement;
class UIAggregate;
class tile;
class game;

class gameWindow : public std::enable_shared_from_this<gameWindow> {
	protected:
		SDL_Window* win;
		SDL_Renderer* ren;

		// gameWindow owns the pure UI stuff; the others are pointers to
		// objects owned by someone else, e.g. map owns tiles, game owns units
		std::vector<std::shared_ptr<UIAggregate>> topLevelUI;
		std::vector<std::unique_ptr<entity>> UI;
		std::vector<entity*> clickables;
		std::vector<entity*> objects;
		std::vector<std::unique_ptr<uiElement>> background;

		entity* selected;

		// colony screen -----------------------------------------------------
		void DrawTiles(const colony* col);
		int  ColonyTileX(const colony* col, const unsigned int row, 
				const unsigned int colm);
		int  ColonyTileY(const colony* col, const unsigned int row);
		void DrawResources(const colony* col);
		//void DrawColonists(const colony* col);
		void DrawColonyMisc(const colony* col);
		void LeaveColony() { std::cout << "Leaving colony." << std::endl;
			leaveColony = true; }
		bool leaveColony;

		std::unique_ptr<Button> endTurnButton;

	public:
		gameWindow() = delete;
		explicit gameWindow(const std::string& title, const int x, const int y,
			const int w, const int h);
		~gameWindow();

		gameWindow(const gameWindow& other) = delete;
//		gameWindow(gameWindow&& other);
		gameWindow& operator=(const gameWindow other) = delete;

		void AddEndTurnButton(std::unique_ptr<Button> newButton);
		void Clean();
		void Render();
		//void AddObject(std::string filename, const int x = 0, const int y = 0);

		SDL_Renderer* Renderer() const;
		entity* Object(const int num);
		entity* SelectedObject(const int x, const int y);
		entity* ClickedObject(const int x, const int y);
		std::array<int, 4> Layout() const;
		bool Ready() const;

		void ResetBackground(std::unique_ptr<uiElement> newThing);
		void AddToBackground(std::unique_ptr<uiElement> newThing);
		void ResetObjects();
		void AddTopLevelUI(std::shared_ptr<UIAggregate> newThing);
		void AddUI(std::unique_ptr<entity> newThing);
		void AddClickable(entity* newThing);
		void AddClickable(std::shared_ptr<entity> newThing);
		void AddObject(entity* newThing);

		static bool InitSDL();
		static void QuitSDL();

		signal_t HandleKeyPress(SDL_Keycode key, std::shared_ptr<map> theMap);

		// colony management screen -------------------------------------------

		signal_t ColonyScreen(std::shared_ptr<colony> col);

		void MakeColonyScreen(const colony* col);
		
		// map screen ---------------------------------------------------------

		signal_t MapScreen(std::shared_ptr<map> theMap, int centerRow,
				int centerColm);

		void MapScreenCenteredOn(std::shared_ptr<map> theMap, 
				const int centerRow, const int centerColm);
		void AddMapTiles(std::shared_ptr<map> theMap,
				const int centerRow, const int centerColm);
		void MakeUnitInfoPanel(const entity* unit);
		void UpdateUnitInfoPanel(const entity* unit);
		void RemoveUnitInfoPanel();

		void MoveUpLeft(person* mover, std::shared_ptr<map> theMap);
		void MoveUpRight(person* mover, std::shared_ptr<map> theMap);
		void MoveLeft(person* mover, std::shared_ptr<map> theMap);
		void MoveRight(person* mover, std::shared_ptr<map> theMap);
		void MoveDownLeft(person* mover, std::shared_ptr<map> theMap);
		void MoveDownRight(person* mover, std::shared_ptr<map> theMap);
};

#endif
