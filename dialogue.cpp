#include "dialogue.hpp"

Dialogue::Dialogue(const std::string& filePath){
	std::vector<std::string> file = File::Read(filePath);
	characters = File::GetSection(file, "Characters");
	lines = ParseLines(File::GetSection(file, "Lines"));
	hooks = ExtractHooks(lines);
	decisionPoints = ParseDecisions(File::GetSection(file, "Decisions"), hooks);
}

std::vector<std::string> Dialogue::ParseLines(
		const std::vector<std::string>& source){
	//std::cout << "Parsing the following lines of dialogue:" << std::endl;
	//for(auto& ln : source) std::cout << ln << std::endl;
	if(source.size() == 0) return std::vector<std::string>();
	std::vector<std::string> ret;
	auto it = source.begin();
	if(it->size() == 0 || it->at(0) != '-'){
		std::cerr << "Error: each line of dialogue (including in particular the"
			<< " first one) should start with a '-'." << std::endl;
		return ret;
	}
	std::string currentLine = boost::trim_copy(it->substr(1));
	do{
		++it;
		if(it != source.end() && it->length() == 0){
			std::cerr << "Error: asked to parse a dialogue line with length 0; "
				<< "it should not be possible to receive an empty line." 
				<< std::endl;
			return ret;
		}
		if(it == source.end() || it->front() == '-'){
			ret.push_back(currentLine);
			if(it != source.end()) currentLine = boost::trim_copy(it->substr(1));
		} else {
			currentLine += " " + boost::trim_copy(*it);
		}
	}while(it != source.end());
	//std::cout << "Parsed to produce the following lines:" << std::endl;
	//for(auto& ln : ret) std::cout << ln << std::endl;
	return ret;
}

std::vector<Dialogue::Hook> Dialogue::ExtractHooks(std::vector<std::string>& lines){
	std::vector<Hook> ret;
	std::regex extractor("@hook=(.*?)@");
	for(auto lineNum = 0u; lineNum < lines.size(); ++lineNum){
		auto hook_begin = std::sregex_iterator(lines[lineNum].begin(), 
				lines[lineNum].end(), extractor);
		auto hook_end = std::sregex_iterator();
		for(auto it = hook_begin; it != hook_end; ++it){
			ret.emplace_back(it->str(1), lineNum);
			//std::cout << "Found a hook: (" << ret.back().name << "," 
				//<< ret.back().lineNumber << ")." << std::endl;
		}
		lines[lineNum] = std::regex_replace(lines[lineNum], extractor, "");
	}
	//std::cout << "After extracting hooks, left with these lines:" << std::endl;
	//for(auto& ln : lines) std::cout << ln << std::endl;
	return ret;
}

std::vector<Dialogue::DecisionPoint> Dialogue::ParseDecisions(
		const std::vector<std::string>& source, 
		const std::vector<Dialogue::Hook>& hooks){
	//std::cout << "Parsing decisions from the following lines:" << std::endl;
	//for(auto& ln : source) std::cout << ln << std::endl;
	std::vector<DecisionPoint> ret;

	auto startIt = source.begin();
	while(startIt != source.end()){
		if(startIt->find('{') != std::string::npos){
			std::vector<std::string> options;
			std::vector<size_t> destinationLines;

			std::string decisionName = startIt->substr(0, startIt->find('{'));
			boost::trim(decisionName);
			auto insideIt = startIt + 1;
			//std::cout << "Parsing a decision called " << decisionName << std::endl;
			while(insideIt != source.end() && insideIt->at(0) != '}'){
				//std::cout << "Found this line: " << *insideIt << std::endl;
				if(boost::trim_copy(*insideIt).compare(0, 7, "option:") == 0){
					size_t destinationLine = -1u;
					std::string optionText = FindOptionText(*insideIt);
					std::string hookName = FindHookName(*insideIt);
					boost::trim(hookName);
					for(auto& knownHook : hooks){
						if(boost::iequals(hookName, knownHook.name)){
							destinationLine = knownHook.lineNumber;
						}
					}
					if(optionText == "" || destinationLine == -1u){
						std::cerr << "Error: attempted to construct a decision "
							<< "but was not able to read an option." << std::endl;
						return ret;
					} else {
						options.push_back(optionText);
						destinationLines.push_back(destinationLine);
					}
				}
				++insideIt;
			}
			ret.emplace_back(decisionName, options, destinationLines);
			startIt = insideIt;
		} else {
			++startIt;
		}
	}
	return ret;
}

std::string Dialogue::FindOptionText(const std::string& sourceLine){
	std::smatch match;
	std::regex extractor("option:.*\"(.*?)\",");
	
	if(std::regex_search(sourceLine, match, extractor)){
		if(match.size() == 0){
			std::cerr << "Error: a FindOptionText search returned true but "
				<< "somehow failed to fill the match object." << std::endl;
			return "";
		}
		//std::cout << "Found " << match.size()-1 << " matches; here's the first: "
			//<< match[1] << std::endl;
		return boost::trim_copy(std::string(match[1]));
	} else {
		std::cerr << "Error: could not find any text for a dialogue option."
			<< std::endl;
		return "";
	}
}

std::string Dialogue::FindHookName(const std::string& sourceLine){
	std::smatch match;
	std::regex extractor("\",(.*)");
	
	if(std::regex_search(sourceLine, match, extractor)){
		if(match.size() == 0){
			std::cerr << "Error: a FindHookName search returned true but "
				<< "somehow failed to fill the match object." << std::endl;
			return "";
		}
		//std::cout << "Found " << match.size()-1 << " matches; here's the first: "
			//<< match[1] << std::endl;
		return boost::trim_copy(std::string(match[1]));
	} else {
		std::cerr << "Error: could not find any text for a dialogue option."
			<< std::endl;
		return "";
	}
}

void Dialogue::AddLine(std::string line){
	//if(line.length() > 80) line.insert(80, "\n");
	lines.push_back(line);
}

void Dialogue::AddDecisionPoint(DecisionPoint dp){
	decisionPoints.push_back(dp);
}

size_t Dialogue::Length() const{
	return lines.size();
}

const std::string& Dialogue::Line(const size_t lineNum) const{
	return lines[lineNum];
}

/*Dialogue::DecisionPoint* Dialogue::DecisionAt(const size_t n) {
	for(auto& dp : decisionPoints){
		if(dp.beforeLine == n) return &dp;
	}
	return nullptr;
}*/

const Dialogue::DecisionPoint* Dialogue::DecisionNamed(const std::string& name) const{
	for(auto& decPt : decisionPoints){
		if(decPt.name == name) return &decPt;
	}
	std::cerr << "Error: dialogue asked to find a decision point named " << name
		<< " but was not able to." << std::endl;
	return nullptr;
}

void Dialogue::AddCharacter(const std::string& name){
	characters.push_back(name);
}

const std::vector<std::string>& Dialogue::Characters() const{
	return characters;
}

DialogueBox::DialogueBox(SDL_Renderer* ren, Dialogue* dialogue):
	UIElement(ren, "dialogue_box_background", DIALOGUE_BOX_X,
			DIALOGUE_BOX_Y), 
	advanceArrow(ren, "dialogue_box_arrow", 
			DIALOGUE_BOX_X + 9*DIALOGUE_BOX_WIDTH/10,
			DIALOGUE_BOX_Y + 7*DIALOGUE_BOX_HEIGHT/10),
	closeBox(ren, "dialogue_box_close",
			DIALOGUE_BOX_X + 9*DIALOGUE_BOX_WIDTH/10,
			DIALOGUE_BOX_Y + 7*DIALOGUE_BOX_HEIGHT/10){
	SetDialogue(dialogue);
}

void DialogueBox::SetDialogue(Dialogue* newDialogue){
	if(!newDialogue){
		std::cerr << "Error: tried to set DialogueBox's dialogue to a nullptr."
			<< std::endl;
		return;
	}
	dialogue = newDialogue;
	portraits.clear();
	int position = 0;
	for(auto& charName : dialogue->Characters()){
		portraits.emplace_back(ren, charName, position);
		switch(position){
			case 0: position = 3;
					break;
			case 1: position = 2;
					break;
			case 2: position = 0;
					break;
			case 3: position = 1;
					break;
		}
	}
	currentLineNumber = -1;
	Advance();
}

// return true if the dialogue is over the box should be closed
bool DialogueBox::Advance(){
	if(currentDecision){
		DisplayDecision();
		return false;
	}
	//std::cout << "Advance: " << currentLineNumber << "->";
	if(endAfterCurrentLine) return true;
	if(!CanAdvance()) return false;
	currentLineNumber = (currentLineNumber + 1);
	if(currentLineNumber == dialogue->Length()) return true;
	currentLine = LoadLine();
	//currentDecision = dialogue->DecisionAt(currentLineNumber);
	//std::cout << "Current line: " << currentLineNumber << "\nAddress of current "
		//<< "decision: " << currentDecision << std::endl;
	DisplayLine();
	return false;
}

void DialogueBox::DisplayLine(){
	//std::cout << currentLineNumber << " of " << dialogue->Length() << std::endl;
	if(currentLineNumber >= dialogue->Length()){
		std::cerr << "Error: asked to display line " << currentLineNumber << " which "
			<< "is higher than the dialogue length " << dialogue->Length()
			<< "." << std::endl;
		return;
	}
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	std::string displayLine = CurrentLine();
	if(displayLine.length() == 0){
		displayLine = "Error: asked to display an empty dialogue line.";
	}
	//std::cout << "Displaying the following line: " << displayLine << std::endl;
	AddText(displayLine, boundingBox, dialogueFont);
}

void DialogueBox::ActivateSpeaker(const size_t speakerNumber){
	currentSpeaker = speakerNumber;
	speakerName = std::make_unique<UIElement>(ren, "dialogue_speaker_name",
			layout.x + layout.w/20, layout.y + layout.h/20);
	SDL_Rect boundingBox;
	boundingBox.x = layout.w/10;
	boundingBox.y = layout.h/10;
	boundingBox.w = layout.w/5;
	boundingBox.h = layout.w/5;
	speakerName->AddText(portraits[currentSpeaker].Name(), boundingBox,
			dialogueFont);
}

std::string DialogueBox::LoadLine(){
	std::string line = dialogue->Line(currentLineNumber);
	//std::cout << "Loading line #" << lineNumber << ":" << line << std::endl;
	std::regex extractor("@(.*?)@");
	auto command_begin = std::sregex_iterator(line.begin(), line.end(), 
			extractor);
	auto command_end = std::sregex_iterator();
	for(std::sregex_iterator it = command_begin; it != command_end; ++it){
		std::string command = it->str();
		//std::cout << "Found a command in the dialogue: " << command << std::endl;
		ParseCommand(command);
	}
	//std::cout << "After replacement it becomes: " 
		//<< std::regex_replace(line, extractor, "") << std::endl;
	return std::regex_replace(line, extractor, "");
}

void DialogueBox::ParseCommand(const std::string& command){
	if(command.compare(0, 8, "@active=") == 0){
		try{
			ActivateSpeaker(std::stoul(command.substr(8, command.length()-9)));
		}
		catch(std::invalid_argument){
			std::cerr << "Error: got an @active=#@ dialogue command but could not read "
				<< "the number which was supposed to be made active." << std::endl;
			return;
		}
		catch(std::out_of_range){
			std::cerr << "Error: got an @active=#@ dialogue command but the "
				<< "number was out of range." << std::endl;
			return;
		}
		return;
	}
	if(command.compare(0, 10, "@decision=") == 0){
		currentDecision = dialogue->DecisionNamed(
				command.substr(10, command.length()-11));
		return;
	}
	if(command.compare(0, 5, "@end@") == 0){
		endAfterCurrentLine = true;
		return;
	}
}

const std::string& DialogueBox::CurrentLine() const{
	return currentLine;
}

bool DialogueBox::CanAdvance() const{
	return !endAfterCurrentLine && !makingDecision;
}

void DialogueBox::DisplayDecision(){
	if(!currentDecision){
		std::cerr << "Error: asked to display a non-existent decision." << std::endl;
		return;
	}
	std::string decText = "";
	for(auto i = 0u; i < currentDecision->options.size(); ++i){
		decText += std::to_string(i+1) + ". " + currentDecision->options[i] + "\n";
	}
	if(decText.length() == 0){
		std::cerr << "Error: all options of the requested decision were blank."
			<< std::endl;
		decText = "Display error: all decisions options are blank.";
	}
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	AddText(decText, boundingBox, dialogueFont);
	makingDecision = true;
}

// return true if a decision was successfully made
bool DialogueBox::MakeDecision(const unsigned int n){
	if(!currentDecision) return true;
	if(n-1 >= currentDecision->options.size()) return false;
	currentLineNumber = currentDecision->jumpTo[n-1];
	if(currentLineNumber >= dialogue->Length()){
		std::cerr << "Error: decision point jumped a dialogue to an invalid "
			<< "address: " << currentLineNumber << " out of " << dialogue->Length()
			<< "." << std::endl;
	}
	currentDecision = nullptr;
	currentLine = LoadLine();
	DisplayLine();
	makingDecision = false;
	return true;
}

void DialogueBox::Render() const{
	for(auto& portrait : portraits) portrait.Render();
	UIElement::Render();
	if(speakerName) speakerName->Render();
	if(CanAdvance()) advanceArrow.Render();
	if(endAfterCurrentLine) closeBox.Render();
}
