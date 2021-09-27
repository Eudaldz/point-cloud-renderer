#pragma once

class Test {
public:
	virtual void Run() = 0;
	static Test* NewTest(const char* id);
};