#include <iostream>
#include "app_manager.h"



int process_input(char* input);
void process_pointcloud_command(char* arguments);
void process_demo_command(char* arguments);
Render parse_render_str(const char* str);
void process_help_command(char* arguments);



const int MAX_INPUT_LEN = 256;

const char* CMD_POINTCLOUD = "pointcloud";
const char* CMD_DEMO = "demo";
const char* CMD_EXIT = "exit";
const char* CMD_HELP = "help";

const char* WELCOME_TEXT = "MyPCV point cloud visualizer\nEudald Elias Vilanova\n"
						   "Type \"help\" for usage instructions.\nType \"exit\" to close\n";

//TODO help text
const char* HELP_TEXT = "<TODO: help text>\n";
//TODO open manual text
const char* HELP_POINTCLOUD_TEXT = "<TODO: pointcloud info text>\n";
//TODO config manual text
const char* HELP_DEMO_TEXT = "<TODO: demo info text>\n";

const char* UNKOWN_COMMAND_TEXT = "Unkown command\n";
const char* WRONG_ARGUMENTS_TEXT = "Bad arguments\n";

AppManager appManager;

int main(){
	std::cout << WELCOME_TEXT;
	char input[MAX_INPUT_LEN];
	int result = 1;
	while (result != 0) {
		std::cout << "\nMyPCV>> ";
		std::cin.getline(input, MAX_INPUT_LEN);
		result = process_input(input);
	}
}

int process_input(char* input) {
	char *token = strtok(input, " "); 
	if (token) {
		if (strcmp(token, CMD_EXIT) == 0) {
			return 0;
		}

		if (strcmp(token, CMD_POINTCLOUD) == 0) {
			process_pointcloud_command(strtok(NULL, ""));
		}
		else if (strcmp(token, CMD_DEMO) == 0) {
			process_demo_command(strtok(NULL, ""));
		}
		else if (strcmp(token, CMD_HELP) == 0) {
			process_help_command(strtok(NULL, ""));
		}
		else {
			std::cout << UNKOWN_COMMAND_TEXT;
		}
	}
	return 1;
}

void process_pointcloud_command(char* arguments) {
	Render render = Render::Default;
	const char* id_1 = NULL;
	const char* id_2 = NULL;
	const char* render_str = NULL;
	for (char* token = strtok(arguments, " "); token != NULL; token = strtok(NULL, " ")) {
		if (strcmp(token, "-r") == 0) render_str = strtok(NULL, " ");
		else if (!id_1) id_1 = token;
		else if (!id_2) id_2 = token;
		else {
			std::cout << WRONG_ARGUMENTS_TEXT;
			return;
		}
	}
	if (!id_1) {
		std::cout << WRONG_ARGUMENTS_TEXT;
		return;
	}
	if (render_str) {
		render = parse_render_str(render_str);
	}
	if (!id_2)appManager.OpenPointcloud(id_1, render);
	else appManager.OpenPointcloudDual(id_1, id_2, render);
}

void process_demo_command(char* arguments)
{
	const char* id = strtok(arguments, " ");
	if (!id) {
		std::cout << WRONG_ARGUMENTS_TEXT;
		return;
	}
	char* next = strtok(NULL, " ");
	if (next) {
		std::cout << WRONG_ARGUMENTS_TEXT;
		return;
	}
	appManager.OpenDemo(id);
}

Render parse_render_str(const char* str) {
	//TODO: parse render id string
	return Render::Default;
}

void process_help_command(char* arguments) {
	char* arg = strtok(arguments, " ");
	std::cout << "\n";
	if (!arg) {
		std::cout << HELP_TEXT;
		return;
	}
	char* next = strtok(NULL, " ");
	if (next) {
		std::cout << WRONG_ARGUMENTS_TEXT;
		return;
	}

	if (strcmp(arg, CMD_POINTCLOUD) == 0) {
		std::cout << HELP_POINTCLOUD_TEXT;
	}
	else if (strcmp(arg, CMD_DEMO) == 0) {
		std::cout << HELP_DEMO_TEXT;
	}
	else if (strcmp(arg, CMD_HELP) == 0) {
		std::cout << "Type \"help\" for usage instructions.";
	}
	else if (strcmp(arg, CMD_EXIT) == 0) {
		std::cout << "Type \"exit\" to close";
	}
}