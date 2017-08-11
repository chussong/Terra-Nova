#ifndef FILE_HPP
#define FILE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <SDL.h>

namespace File {
	void Initialize();

	std::vector<std::string> Read(const std::string& relativePath);
	std::vector<std::string> GetSection(const std::vector<std::string>& source,
			const std::string& sectionName);
}

#endif
