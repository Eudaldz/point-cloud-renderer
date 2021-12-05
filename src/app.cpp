#include <iostream>
#include <string>

#include "renderer.h"
#include "point_cloud.h"
#include "file_reader.h"

using namespace std;

constexpr char* DEFAULT_POINTCLOUD = "";
constexpr char* DEFAULT_RENDERER = "";
constexpr char* DEFAULT_COLOR_SHADE = "";
constexpr char* DEFAULT_PSIZE_FUNC = "";

constexpr char* USAGE_STRING = "";

constexpr int DEFAULT_VIEWPORTX = 1280;
constexpr int DEFAULT_VIEWPORTY = 720;

enum class ParamaterValue {
	RENDER, COLOR_SHADE, POINT_SIZE, 
	VIEWPORT_X, VIEWPORT_Y,
	NONE
};

int main(int argc, char* argv[]) {
	string pointCloudStr = DEFAULT_POINTCLOUD;
	string renderStr = DEFAULT_RENDERER;
	string colorShadeStr = DEFAULT_COLOR_SHADE;
	string psizeFuncStr = DEFAULT_PSIZE_FUNC;
	bool file = false;
	
	int viewportX = DEFAULT_VIEWPORTX;
	int viewportY = DEFAULT_VIEWPORTY;

	ParamaterValue paramValue = ParamaterValue::NONE;
	string prev_param;

	for (int i = 1; i < argc; i++) {
		string param = argv[i];
		if (param[0] == '-') {
			//PARAMETER KEY
			if (paramValue != ParamaterValue::NONE) {
				cout << "Expected value for paramater: " << prev_param << endl;
				return 0;
			}
			
			if (param == "-r" || param == "--render") {
				paramValue = ParamaterValue::RENDER;
			}
			else if (param == "-p" || param == "--point-size-f") {
				paramValue = ParamaterValue::POINT_SIZE;
			}
			else if (param == "-c" || param == "--color") {
				paramValue = ParamaterValue::COLOR_SHADE;
			}
			else if (param == "-v" || param == "--viewport") {
				paramValue = ParamaterValue::VIEWPORT_X;
			}
			else if (param == "-f" || param == "--file") {
				file = true;
			}
			
			else if (param == "-h" || param == "--help") {
				cout << USAGE_STRING << endl;
				return 0;
			}
			else {
				cout << "Urecongnized paramater: " << param << endl;
				return 0;
			}
			prev_param = param;
		}
		else {
			//PARAMETER VALUE
			switch (paramValue) {
			case ParamaterValue::RENDER:
				renderStr = param;
				break;
			case ParamaterValue::COLOR_SHADE:
				colorShadeStr = param;
				break;
			case ParamaterValue::POINT_SIZE:
				psizeFuncStr = param;
				break;
			case ParamaterValue::VIEWPORT_X:
				paramValue = ParamaterValue::VIEWPORT_Y;
				try {
					viewportX = std::stoi(param);
				}
				catch (std::invalid_argument e) {
					cout << "Paramater -v expects two integers" << endl;
					return 0;
				}
				break;
			case ParamaterValue::VIEWPORT_Y:
				try {
					viewportY = std::stoi(param);
				}
				catch (std::invalid_argument e) {
					cout << "Paramater -v expects two integers" << endl;
					return 0;
				}
				break;
			case ParamaterValue::NONE:
				pointCloudStr = param;
				break;
			}
		}
	}

	if (file) {
		vector<Point> points;
		Reader::Options options;
		if (Reader::ReadPointCloud(pointCloudStr, options, points) == -1) {
			return 0;
		}
	}
	else {

	}

}