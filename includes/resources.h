#pragma once

struct ShaderCode
{
	const char* vertex;
	const char* fragment;
};

enum ShaderName
{
	Point
};

enum DataName
{
	Demo1
};

namespace Resource
{
	
	ShaderCode getShaderCode(ShaderName sn);
	const char* getPCData(DataName dn);
}