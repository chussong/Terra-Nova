#ifndef EVENT_HPP
#define EVENT_HPP

#include <vector>
#include <array>
#include <string>
#include <memory>

#include <boost/algorithm/string/trim.hpp>

#include "file.hpp"
#include "dialogue.hpp"

namespace TerraNova {

class EventTrigger { 
	public:
		virtual std::string TriggerType() const { return "InvalidTrigger"; }
		virtual int TriggeringPlayer() const { return 0; }
		virtual ~EventTrigger() = default;
};

class MapStartTrigger : public EventTrigger {
	public:
		std::string TriggerType() const { return "MapStartTrigger"; }
};

class LocationTrigger : public EventTrigger {
	std::vector<std::array<unsigned int,2>> locations;
	public:
		explicit LocationTrigger(const std::vector<std::array<unsigned int,2>>& locations);
		explicit LocationTrigger(std::vector<std::array<unsigned int,2>>&& locations);
		std::string TriggerType() const { return "LocationTrigger"; }
		const std::vector<std::array<unsigned int,2>>& Locations() const 
				{ return locations; }
};

class BuildingTrigger : public EventTrigger {
	public:
		BuildingTrigger(const int triggeringFaction, 
				const std::string& triggeringBuilding);
		std::string TriggerType() const { return "BuildingTrigger"; }

		int TriggeringPlayer() const;
		const std::string& TriggeringBuilding() const;

	private:
		const int triggeringPlayer;
		const std::string triggeringBuilding;
};

class Event {
	std::string name;
	std::unique_ptr<EventTrigger> trigger;
	bool repeatable = false;
	bool finished = false;
	bool victory = false;

	//std::string linkedDialogueName;
	//File::fs::path linkedDialoguePath;
	std::unique_ptr<Dialogue> linkedDialogue = nullptr;

	public:
		Event(std::string name, std::unique_ptr<EventTrigger>&& trigger);

		//void LinkDialogue(const std::string& dialogueName);
		void LinkDialogue(const File::fs::path& pathToDialogue);
		bool HasLinkedDialogue() const;
		Dialogue* LinkedDialogue() const;

		const std::string& Name() const { return name; }

		std::string TriggerType() const;
		bool HasStartTrigger() const;
		int TriggeringPlayer() const;
		bool HasLocationTrigger() const;
		const std::vector<std::array<unsigned int,2>>* TriggerLocations() const;
		bool HasBuildingTrigger() const;
		const std::string* TriggeringBuilding() const;

		bool Repeatable() const { return repeatable; }
		bool Finished() const { return finished; }
		void SetFinished() { finished = true; }
		bool Victory() const { return victory; }
		void SetVictory() { victory = true; }

		static std::vector<Event> ReadEventDirectory(const std::string& chapter);
		//static std::vector<Event> ReadEvents(const std::vector<std::string>& file);
		static std::vector<Event> ReadEvents(const File::fs::path& pathToEventFile);
		static std::unique_ptr<LocationTrigger> MakeLocationTrigger(
				const std::vector<std::string>& source);
		static std::unique_ptr<BuildingTrigger> MakeBuildingTrigger(
				const std::vector<std::string>& source);
};

} // namespace TerraNova

#endif
