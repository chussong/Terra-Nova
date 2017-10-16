#include "event.hpp"

namespace TerraNova {

LocationTrigger::LocationTrigger(const std::vector<std::array<unsigned int,2>>& locations): 
		locations(locations) {
}

LocationTrigger::LocationTrigger(std::vector<std::array<unsigned int,2>>&& locations): 
		locations(std::move(locations)) { 
}

Event::Event(std::string name, std::unique_ptr<EventTrigger>&& trigger): 
	name(name), trigger(std::move(trigger)) {
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
	//return !linkedDialogueName.empty();
	//return !linkedDialoguePath.empty();
	return linkedDialogue != nullptr;
}

Dialogue* Event::LinkedDialogue() const {
	//return linkedDialogueName;
	//return linkedDialoguePath.native();
	return linkedDialogue.get();
}

bool Event::HasStartTrigger() const {
	return (dynamic_cast<MapStartTrigger*>(trigger.get()) != nullptr);
}

bool Event::HasLocationTrigger() const {
	return (dynamic_cast<LocationTrigger*>(trigger.get()) != nullptr);
}

const std::vector<std::array<unsigned int,2>>* Event::TriggerLocations() const {
	if(!HasLocationTrigger()) return nullptr;
	return &(dynamic_cast<LocationTrigger*>(trigger.get())->Locations());
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
	/*for(auto& file : File::ReadAll("chapters/" + chapter + "/events") ) {
		std::vector<Event> pileOfEvents = ReadEvents(file);
		for(auto& event : pileOfEvents) ret.push_back(event);
	}*/
	return ret;
}

std::vector<Event> Event::ReadEvents(const File::fs::path& pathToEventFile) {
	std::vector<std::string> file = File::ReadFromFullPath(pathToEventFile);
	if(file.size() == 0) return {};
	std::vector<Event> ret;
	for(auto line = 0u; line < file.size(); ++line) {
		size_t bracePos = file[line].find('{');
		if(bracePos != std::string::npos){
			std::string eventName = file[line].substr(0, bracePos);
			std::vector<std::string> contents = File::GetField(file, line);
			std::unique_ptr<EventTrigger> trigger;
			for(auto innerLine = 0u; innerLine < contents.size(); ++innerLine) {
				// should be one of those boost comparison functions but w/ev
				if(contents[innerLine] == "map_start_trigger") {
					trigger = std::make_unique<MapStartTrigger>();
					continue;
				}
				if(contents[innerLine].compare(0, 16, "location_trigger") == 0){
					trigger = MakeLocationTrigger(File::GetField(contents, innerLine));
				}
			}
			ret.emplace_back(eventName, std::move(trigger));
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
			}
		}
	}
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

} // namespace TerraNova
