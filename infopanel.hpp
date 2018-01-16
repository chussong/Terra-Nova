#ifndef INFOPANEL_HPP
#define INFOPANEL_HPP

#include "file.hpp"
#include "gfxobject.hpp"
#include "ui.hpp"
#include "unit.hpp"
#include "building.hpp"

namespace TerraNova{

class InfoPanel : public GFXObject {
	public:
		InfoPanel(SDL_Renderer* ren, const GFXObject& source, const int x, 
				const int y);

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);

		bool IsInfoPanel() const { return true; }

		void UpdateFromSource(const GFXObject& source);

	private:
		enum InfoPanelMode { INFO_PANEL_NONE, INFO_PANEL_UNIT, 
			INFO_PANEL_BUILDING };
		InfoPanelMode panelMode = INFO_PANEL_NONE;
		std::vector<std::unique_ptr<UIElement>> dividers;
		std::vector<std::unique_ptr<UIElement>> icons;

		// unit info panel
		void UpdateFromUnit(const Unit& source);
		std::vector<std::unique_ptr<UIElement>> UnitModeDividers();
		std::vector<std::unique_ptr<UIElement>> 
			IconsFromUnit(const Unit& source);

		// building info panel
		void UpdateFromBuilding          (const Building&          source);
		void UpdateFromBuildingPrototype (const BuildingPrototype& source);
		std::vector<std::unique_ptr<UIElement>> BuildingModeDividers();
		std::vector<std::unique_ptr<UIElement>> 
			IconsFromBuilding(const Building& source);
		std::vector<std::unique_ptr<UIElement>> 
			IconsFromBuildingPrototype(const BuildingPrototype& source);
		std::vector<std::unique_ptr<UIElement>> 
			IconsFromBuildingType(const BuildingType& source, 
					const bool poweredOn);
		std::unique_ptr<UIElement> BuildingFactionIcon(const char faction);

		// common generation functions
		std::vector<std::unique_ptr<UIElement>> MakeDividers(
				const int numberOfHorizontal, const int* horizontalX, 
				const int* horizontalY     , const int* horizontalL,
				const int numberOfVertical  , const int* verticalX  ,
				const int* verticalY       , const int* verticalL);
};

} // namespace TerraNova
#endif
