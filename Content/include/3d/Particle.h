#pragma once

struct GpuParticle
{
	ofVec4f m_pos;
	ofFloatColor m_col;
	ofVec4f m_vel;
};

namespace GpuParticleFactory
{
	//! Spawns gpu particles randomly across the width and height of the image, depth is calculated from colour brightness
	std::vector<GpuParticle> fromImage(const ofImage& image, const int& numPoints, const float& minDepth, const float& maxDepth, const float& minBrightness = 0, const float& maxBrightness = 255);


}