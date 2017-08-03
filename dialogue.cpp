#include "dialogue.hpp"

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

Dialogue::DecisionPoint* Dialogue::DecisionAt(const size_t n) {
	for(auto& dp : decisionPoints){
		if(dp.beforeLine == n) return &dp;
	}
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
	currentLine = -1;
	Advance();
}

bool DialogueBox::Advance(){
	//std::cout << "Advance: " << currentLine << "->";
	if(!CanAdvance()) return false;
	currentLine = (currentLine + 1);
	if(currentLine == dialogue->Length()) return true;
	currentDecision = dialogue->DecisionAt(currentLine);
	//std::cout << "Current line: " << currentLine << "\nAddress of current "
		//<< "decision: " << currentDecision << std::endl;
	if(currentDecision){
		DisplayDecision();
	} else {
		DisplayLine();
	}
	return false;
}

void DialogueBox::DisplayLine(){
	//std::cout << currentLine << " of " << dialogue->Length() << std::endl;
	if(currentLine >= dialogue->Length()){
		std::cerr << "Error: asked to display line " << currentLine << " which "
			<< "is higher than the dialogue length " << dialogue->Length()
			<< "." << std::endl;
		return;
	}
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	AddText(CurrentLine(), boundingBox, dialogueFont);
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
	}
}

std::string DialogueBox::CurrentLine(){
	std::string ret(dialogue->Line(currentLine));
	std::regex extractor("@(.*?)@");
	auto command_begin = std::sregex_iterator(ret.begin(), ret.end(), extractor);
	auto command_end = std::sregex_iterator();
	for(std::sregex_iterator it = command_begin; it != command_end; ++it){
		std::string command = it->str();
		//std::cout << "Found a command in the dialogue: " << command << std::endl;
		ParseCommand(command);
	}
	return std::regex_replace(ret, extractor, "");
}

bool DialogueBox::CanAdvance() const{
	return currentDecision == nullptr;
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
	SDL_Rect boundingBox;
	boundingBox.x = DIALOGUE_BOX_WIDTH/2;
	boundingBox.y = DIALOGUE_BOX_HEIGHT/2;
	boundingBox.w = DIALOGUE_BOX_WIDTH*9/10;
	boundingBox.h = DIALOGUE_BOX_HEIGHT*9/10;
	AddText(decText, boundingBox, dialogueFont);
}

bool DialogueBox::MakeDecision(const unsigned int n){
	if(!currentDecision) return true;
	if(n-1 >= currentDecision->options.size()) return false;
	currentLine = currentDecision->jumpTo[n-1];
	if(currentLine >= dialogue->Length()){
		std::cerr << "Error: decision point jumped a dialogue to an invalid "
			<< "address: " << currentLine << " out of " << dialogue->Length()
			<< "." << std::endl;
	}
	currentDecision = nullptr;
	DisplayLine();
	return true;
}

void DialogueBox::Render() const{
	for(auto& portrait : portraits) portrait.Render();
	UIElement::Render();
	if(speakerName) speakerName->Render();
	if(CanAdvance()) advanceArrow.Render();
}
