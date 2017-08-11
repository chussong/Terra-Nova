#include "file.hpp"

namespace File {

namespace {
#ifdef _WIN32
	constexpr char PATH_SEP = '\\';
#else
	constexpr char PATH_SEP = '/';
#endif

	std::string baseDir = "";
} // anonymous namespace

void Initialize(){
	baseDir = SDL_GetBasePath();
}

std::vector<std::string> Read(const std::string& relativePath){
	std::vector<std::string> ret;

	std::ifstream in(baseDir + relativePath);
	if((in.rdstate() & std::ifstream::failbit) != 0){
		std::cerr << "Error: unable to open file at "
			<< baseDir + relativePath << ". Does it exist?" << std::endl;
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
	std::cerr << "Error: asked to get the section " << sectionName << " from a "
		<< "file but could not find it." << std::endl;
	return std::vector<std::string>();
}

} // File namespace
