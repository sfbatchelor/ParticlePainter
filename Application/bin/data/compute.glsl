#version 440


struct Point{
	vec3 pos;
	vec4 col;
	vec3 vel;
	vec3 accel;
};

const float EPSILON = 0.0001;

layout(std140, binding=0) buffer points{
    Point p[];
};

layout(rgba8, binding=0) uniform readonly image2D src;

uniform int u_numPoints = 0;
uniform ivec2 u_pixSampleSize = ivec2(100,100);

uniform float u_pixAttractMax = 100.;
uniform float u_pixRepulseMax = 100.;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

float map(float value, float min1, float max1, float min2, float max2)
{
	// Convert the current value to a percentage
	// 0% - min1, 100% - max1
	float perc = (value - min1) / (max1 - min1);

	// Do the same operation backwards with min2 and max2
	return  perc * (max2 - min2) + min2;

}

void main(){

	Point point = p[gl_GlobalInvocationID.x * gl_GlobalInvocationID.y];

	// Lookup Pixels from underlying image
	ivec2 lookup = ivec2(point.pos.xy);
	int xMin = lookup.x - int(u_pixSampleSize.x/2);
	int xMax = lookup.x + int(u_pixSampleSize.x/2);
	int yMin = lookup.y - int(u_pixSampleSize.y/2);
	int yMax = lookup.y + int(u_pixSampleSize.y/2);
	for (int x = xMin; x < xMax; x++)
	{
		for (int y = yMin; y < yMax; y++)
		{
			// get colour at point, work out difference, remap to 0-1, use as an attraction force
			vec4 imageCol = imageLoad(src, ivec2(x,y));
			float imageDiff = distance(imageCol.rgb, point.col.rgb);
			float coeff = map(imageDiff, 0, 200, 0, 1);

			float pixAttractMag = u_pixAttractMax * coeff;
			vec3 pixAttractDir = vec3(point.pos.xy - vec2(x,y), 0.0); //no depth component atm
			point.accel += pixAttractDir * pixAttractMag;
		}
	}


	point.vel += point.accel;
	point.pos += point.vel;

}