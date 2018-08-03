#version 440


struct Point{
	vec4 pos;
	vec4 col;
	vec4 vel;
};

const float EPSILON = 0.0001;

layout(std140, binding=0) buffer points{
    Point p[];
};

layout(rgba8, binding=0) uniform readonly image2D src;

uniform int u_numPoints = 0;
uniform int u_width = 1000;
uniform int u_height = 1000;
uniform ivec2 u_pixSampleSize = ivec2(200,200);

uniform float u_pixAttractMax = .001;
uniform float u_pixRepulseMax = .01;

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

float map(float value, float min1, float max1, float min2, float max2)
{
	// Convert the current value to a percentage
	// 0% - min1, 100% - max1
	float perc = (value - min1) / (max1 - min1);

	// Do the same operation backwards with min2 and max2
	return  perc * (max2 - min2) + min2;

}

void main(){

	Point point = p[gl_GlobalInvocationID.x];

	// Lookup Pixels from underlying image
	ivec2 lookup = ivec2(point.pos.xy);
	int xMin = lookup.x - int(u_pixSampleSize.x/2.);
	int xMax = lookup.x + int(u_pixSampleSize.x/2.);
	int yMin = lookup.y - int(u_pixSampleSize.y/2.);
	int yMax = lookup.y + int(u_pixSampleSize.y/2.);
	for (int x = xMin; x < xMax; x++)
	{
//		if (x < 0 || x > u_width)
//			continue;
		for (int y = yMin; y < yMax; y++)
		{
//			if (y < 0 || y > u_height)
//				continue;
			if(x == point.pos.x && y == point.pos.y)
				continue;

			// get colour at point, work out difference, remap to 0-1, use as an attraction force
			vec4 imageCol = imageLoad(src, ivec2(x,y));
			float imageDiff = distance(imageCol.rgb, point.col.rgb);
			float coeff = smoothstep( 200, 0, imageDiff);

			// colour attraction component
			float pixAttractMag = u_pixAttractMax * coeff;
			vec2 dir =  point.pos.xy - vec2(x,y)  ;
		//	vec3 pixAttractDir = (vec3(normalize(), 0.0)); //no depth component atm
			vec3 pixAttractDir = vec3(dir,0);//no depth component atm

			float dcoeff =  length(point.pos.xy - vec2(x,y));
			dcoeff = smoothstep(100, 0, dcoeff);

			point.vel.xyz += pixAttractDir * pixAttractMag * dcoeff;

//			// colour repulsion component
//			coeff = smoothstep( 200, 0, imageDiff);
//			float pixRepulseMag = u_pixRepulseMax * coeff;
//			vec3 pixRepulseDir = vec3((point.pos.xy - vec2(x,y)), 0.0); //no depth component atm
//			dcoeff =  length(point.pos.xy - vec2(x,y));
//			dcoeff = smoothstep(10, 0, dcoeff);
//			point.vel.xyz += pixRepulseDir * pixRepulseMag * dcoeff;
		}
	}


//	//boundary checks
//	if( point.pos.y < 0 || point.pos.y > u_height )
//	{
//		point.vel *= -1.;
//	}


	point.pos += point.vel;

	p[gl_GlobalInvocationID.x ] = point;
}