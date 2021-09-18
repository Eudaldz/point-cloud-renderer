#include "resources.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

string Resources::GetResourceText(const char* filename)
{
	string path = "resources/" + string(filename);
	ifstream ifs(path);
	stringstream ss;
	ss << ifs.rdbuf();
	return ss.str();
}

ifstream Resources::GetResourceStream(const char* filename)
{
	string path = "resources/" + string(filename);
	return ifstream(path);
}