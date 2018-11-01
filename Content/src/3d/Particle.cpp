#include "stdafx.h"
#include "3d\Particle.h"

std::vector<GpuParticle> GpuParticleFactory::fromImage(const ofImage & image, const int & numPoints, const float &minDepth, const float & maxDepth, const float & minBrightness, const float & maxBrightness)
{


	std::vector<GpuParticle> output;
	if (!image.isAllocated())
	{
		ofLogError() << "Image not allocated";
	}
	else
	{
		for (int i = 0; i < numPoints; i++)
		{
			int x = ofRandom(float(image.getWidth()));
			int y = ofRandom(float(image.getHeight()));

			ofColor cur = image.getColor(x, y);
			if (cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.getBrightness(), minBrightness, maxBrightness, minDepth, maxDepth);
				cur.a = 255;
				ofVec3f pos(x, y, z);
				GpuParticle point{};
				point.m_col = ofFloatColor(cur.r, cur.g, cur.b, cur.a);
				point.m_pos = ofVec4f(pos.x, pos.y, pos.z, 1.);
				point.m_vel = ofVec4f(0);
				output.push_back(point);
			}
		}
	}
	return output;
}
