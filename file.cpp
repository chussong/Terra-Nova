#include "file.hpp"

namespace File {

namespace {
	fs::path basePath;
} // anonymous namespace

void Initialize(){
	basePath = fs::path(SDL_GetBasePath());
}

const fs::path& BasePath() {
	return basePath;
}

std::string AbsolutePath(const std::string& relativePath) {
	return (basePath / relativePath).native();
}

std::vector<std::string> ReadFromFullPath(const fs::path& fullPath){
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
		if(line.length() > 0 && line[0] != '%') ret.push_back(line);
	}while((in.rdstate() & std::ifstream::eofbit) == 0);

	//std::cout << "Read the following file:" << std::endl;
	//for(auto& ln : ret) std::cout << ln << std::endl;

	return ret;
}

std::vector<std::string> Read(const std::string& relativePath){
	return ReadFromFullPath(basePath.append(relativePath));
}

std::vector<std::vector<std::string>> ReadAll(const std::string& relativePath) {
	fs::path path = basePath.append(relativePath);
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
	std::cerr << "Warning: asked to get the section " << sectionName << " from "
		<< "a file but could not find it." << std::endl;
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

} // File namespace
