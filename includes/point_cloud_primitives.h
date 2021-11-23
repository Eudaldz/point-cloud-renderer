#pragma once
#include "point_cloud.h"

namespace PCPrimitives
{
	constexpr uint32_t MAX_SQUARE_SAMPLE_RES = 65535;
	constexpr uint32_t MAX_CUBE_SAMPLE_RES = 1625;
	PointCloud* star(uint32_t sampleRes);
	PointCloud* star_noisy(uint32_t sampleRes);
	PointCloud* slice(uint32_t sampleRes);
	PointCloud* slice_noisy(uint32_t sampleRes);
	PointCloud* slice_transparent(uint32_t sampleRes);
	PointCloud* cube_opaque(uint32_t sampleRes);
	PointCloud* cube_transparent(uint32_t sampleRes);
	PointCloud* sphere_opaque(uint32_t sampleRes);
	PointCloud* sphere_transparent(uint32_t sampleRes);
	PointCloud* menger_cube(uint32_t sampleRes);
	PointCloud* menger_cube_transparent(uint32_t sampleRes);
}