#include "event.hpp"

namespace TerraNova {

LocationTrigger::LocationTrigger(const std::vector<std::array<unsigned int,2>>& locations): 
		locations(locations) {
}

LocationTrigger::LocationTrigger(std::vector<std::array<unsigned int,2>>&& locations): 
		locations(std::move(locations)) { 
}

BuildingTrigger::BuildingTrigger(const int triggeringPlayer, 
		const std::string& triggeringBuilding): 
		triggeringPlayer(triggeringPlayer), 
		triggeringBuilding(triggeringBuilding) {
}

int BuildingTrigger::TriggeringPlayer() const {
	return triggeringPlayer;
}

const std::string& BuildingTrigger::TriggeringBuilding() const {
	return triggeringBuilding;
}

Event::Event(std::string name, std::unique_ptr<EventTrigger>&& trigger): 
	name(name), trigger(std::move(trigger)) {
	/*if (name.empty()) {
		throw (std::runtime_error("Event constructed with no name"));
	}
	if (trigger == nullptr) {
		throw (std::runtime_error("Event constructed with no trigger"));
	}*/
}

// would like to deprecate this overload
/*void Event::LinkDialogue(const std::string& dialogueName) {
	linkedDialogueName = boost::trim_copy(dialogueName);
}*/

void Event::LinkDialogue(const File::fs::path& pathToDialogue) {
	//linkedDialoguePath = pathToDialogue;
	linkedDialogue = std::make_unique<Dialogue>(pathToDialogue);
}

bool Event::HasLinkedDialogue() const {
	return linkedDialogue != nullptr;
}

Dialogue* Event::LinkedDialogue() const {
	return linkedDialogue.get();
}

std::string Event::TriggerType() const {
	if (trigger == nullptr) return "NULLPTR";
	return trigger->TriggerType();
}

bool Event::HasStartTrigger() const {
	if (trigger == nullptr) {
		std::cerr << "Error: event with name " << Name() << " has no trigger."
			<< std::endl;
		return false;
	}
	return (dynamic_cast<MapStartTrigger*>(trigger.get()) != nullptr);
}

int Event::TriggeringPlayer() const {
	return trigger->TriggeringPlayer();
}

bool Event::HasLocationTrigger() const {
	if (trigger == nullptr) {
		std::cerr << "Error: event with name " << Name() << " has no trigger."
			<< std::endl;
		return false;
	}
	return (dynamic_cast<LocationTrigger*>(trigger.get()) != nullptr);
}

const std::vector<std::array<unsigned int,2>>* Event::TriggerLocations() const {
	if(!HasLocationTrigger()) return nullptr;
	return &(dynamic_cast<LocationTrigger*>(trigger.get())->Locations());
}

bool Event::HasBuildingTrigger() const {
	if (trigger == nullptr) {
		std::cerr << "Error: event with name " << Name() << " has no trigger."
			<< std::endl;
		return false;
	}
	return (dynamic_cast<BuildingTrigger*>(trigger.get()) != nullptr);
}

const std::string* Event::TriggeringBuilding() const {
	if(!HasBuildingTrigger()) return nullptr;
	return &(dynamic_cast<BuildingTrigger*>(trigger.get())->TriggeringBuilding());
}

// this is a silly way to do this; if we're going to link perhaps it should just
// be a forward_list
std::vector<Event> Event::ReadEventDirectory(const std::string& chapter) {
	std::vector<Event> ret;
	File::fs::path eventPath = File::BasePath() / "chapters" / chapter / "events";
	for(auto& eventVector : File::ForEachFile(eventPath, ReadEvents)) {
		for(auto& event : eventVector) {
			ret.push_back(std::move(event));
		}
	}
	// std::cout << "Returning " << ret.size() << " events:" << std::endl;
	// for (auto& e : ret) {
		// std::cout << e.Name() << " with trigger type " << e.TriggerType()
			// << std::endl;
	// }
	return ret;
}

std::vector<Event> Event::ReadEvents(const File::fs::path& pathToEventFile) {
	std::vector<std::string> file = File::ReadFromFullPath(pathToEventFile);
	if(file.size() == 0) return {};
	std::vector<Event> ret;
	std::string eventName = "EVENT_NAME";
	//std::cout << "Parsing lines from event file: " << std::endl;
	for(auto line = 0u; line < file.size(); ++line) {
		//std::cout << file[line] << std::endl;
		size_t bracePos = file[line].find('{');
		if(bracePos != std::string::npos) {
			if (eventName == "EVENT_NAME") {
				eventName = file[line].substr(0, bracePos);
			}
			std::vector<std::string> contents = File::GetField(file, line);
			if (contents.empty()) {
				std::cerr << "Error: found no contents when parsing event "
					<< eventName << std::endl;
				eventName = "EVENT_NAME";
				continue;
			}
			std::unique_ptr<EventTrigger> trigger;
			for(auto innerLine = 0u; innerLine < contents.size(); ++innerLine) {
				// should be one of those boost comparison functions but w/ev
				//std::cout << "Parsing this line for event triggers:" 
					//<< contents[innerLine] << std::endl;
				if(contents[innerLine] == "map_start_trigger") {
					//std::cout << "Found a map start trigger." << std::endl;
					trigger = std::make_unique<MapStartTrigger>();
					continue;
				}
				if(contents[innerLine].compare(0, 16, "location_trigger") == 0){
					//std::cout << "Found a location trigger." << std::endl;
					trigger = MakeLocationTrigger(File::GetField(contents, innerLine));
				}
				if(contents[innerLine].compare(0, 16, "building_trigger") == 0){
					//std::cout << "Found a building trigger." << std::endl;
					trigger = MakeBuildingTrigger(File::GetField(contents, innerLine));
				}
			}
			if (trigger == nullptr) {
				std::cerr << "Error: no valid trigger found." << std::endl;
			}
			ret.emplace_back(eventName, std::move(trigger));
			eventName = "EVENT_NAME";
			for(auto innerLine = 0u; innerLine < contents.size(); ++innerLine) {
				if(contents[innerLine].compare(0, 13, "play_dialogue") == 0) {
					File::fs::path pathToDialogue = pathToEventFile.parent_path();
					pathToDialogue = pathToDialogue.parent_path();
					pathToDialogue.append("dialogues");
					pathToDialogue.append(
							boost::trim_copy(contents[innerLine].substr(14))
							+ ".txt");
					ret.back().LinkDialogue(pathToDialogue);
					//ret.back().LinkDialogue(contents[innerLine].substr(14));
				}
				if(contents[innerLine].compare(0, 7, "victory") == 0) {
					ret.back().SetVictory();
				}
			}
			line += contents.size() + 1;
		}
	}
	// std::cout << "Returning " << ret.size() << " events:" << std::endl;
	// for (auto& e : ret) {
		// std::cout << e.Name() << " with trigger type " << e.TriggerType()
			// << std::endl;
	// }
	return ret;
}

std::unique_ptr<LocationTrigger> Event::MakeLocationTrigger(
		const std::vector<std::string>& source) {
	std::vector<std::array<unsigned int,2>> parsedLocations;
	for(auto& location : source) {
		size_t commaPos = location.find(',');
		parsedLocations.push_back( {{ 
			static_cast<unsigned int>(std::stoul(location.substr(0, commaPos))),
			static_cast<unsigned int>(std::stoul(location.substr(commaPos+1)))
			}} );
	}
	return std::make_unique<LocationTrigger>(std::move(parsedLocations));
}

std::unique_ptr<BuildingTrigger> Event::MakeBuildingTrigger(
		const std::vector<std::string>& source) {
	int player = 0;
	std::string name = "";
	for(auto& line : source) {
		File::Entry entry = File::GetEntry(line);
		if (entry.name == "player") {
			player = std::stol(entry.value);
		} else if (entry.name == "name") {
			name = entry.value;
		}
	}
	if (player == 0 || name.empty()) {
		std::cout << "Error: attempted to construct a BuildingTrigger but was "
			<< "not able to parse all of the required fields." << std::endl;
		// SendToErrorLog(source);
	}
	return std::make_unique<BuildingTrigger>(player, name);
}

} // namespace TerraNova
