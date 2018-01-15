#ifndef FILE_HPP
#define FILE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
//#include <filesystem>

#include <SDL.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>

// currently using boost::filesystem to stay on C++14; if switched to 17, can 
// use std::filesystem instead. In that case, can likely unlink boost_system
// and boost_filesystem in the Makefile.

namespace TerraNova {

namespace File {

namespace fs = boost::filesystem;
//namespace fs = std::filesystem;

typedef fs::path Path;

void Initialize();

const Path& BasePath();
std::string AbsolutePath(const std::string& relativePath);
std::string JoinPaths(const std::vector<std::string>& paths);
std::vector<std::string> ReadFromFullPath(const Path& fullPath);
std::vector<std::string> Read(const std::string& relativePath);
std::vector<std::vector<std::string>> ReadAll(const std::string& relativePath);

struct Entry { std::string name; std::string value; };
std::vector<std::string> GetSection(const std::vector<std::string>& source,
		const std::string& sectionName);
std::vector<std::string> GetField(const std::vector<std::string>& source,
		const size_t startingLine);
Entry GetEntry(const std::string& line);

template<typename Result>
std::vector<Result> ForEachFile(const Path& location, 
		Result (*function)(const Path&)) {
	std::vector<Result> ret;
	for(auto& file : fs::directory_iterator(location)) {
		ret.push_back(function(file.path()));
	}
	return ret;
}

} // namespace File

} // namespace TerraNova

#endif
