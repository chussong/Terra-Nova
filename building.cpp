#include "building.hpp"

namespace TerraNova {

int BuildingType::ID() const {
	return id;
}

const std::string& BuildingType::Name() const {
	return name;
}

std::string BuildingType::PathName() const {
	std::string output = boost::to_lower_copy(name);
	boost::replace_all(output, " ", "_");
	return output;
}

std::array<int, LAST_RESOURCE> BuildingType::Cost() const {
	return cost;
}

bool BuildingType::CanBuyWith(std::array<int, LAST_RESOURCE> availableResources) 
		const {
	for (auto i = 0u; i < LAST_RESOURCE; ++i) {
		if (availableResources[i] < cost[i]) return false;
	}
	return true;
}

int BuildingType::BuildTime() const {
	return buildTime;
}

/*void BuildingType::SetAllowedTerrain(const std::vector<std::shared_ptr<TileType>>& val){
	std::vector<std::weak_ptr<TileType>> weakVal;
	weakVal.resize(val.size());
	for(unsigned int i = 0; i < weakVal.size(); ++i) weakVal[i] = val[i];
	allowedTerrain = weakVal;
}

// this is obviously a very stupid implementation
std::vector<std::shared_ptr<TileType>> BuildingType::AllowedTerrain() const{
	std::vector<std::shared_ptr<TileType>> strongTerrain;
	strongTerrain.resize(allowedTerrain.size());
	for(auto i = 0u; i < strongTerrain.size(); ++i){
		strongTerrain[i] = allowedTerrain[i].lock();
	}
	return strongTerrain;
}*/

void BuildingType::SetCanHarvest(const bool val) {
	canHarvest = val;
}

bool BuildingType::CanHarvest() const {
	return canHarvest;
}

void BuildingType::SetAutomatic(const bool val) {
	automatic = val;
}

bool BuildingType::Automatic() const {
	return automatic;
}

void BuildingType::SetMaxOccupants(const int val) {
	maxOccupants = val;
}

unsigned int BuildingType::MaxOccupants() const {
	return maxOccupants;
}

void BuildingType::SetPowerProduction(const int val) {
	powerConsumption = -val;
}

int BuildingType::PowerProduction() const {
	return std::max(-powerConsumption, 0);
}

void BuildingType::SetPowerConsumption(const int val) {
	powerConsumption = val;
}

int BuildingType::PowerConsumption() const {
	return std::max(powerConsumption, 0);
}

void BuildingType::SetBonusResources(const std::array<int, LAST_RESOURCE>& val){
	bonusResources = val;
}

std::array<int, LAST_RESOURCE> BuildingType::BonusResources() const{
	return bonusResources;
}

void BuildingType::SetCanTrain(const std::vector<std::shared_ptr<UnitType>>& val){
	canTrain = val;
}

void BuildingType::SetCanTrain(const std::shared_ptr<UnitType> val) {
	canTrain.clear();
	canTrain.push_back(val);
}

std::vector<std::shared_ptr<UnitType>> BuildingType::CanTrain() const {
	return canTrain;
}

const std::string& BuildingPrototype::Name() const {
	return type->Name();
}

std::string BuildingPrototype::PathName() const {
	return type->PathName();
}

std::array<int, LAST_RESOURCE> BuildingPrototype::Cost() const {
	return type->Cost();
}

bool BuildingPrototype::CanBuyWith(
		std::array<int,LAST_RESOURCE> availableResources) const {
	return type->CanBuyWith(availableResources);
}

int BuildingPrototype::PowerProduction() const {
	return type->PowerProduction();
}

int BuildingPrototype::PowerConsumption() const {
	return type->PowerConsumption();
}

int BuildingPrototype::BuildTime() const {
	return type->BuildTime();
}

const BuildingType* BuildingPrototype::Type() const {
	return type;
}

const std::string& Building::Name() const {
	return type->Name();
}

std::string Building::PathName() const {
	return type->PathName();
}

std::array<int, LAST_RESOURCE> Building::Cost() const {
	return type->Cost();
}

bool Building::CanBuyWith(
		std::array<int,LAST_RESOURCE> availableResources) const {
	return type->CanBuyWith(availableResources);
}

int Building::BuildTime() const {
	return type->BuildTime();
}

void Building::StartConstruction() {
	turnsLeft = BuildTime();
}

int Building::TurnsLeft() const {
	return turnsLeft;
}

void Building::BuildTurn() {
	turnsLeft--;
	//std::cout << Name() << " build timer decremented, now " << turnsLeft << "."
		//<< std::endl;
}

bool Building::Finished() const {
	//std::cout << "Building " << Name() << " has " << turnsLeft << " turns left."
		//<< std::endl;
	return turnsLeft == 0;
}

bool Building::CanHarvest() const {
	//std::cout << "Checking if " << Name() << " can harvest." << std::endl;
	return turnsLeft == 0 && (type->CanHarvest() || type->Automatic());
}

int Building::PowerProduction() const {
	//if (!crewed) return 0;
	return type->PowerProduction();
}

int Building::PowerConsumption() const {
	return type->PowerConsumption();
}

bool Building::PoweredOn() const {
	return poweredOn;
}

// return true if building actually was powered on
bool Building::PowerOn() {
	//if (!crewed) return false;
	poweredOn = true;
	return true;
}

void Building::PowerOff() {
	poweredOn = false;
}

bool Building::Automatic() const {
	//std::cout << "Building " << Name() << " is ";
	//if (!type->Automatic()) std::cout << "not ";
	//std::cout << "automatic." << std::endl;
	return type->Automatic();
}

unsigned int Building::MaxOccupants() const {
	return type->MaxOccupants();
}

std::array<int, LAST_RESOURCE> Building::BonusResources() const {
	return type->BonusResources();
}

std::vector<std::shared_ptr<UnitType>> Building::CanTrain() const {
	return type->CanTrain();
}

void Building::StartTraining(std::shared_ptr<UnitType> newSpec) {
	if(!newSpec){
		std::cerr << "Error: a Building was assigned to respec a Unit to a "
			<< "blank specialization." << std::endl;
		return;
	}
	nowTraining = newSpec;
	turnsToTrain = newSpec->TrainingTime();
}

int Building::TurnsToTrain() const {
	return turnsToTrain;
}

std::shared_ptr<UnitType> Building::NowTraining() const {
	return nowTraining;
}

void Building::TrainingTurn() {
	turnsToTrain--;
}

void Building::FinishTraining() {
	nowTraining.reset();
}

} // namespace TerraNova
