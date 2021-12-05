#include "file_reader.h"
#include "point.h"
#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

namespace {
	enum class DataFormat {
		ASCII, BINARY, UNKOWN
	};

	enum class Field {
		X, Y, Z, R, G, B, NX, NY, NZ, RGB, RGBA, UNKOWN, LABEL, PART
	};

	enum class FieldFormat {
		FLOAT, INT, UINT
	};

	enum class FieldSize {
		B8, B32, B64 
	};

	struct PointDataFormat {
		uint32_t point_count;
		DataFormat dataFormat;
		uint32_t fieldN;
		Field* fields;
		FieldFormat* formats;
		FieldSize* sizes;
	};
	
	struct PointAsciiFormat {
		uint32_t fieldN;
		Field* fields;
		FieldFormat* formats;
	};

	struct PointBinaryFormat {
		uint32_t fieldN;
		Field* fields;
		FieldFormat* formats;
		FieldSize* sizes;
	};

	int readNextAsciiPoint(istream& is, const PointAsciiFormat& format, Point& pointOut) {
		vec3 pos(0,0,0);
		vec4 color(0,0,0,0);
		vec3 normal(0,0,0);
		
		for (uint32_t i = 0; i < format.fieldN; i++) {
			Field f = format.fields[i];
			FieldFormat form = format.formats[i];
			if (!is.good())return -1;
			if (f == Field::X || f == Field::Y || f == Field::Z || f == Field::NX || f == Field::NY || f == Field::NZ) {
				float value = 0;
				if (form == FieldFormat::FLOAT) {
					is >> value;
				}
				else if (form == FieldFormat::INT) {
					int32_t v;
					is >> v;
					value = (float)v;
				}
				else if (form == FieldFormat::UINT) {
					uint32_t v;
					is >> v;
					value = (float)v;
				}
				switch (f) {
				case Field::X:
					pos.x = value;
					break;
				case Field::Y:
					pos.y = value;
					break;
				case Field::Z:
					pos.z = value;
					break;
				case Field::NX:
					normal.x = value;
					break;
				case Field::NY:
					normal.y = value;
					break;
				case Field::NZ:
					normal.z = value;
					break;
				}
			}
			else if(f == Field::R || f == Field::G || f == Field::B){
				float value = 0;
				if (form == FieldFormat::FLOAT) {
					is >> value;
				}
				else if (form == FieldFormat::UINT) {
					uint32_t c;
					is >> c;
					value = (float)c / 255.0f;
				}
				switch (f) {
				case Field::R:
					color.r = value;
					break;
				case Field::G:
					color.g = value;
					break;
				case Field::B:
					color.b = value;
					break;
				}
			}
			else if (f == Field::RGB || f == Field::RGBA) {
				if (form == FieldFormat::FLOAT) {
					float value;
					is >> value;
					uint32_t rgb = *(uint32_t*)&value;
					uint8_t a = (rgb >> 24) & 0x0000ff;
					uint8_t r = (rgb >> 16) & 0x0000ff;
					uint8_t g = (rgb >> 8) & 0x0000ff;
					uint8_t b = (rgb) & 0x0000ff;
					color.r = (float)r / 255.0f;
					color.g = (float)g / 255.0f;
					color.b = (float)b / 255.0f;
					color.a = f == Field::RGBA ? (float)a / 255.0f : 1.0f;

				}
				else if (form == FieldFormat::UINT) {
					uint32_t rgb;
					is >> rgb;
					uint8_t a = (rgb >> 24) & 0x0000ff;
					uint8_t r = (rgb >> 16) & 0x0000ff;
					uint8_t g = (rgb >> 8) & 0x0000ff;
					uint8_t b = (rgb) & 0x0000ff;
					color.r = (float)r / 255.0f;
					color.g = (float)g / 255.0f;
					color.b = (float)b / 255.0f;
					color.a = f == Field::RGBA ? (float)a / 255.0f : 1.0f;
				}
			}
			else {
				string param;
				is >> param;
			}
		}
		pointOut.position = pos;
		pointOut.color = color;
		pointOut.normal = normal;
		return 0;
	}

	int readNextBinaryPoint(istream& is, const PointBinaryFormat& format, Point& pointOut) {
		vec3 pos(0, 0, 0);
		vec4 color(0, 0, 0, 0);
		vec3 normal(0, 0, 0);
		uint32_t label = 0;
		uint32_t part = 0;
		for (uint32_t i = 0; i < format.fieldN; i++) {
			Field f = format.fields[i];
			FieldFormat form = format.formats[i];
			FieldSize size = format.sizes[i];
			if (!is.good())return -1;

			if (f == Field::X || f == Field::Y || f == Field::Z || f == Field::NX || f == Field::NY || f == Field::NZ || f == Field::R || f == Field::G || f == Field::B) {
				float value = 0;
				uint64_t bytes = 0;
				uint32_t bsize = 1;
				if (size == FieldSize::B32)bsize = 4;
				else if (size == FieldSize::B64)bsize = 8;
				for (uint32_t bi = 0; bi < bsize; bi++) {
					bytes = bytes*256 + (uint64_t)is.get();
				}
				if (form == FieldFormat::FLOAT) {
					if (size == FieldSize::B32) {
						value = *(float*)&bytes;
					}
					else if (size == FieldSize::B64) {
						value = (float)(*(double*)&bytes);
					}
					
				}
				else if (form == FieldFormat::INT) {
					if (size == FieldSize::B8) {
						value = (float)(*(int8_t*)&bytes);
					}
					else if (size == FieldSize::B32) {
						value = (float)(*(int32_t*)&bytes);
					}
					else {
						value = (float)(*(int64_t*)&bytes);
					}
				}
				else if (form == FieldFormat::UINT) {
					if (size == FieldSize::B8) {
						value = (float)(*(uint8_t*)&bytes);
					}
					else if (size == FieldSize::B32) {
						value = (float)(*(uint32_t*)&bytes);
					}
					else {
						value = (float)(*(uint64_t*)&bytes);
					}
				}
				switch (f) {
				case Field::X:
					pos.x = value;
					break;
				case Field::Y:
					pos.y = value;
					break;
				case Field::Z:
					pos.z = value;
					break;
				case Field::NX:
					normal.x = value;
					break;
				case Field::NY:
					normal.y = value;
					break;
				case Field::NZ:
					normal.z = value;
					break;
				case Field::R:
					color.r = value;
					break;
				case Field::G:
					color.g = value;
					break;
				case Field::B:
					color.b = value;
					break;
				}
			}
			else if ((f == Field::RGB || f == Field::RGBA) && size == FieldSize::B32) {
				float r, g, b, a;
				color.r = (float)is.get() / 255.0f;
				color.g = (float)is.get() / 255.0f;
				color.b = (float)is.get() / 255.0f;
				color.a = (float)is.get() / 255.0f;
				if (f == Field::RGB)color.a = 1.0f;
			}
			else if (f == Field::LABEL || f == Field::PART) {
				uint64_t bytes = 0;
				uint32_t bsize = 1;
				if (size == FieldSize::B32)bsize = 4;
				else if (size == FieldSize::B64)bsize = 8;
				for (uint32_t bi = 0; bi < bsize; bi++) {
					bytes = bytes * 256 + (uint64_t)is.get();
				}
				double dvalue = *(double*)&bytes;
				switch (f) {
				case Field::LABEL:
					label = (uint32_t)dvalue;
					break;
				case Field::PART:
					part = (uint32_t)dvalue;
					break;
				}
			}
			else {
				uint32_t bsize = 1;
				if (size == FieldSize::B32)bsize = 4;
				else if (size == FieldSize::B64)bsize = 8;
				for (uint32_t bi = 0; bi < bsize; bi++) {
					is.get();
				}
			}
		}
		pointOut.position = pos;
		pointOut.color = color;
		pointOut.normal = normal;
		pointOut.label = label;
		pointOut.part = part;
		return 0;
	}

	int readASCIIPointData(istream& is, uint32_t point_count, PointAsciiFormat& format, vector<Point>& points) {
		uint32_t i = 0;
		while (i < point_count && is.good()) {
			Point p;
			int e = readNextAsciiPoint(is, format, p);
			if (e != -1) {
				points.push_back(p);
				++i;
			}
		}
		if (i < point_count)return -1;
		return 0;
	}

	int readBinaryPointData(istream& is, uint32_t point_count, PointBinaryFormat& format, vector<Point>& points) {
		uint32_t i = 0;
		while (i < point_count && !is.fail()) {
			Point p;
			int e = readNextBinaryPoint(is, format, p);
			if (e == -1) {
				cout << "ERROR: problem reading binary points" << endl;
				return -1;
			}
			points.push_back(p);
			++i;
		}
		if (i < point_count) {
			return -1;
		}
		return 0;
	}

	int readDataFormat(istream& is, PointDataFormat& formatOut) {
		string line;
		string param;

		vector<Field> fields;
		vector<FieldSize> sizes;
		vector<FieldFormat> formats;
		uint32_t pn = 0;
		DataFormat dataFormat = DataFormat::UNKOWN;

		bool endOfHeader = false;

		while (!endOfHeader && getline(is, line)) {
			istringstream ls(line);
			ls >> param;
			if (param == "element") {
				ls >> param;
				ls >> pn;
			}
			else if (param == "format") {
				ls >> param;
				if (param == "ascii") {
					dataFormat = DataFormat::ASCII;
				}
				else if (param == "binary_big_endian") {
					dataFormat = DataFormat::BINARY;
				}
				else {
					cout << "ERROR: Incompatible data format!" << endl;
					return -1;
				}
			}
			else if (param == "property") {
				ls >> param;
				if (param == "double") {
					sizes.push_back(FieldSize::B64);
					formats.push_back(FieldFormat::FLOAT);
				}
				else if (param == "float") {
					sizes.push_back(FieldSize::B32);
					formats.push_back(FieldFormat::FLOAT);
				}
				else if (param == "uchar") {
					sizes.push_back(FieldSize::B8);
					formats.push_back(FieldFormat::UINT);
				}
				else {
					cout << "ERROR: Incompatible property data type!" << endl;
					return -1;
				}
				ls >> param;
				if (param == "x") {
					fields.push_back(Field::X);
				}
				else if (param == "y") {
					fields.push_back(Field::Y);
				}
				else if (param == "z") {
					fields.push_back(Field::Z);
				}
				else if (param == "l") {
					fields.push_back(Field::LABEL);
				}
				else if (param == "p") {
					fields.push_back(Field::PART);
				}
				else if (param == "red") {
					fields.push_back(Field::R);
				}
				else if (param == "green") {
					fields.push_back(Field::G);
				}
				else if (param == "blue") {
					fields.push_back(Field::B);
				}
				else if (param == "nx") {
					fields.push_back(Field::NX);
				}
				else if (param == "ny") {
					fields.push_back(Field::NY);
				}
				else if (param == "nz") {
					fields.push_back(Field::NZ);
				}
				else {
					fields.push_back(Field::UNKOWN);
				}
			}
			else if (param == "end_header") {
				endOfHeader = true;
			}
		}
		if (endOfHeader) {
			formatOut.point_count = pn;
			formatOut.fieldN = fields.size();
			formatOut.fields = new Field[fields.size()];
			formatOut.formats = new FieldFormat[formats.size()];
			formatOut.sizes = new FieldSize[sizes.size()];
			formatOut.dataFormat = dataFormat;
			std::copy(fields.begin(), fields.end(), formatOut.fields);
			std::copy(formats.begin(), formats.end(), formatOut.formats);
			std::copy(sizes.begin(), sizes.end(), formatOut.sizes);
		}
		else {
			return -1;
		}
	}
}

int Reader::ReadPointCloud(const string& filename, const Reader::Options& params, vector<Point>& points)
{
	ifstream ifs(filename, std::ifstream::binary);
	points.clear();
	if (ifs) {
		PointDataFormat pdf;
		if (readDataFormat(ifs, pdf) != -1) {
			points.reserve(pdf.point_count);
			if (pdf.dataFormat == DataFormat::ASCII) {
				PointAsciiFormat paf;
				paf.fieldN = pdf.fieldN;
				paf.fields = pdf.fields;
				paf.formats = pdf.formats;
				int e = readASCIIPointData(ifs, pdf.point_count, paf, points);
				if (e == -1)cout << "INCOMPLETE POINT DATA" << endl;
			}
			else if (pdf.dataFormat == DataFormat::BINARY) {
				PointBinaryFormat pbf;
				pbf.fieldN = pdf.fieldN;
				pbf.fields = pdf.fields;
				pbf.formats = pdf.formats;
				pbf.sizes = pdf.sizes;
				int e = readBinaryPointData(ifs, pdf.point_count, pbf, points);
				if (e == -1)cout << "INCOMPLETE POINT DATA" << endl;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	else {
		cout << "ERROR: Point cloud file does not exist" << endl;
		return -1;
	}
	return 0;
}