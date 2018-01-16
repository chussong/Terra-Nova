#include "infopanel.hpp"

namespace TerraNova{

InfoPanel::InfoPanel(SDL_Renderer* ren, const GFXObject& source, 
		const int x, const int y): 
			GFXObject(ren, File::SpritePath() / "ui" / "info_panel", x, y) {
	UpdateFromSource(source);
}

void InfoPanel::Render() const {
	GFXObject::Render();
	for (const auto& div : dividers) div->Render();
	for (const auto& ico : icons   ) ico->Render();
}

void InfoPanel::MoveTo(int x, int y) {
	for (const auto& div : dividers) {
		div->MoveTo(div->X() - X() + x, div->Y() - Y() + y);
	}
	for (const auto& ico : icons) {
		ico->MoveTo(ico->X() - X() + x, ico->Y() - Y() + y);
	}
	GFXObject::MoveTo(x, y);
}

void InfoPanel::MoveTo(SDL_Rect newLayout) {
	MoveTo(newLayout.x, newLayout.y);
}

void InfoPanel::UpdateFromSource(const GFXObject& source) {
	if (source.IsUnit()) {
		UpdateFromUnit(dynamic_cast<const Unit&>(source));
		return;
	}
	if (source.IsBuilding()) {
		UpdateFromBuilding(dynamic_cast<const Building&>(source));
		return;
	}
	if (source.IsBuildingPrototype()) {
		UpdateFromBuildingPrototype(
				dynamic_cast<const BuildingPrototype&>(source) );
		return;
	}
}

void InfoPanel::UpdateFromUnit(const Unit& source) {
	if (panelMode != INFO_PANEL_UNIT) {
		dividers = UnitModeDividers();
		panelMode = INFO_PANEL_UNIT;
	}

	icons = IconsFromUnit(source);
}

std::vector<std::unique_ptr<UIElement>> InfoPanel::MakeDividers(
		const int numberOfHorizontal, const int* horizontalX, 
		const int* horizontalY     , const int* horizontalL,
		const int numberOfVertical  , const int* verticalX  ,
		const int* verticalY       , const int* verticalL) {
	std::vector<std::unique_ptr<UIElement>> output;
	for (int i = 0; i < numberOfHorizontal; ++i) {
		std::unique_ptr<UIElement> newDiv = std::make_unique<UIElement>(
				ren, "info_panel_horizontal_divider", X() + horizontalX[i], 
				Y() + horizontalY[i] );
		newDiv->Resize(horizontalL[i], newDiv->H());
		output.push_back(std::move(newDiv));
	}
	for (int i = 0; i < numberOfVertical; ++i) {
		std::unique_ptr<UIElement> newDiv = std::make_unique<UIElement>(
				ren, "info_panel_vertical_divider", X() + verticalX[i], 
				Y() + verticalY[i] );
		newDiv->Resize(newDiv->W(), verticalL[i]);
		output.push_back(std::move(newDiv));
	}
	return output;
}

namespace UnitPanel {
	constexpr int numberOfHorizontal = 2;
	constexpr int horizontalX[numberOfHorizontal] = { 154, 154 };
	constexpr int horizontalY[numberOfHorizontal] = { 54 , 104 };
	constexpr int horizontalL[numberOfHorizontal] = { 250, 250 };

	constexpr int numberOfVertical = 1;
	constexpr int verticalX[numberOfVertical]   = { 204 };
	constexpr int verticalY[numberOfVertical]   = { 4   };
	constexpr int verticalL[numberOfVertical]   = { 150 };

	enum IconType { UICON_PORTRAIT, UICON_FACTION, UICON_SPECIES, UICON_WEAPON};
	enum TextType { UTEXT_NONE, UTEXT_NAME, UTEXT_HEALTH, UTEXT_WEAPON };
	constexpr int numberOfIcons = 4;
	constexpr IconType iconType[numberOfIcons] = { UICON_PORTRAIT, 
		UICON_FACTION, UICON_SPECIES, UICON_WEAPON };
	constexpr int iconX[numberOfIcons] = { 5  , 156, 156, 156 };
	constexpr int iconY[numberOfIcons] = { 5  , 6  , 56 , 106 };
	constexpr TextType iconText[numberOfIcons] = { UTEXT_NONE, UTEXT_NAME,
		UTEXT_HEALTH, UTEXT_WEAPON };
	constexpr int textX[numberOfIcons] = { 0  , 140, 140, 145 };
	constexpr int textY[numberOfIcons] = { 0  , 25 , 25 , 25  };
	constexpr int textW[numberOfIcons] = { 0  , 199, 199, 199 };
	constexpr int textH[numberOfIcons] = { 0  , 48 , 48 , 48  };
} // namespace UnitPanel

// locations of these dividers given in the above UnitPanel namespace
std::vector<std::unique_ptr<UIElement>> InfoPanel::UnitModeDividers() {
	return MakeDividers(UnitPanel::numberOfHorizontal, UnitPanel::horizontalX,
			UnitPanel::horizontalY, UnitPanel::horizontalL, 
			UnitPanel::numberOfVertical, UnitPanel::verticalX, 
			UnitPanel::verticalY, UnitPanel::verticalL);
}

std::vector<std::unique_ptr<UIElement>> InfoPanel::IconsFromUnit(
		const Unit& source) {
	std::vector<std::unique_ptr<UIElement>> output;
	for (int i = 0; i < UnitPanel::numberOfIcons; ++i) {
		File::Path iconPath = "";
		switch (UnitPanel::iconType[i]) {
			case UnitPanel::UICON_PORTRAIT: {
				iconPath = File::Path("portraits") / "portrait_";
				iconPath += source.Spec()->PathName();
				break;
			}
			case UnitPanel::UICON_FACTION: {
				iconPath = "factioncolor_p";
				iconPath += std::to_string(source.Faction());
				break;
			}
			case UnitPanel::UICON_SPECIES: {
				iconPath = "healthicon_";
				iconPath += source.Species();
				break;
			}
			case UnitPanel::UICON_WEAPON: {
				if (source.Spec()->Attack(0)) {
					iconPath = File::Path("weapons") / 
						source.Spec()->Attack(0)->PathName();
					iconPath += "_icon";
				} else {
					iconPath = File::Path("weapons") / "null_attack_icon";
				}
				break;
			}
		}
		std::unique_ptr<UIElement> newIcon = std::make_unique<UIElement>(ren, 
				iconPath.string(), X() + UnitPanel::iconX[i], 
				Y() + UnitPanel::iconY[i] );

		if (UnitPanel::iconText[i] != UnitPanel::UTEXT_NONE) {
			std::string iconText("");
			SDL_Rect textBox = MakeSDLRect(
					UnitPanel::textX[i], UnitPanel::textY[i], 
					UnitPanel::textW[i], UnitPanel::textH[i] );
			switch (UnitPanel::iconText[i]) {
				case UnitPanel::UTEXT_NONE: {
					break;
				}
				case UnitPanel::UTEXT_NAME: {
					iconText = source.Name();
					break;
				}
				case UnitPanel::UTEXT_HEALTH: {
					iconText = std::to_string(source.Health()) + "/" 
						+ std::to_string(source.MaxHealth());
					break;
				}
				case UnitPanel::UTEXT_WEAPON: {
					if (source.Spec()->Attack(0) == nullptr) {
						iconText = "Unarmed";
					} else {
						iconText = std::to_string(static_cast<int>(
									std::floor(100*source.Accuracy()) ))
								+ "% | " + std::to_string(source.AttackRate()) 
								+ "x " + std::to_string(source.Damage()) + " " 
								+ source.DamageType();
					}
					break;
				}
			}
			if (!iconText.empty()) newIcon->AddText(iconText, textBox);
		}

		output.push_back(std::move(newIcon));
	}
	return output;
}

void InfoPanel::UpdateFromBuilding(const Building& source) {
	if (panelMode != INFO_PANEL_BUILDING) {
		dividers = BuildingModeDividers();
		panelMode = INFO_PANEL_BUILDING;
	}

	icons = IconsFromBuilding(source);
}

void InfoPanel::UpdateFromBuildingPrototype(const BuildingPrototype& source) {
	if (panelMode != INFO_PANEL_BUILDING) {
		dividers = BuildingModeDividers();
		panelMode = INFO_PANEL_BUILDING;
	}

	icons = IconsFromBuildingPrototype(source);
}

namespace BldgPanel {
	constexpr int numberOfHorizontal = 3;
	constexpr int horizontalX[numberOfHorizontal] = { 154, 154 };
	constexpr int horizontalY[numberOfHorizontal] = { 54 , 104 };
	constexpr int horizontalL[numberOfHorizontal] = { 250, 250 };

	constexpr int numberOfVertical = 5;
	constexpr int verticalX[numberOfVertical]   = { 204, 204, 254, 304, 354 };
	constexpr int verticalY[numberOfVertical]   = { 54 , 104, 104, 104, 104 };
	constexpr int verticalL[numberOfVertical]   = { 50 , 50 , 50 , 50 , 50  };

	enum IconType { BICON_PORTRAIT, BICON_FACTION, BICON_POWER, BICON_FOOD,
		BICON_CARBON, BICON_SILICON, BICON_IRON };
	enum TextType { BTEXT_NONE, BTEXT_NAME, BTEXT_POWER, BTEXT_FOOD, 
		BTEXT_CARBON, BTEXT_SILICON, BTEXT_IRON };
	constexpr int numberOfIcons = 7;
	constexpr IconType iconType[numberOfIcons] = { BICON_PORTRAIT, 
		BICON_FACTION, BICON_POWER, BICON_FOOD, BICON_CARBON, BICON_SILICON,
		BICON_IRON };
	constexpr int iconX[numberOfIcons] = { 5  , 156, 206, 156, 206, 256, 306 };
	constexpr int iconY[numberOfIcons] = { 5  , 56 , 56 , 106, 106, 106, 106 };
	constexpr TextType iconText[numberOfIcons] = { BTEXT_NAME, BTEXT_NONE, 
		BTEXT_POWER, BTEXT_FOOD, BTEXT_CARBON, BTEXT_SILICON, BTEXT_IRON };
	constexpr int textX[numberOfIcons] = { 225, 0  , 74 , 25 , 25 , 25 , 25  };
	constexpr int textY[numberOfIcons] = { 25 , 0  , 25 , 30 , 30 , 30 , 30  };
	constexpr int textW[numberOfIcons] = { 399, 0  , 199, 48 , 48 , 48 , 48  };
	constexpr int textH[numberOfIcons] = { 48 , 0  , 48 , 48 , 48 , 48 , 48  };
} // namespace BldgPanel

std::vector<std::unique_ptr<UIElement>> InfoPanel::BuildingModeDividers() {
	return MakeDividers(BldgPanel::numberOfHorizontal, BldgPanel::horizontalX,
			BldgPanel::horizontalY, BldgPanel::horizontalL, 
			BldgPanel::numberOfVertical, BldgPanel::verticalX, 
			BldgPanel::verticalY, BldgPanel::verticalL);
}

std::vector<std::unique_ptr<UIElement>> InfoPanel::IconsFromBuilding(
		const Building& source) {
	std::vector<std::unique_ptr<UIElement>> output;
	output = IconsFromBuildingType(*source.Type(), source.PoweredOn());
	output.push_back(BuildingFactionIcon(source.Faction()));
	return output;
}

std::vector<std::unique_ptr<UIElement>> InfoPanel::IconsFromBuildingPrototype(
		const BuildingPrototype& source) {
	std::vector<std::unique_ptr<UIElement>> output;
	output = IconsFromBuildingType(*source.Type(), false);
	output.push_back(BuildingFactionIcon(0));
	return output;
}

std::vector<std::unique_ptr<UIElement>> InfoPanel::IconsFromBuildingType(
		const BuildingType& source, const bool poweredOn) {
	std::vector<std::unique_ptr<UIElement>> output;
	for (int i = 0; i < BldgPanel::numberOfIcons; ++i) {
		File::Path iconPath = "";
		switch (BldgPanel::iconType[i]) {
			case BldgPanel::BICON_PORTRAIT: {
				iconPath = File::Path("portraits") / "portrait_";
				iconPath += source.PathName();
				break;
			}
			case BldgPanel::BICON_FACTION: {
				break;
			}
			case BldgPanel::BICON_POWER: {
				if (poweredOn) {
					iconPath = "power_on";
				} else {
					iconPath = "power_off";
				}
				break;
			}
			case BldgPanel::BICON_FOOD: {
				iconPath = "food_cost";
				break;
			}
			case BldgPanel::BICON_CARBON: {
				iconPath = "carbon_cost";
				break;
			}
			case BldgPanel::BICON_SILICON: {
				iconPath = "silicon_cost";
				break;
			}
			case BldgPanel::BICON_IRON: {
				iconPath = "iron_cost";
				break;
			}
		}
		if (iconPath.empty()) continue;

		std::unique_ptr<UIElement> newIcon = std::make_unique<UIElement>(ren, 
				iconPath.string(), X() + BldgPanel::iconX[i], 
				Y() + BldgPanel::iconY[i] );

		if (BldgPanel::iconText[i] != BldgPanel::BTEXT_NONE) {
			std::string iconText("");
			SDL_Rect textBox = MakeSDLRect(
					BldgPanel::textX[i], BldgPanel::textY[i], 
					BldgPanel::textW[i], BldgPanel::textH[i] );
			textcolor_t textColor = BLACK;
			switch (BldgPanel::iconText[i]) {
				case BldgPanel::BTEXT_NONE: {
					break;
				}
				case BldgPanel::BTEXT_NAME: {
					iconText = source.Name();
					break;
				}
				case BldgPanel::BTEXT_POWER: {
					if (poweredOn) {
						if (source.PowerProduction() > 0) {
							iconText = "+" 
								+ std::to_string(source.PowerProduction());
							textColor = BLUE;
						} else {
							iconText = std::to_string(source.PowerConsumption());
							textColor = BLACK;
						}
					} else {
						iconText = "0";
						textColor = BLACK;
					}
					break;
				}
				case BldgPanel::BTEXT_FOOD: {
					iconText = std::to_string(source.Cost()[0]);
					break;
				}
				case BldgPanel::BTEXT_CARBON: {
					iconText = std::to_string(source.Cost()[1]);
					break;
				}
				case BldgPanel::BTEXT_SILICON: {
					iconText = std::to_string(source.Cost()[2]);
					break;
				}
				case BldgPanel::BTEXT_IRON: {
					iconText = std::to_string(source.Cost()[3]);
					break;
				}
			}
			if (!iconText.empty()) {
				newIcon->AddText(iconText, textBox, nullptr, textColor);
			}
		}

		output.push_back(std::move(newIcon));
	}
	return output;
}

std::unique_ptr<UIElement> InfoPanel::BuildingFactionIcon(const char faction) {
	for (int i = 0; i < BldgPanel::numberOfIcons; ++i) {
		if (BldgPanel::iconType[i] == BldgPanel::BICON_FACTION) {
			std::string iconName = "factioncolor_p" + std::to_string(faction);
			return std::make_unique<UIElement>(ren, iconName,
					X() + BldgPanel::iconX[i], Y() + BldgPanel::iconY[i]);
		}
	}
	return nullptr;
}

} // namespace TerraNova
