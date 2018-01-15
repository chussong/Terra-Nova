#include "file.hpp"

namespace TerraNova {

namespace File {

namespace {
	Path basePath;
} // anonymous namespace

void Initialize(){
	basePath = Path(SDL_GetBasePath());
}

const Path& BasePath() {
	return basePath;
}

std::string AbsolutePath(const std::string& relativePath) {
	return (basePath / relativePath).native();
}

std::string JoinPaths(const std::vector<std::string>& paths) {
	if (paths.size() == 0) return "";
	Path pathForm(paths.front());
	for (auto it = paths.begin()+1; it != paths.end(); ++it) {
		pathForm /= *it;
	}
	return pathForm.native();
}

std::vector<std::string> ReadFromFullPath(const Path& fullPath){
	std::vector<std::string> ret;

	std::ifstream in(fullPath.native());
	if((in.rdstate() & std::ifstream::failbit) != 0){
		std::cerr << "Error: unable to open file at "
			<< fullPath << ". Does it exist?" << std::endl;
		return ret;
	}

	std::string line;
	do{
		std::getline(in, line);
		boost::trim(line);
		if(line.length() > 0 && line[0] != '%') ret.push_back(std::move(line));
	}while((in.rdstate() & std::ifstream::eofbit) == 0);

	//std::cout << "Read the following file:" << std::endl;
	//for(auto& ln : ret) std::cout << ln << std::endl;

	return ret;
}

std::vector<std::string> Read(const std::string& relativePath){
	return ReadFromFullPath(basePath / relativePath);
}

std::vector<std::vector<std::string>> ReadAll(const std::string& relativePath) {
	Path path = basePath / relativePath;
	std::vector<std::vector<std::string>> ret;

	for(auto& file : fs::directory_iterator(path)) {
		ret.push_back(ReadFromFullPath(file.path()));
	}

	return ret;
}

std::vector<std::string> GetSection(const std::vector<std::string>& source,
		const std::string& sectionName){
	for(auto startIt = source.begin(); startIt != source.end(); ++startIt){
		if(startIt->compare(0, sectionName.size()+2, "[" + sectionName + "]") == 0){
			auto endIt = startIt;
			do{
				++endIt;
				if(endIt == source.end() || endIt->compare(0, 1, "[") == 0){
					return std::vector<std::string>(startIt+1, endIt);
				}
			}while(endIt != source.end());
			std::cerr << "Error: found the start of the section " << sectionName
				<< " in a file but could not find the end." << std::endl;
			return std::vector<std::string>();
		}
	}
	//std::cerr << "Warning: asked to get the section " << sectionName << " from "
		//<< "a file but could not find it." << std::endl;
	return std::vector<std::string>();
}

std::vector<std::string> GetField(const std::vector<std::string>& source,
		const size_t startingLine) {
	if(startingLine >= source.size()) throw(std::out_of_range("File::GetField"));
	if(source[startingLine].find('{') == std::string::npos){
		throw(std::runtime_error("File::GetField -- startingLine has no '{'."));
	}
	int extraBraces = 0;
	for(auto it = source.begin() + startingLine + 1; it != source.end(); ++it) {
		//std::cout << "FIELD SEARCH | " << *it << std::endl;
		if(it->find('{') != std::string::npos) {
			++extraBraces;
			//std::cout << "++extraBraces: " << extraBraces << std::endl;
			continue;
		}
		if(it->find('}') != std::string::npos){
			if(extraBraces != 0) {
				--extraBraces;
				//std::cout << "--extraBraces: " << extraBraces << std::endl;
				continue;
			}
			std::vector<std::string> ret(source.begin() + startingLine+1, it);
			//std::cout << "Returning this field: " << std::endl;
			for(auto& line : ret){
				boost::trim(line);
				//std::cout << line << std::endl;
			}
			return ret;
		}
	}
	throw(std::runtime_error("File::GetField -- could not find '}' in input."));
}

Entry GetEntry(const std::string& line) {
	Entry ret;
	auto colonPos = line.find(':');
	if (colonPos == std::string::npos) {
		std::cerr << "Error: failed to parse an entry from this line:" 
			<< std::endl << line << std::endl;
		return ret;
	}
	ret.name = boost::trim_copy(line.substr(0, colonPos));
	ret.value = boost::trim_copy(line.substr(colonPos+1));
	return ret;
}

} // namespace File

} // namespace TerraNova
