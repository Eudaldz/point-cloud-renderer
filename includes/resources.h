#pragma once
#include <fstream>
#include <string>

namespace Resources
{
	std::string GetResourceText(const char* filename);
	std::ifstream GetResourceStream(const char* filename);
}