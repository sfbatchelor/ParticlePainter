#version 440


struct Point{
	vec4 pos;
	vec4 col;
	vec4 vel;
};

const float EPSILON = 0.0001;

layout(std140, binding=0) buffer newPoints{
    Point np[];
};
layout(std140, binding=1) buffer prevPoints{
    Point pp[];
};

layout(rgba8, binding=0) uniform readonly image2D src;

uniform int uNumPoints = 0;
uniform int uWidth = 1000;
uniform int uHeight = 1000;
uniform ivec2 uPixSampleSize = ivec2(10);

uniform float uMaxCohDist = 5;
uniform float uMinCohDist = 0;
float uCohDistRange = uMaxCohDist - uMinCohDist;
uniform float uMaxCohAccel = 2;

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

void main(){

// SETUP
	Point point = pp[gl_GlobalInvocationID.x];

	//accel value to add to vel, vel to add to pos
	vec3 totalAccel = vec3(0);

	// Lookup Pixels from underlying image
	ivec2 lookup = ivec2(point.pos.xy);
	int xMin = lookup.x - int(uPixSampleSize.x/2.);
	int xMax = lookup.x + int(uPixSampleSize.x/2.);
	int yMin = lookup.y - int(uPixSampleSize.y/2.);
	int yMax = lookup.y + int(uPixSampleSize.y/2.);


// COLOUR COHESION
	// 1- find the closest matching pixel colour in an area, set that pixel pos to be the new 'target'
	float closestColL = 100000.;
	vec3 cohesionTarget = point.pos.xyz;
	for (int x = xMin; x < xMax; x++)
	{
		if (x < 0 || x > uWidth)
			continue;
		for (int y = yMin; y < yMax; y++)
		{
			if (y < 0 || y > uHeight)
				continue;

			vec4 imageCol = imageLoad(src, ivec2(x,y));
			float imageDiff = distance(imageCol.rgb, point.col.rgb);
			if(imageDiff < closestColL)
			{
				cohesionTarget = vec3(x,y,point.pos.z);//using same depth for now
				closestColL = imageDiff;
			}
		}
	}
	// 2- 'Target' - point pos, normalize for direction
	vec3 toTarget = cohesionTarget - point.pos.xyz;
	toTarget.z = 0;
	vec3 toTargetDir = normalize(toTarget);
	// 3 - Get distance to centre (float), clamp to min and max values (0 to 2). 
	//     Calc accel strength = dist2Centre/distRange. So strength is faster the further away the point is
	float toTargetDist = length(toTarget);
	toTargetDist = clamp(toTargetDist, uMinCohDist, uMaxCohDist);
	float cohAccelMag =  (toTargetDist/ uCohDistRange) * uMaxCohAccel;
	// 4 - Add dir*accel strength to accel;
	totalAccel += toTargetDir * cohAccelMag;




// INTEGRATION 
	// 1 - Velocity += Accel
	point.pos.xyz += totalAccel;
	// 2 - Pos += velocity
	point.pos += point.vel;
	// 3 - accel = 0
	// 4- Boundary check
	if( point.pos.y < 0 || point.pos.y > uHeight || point.pos.x < 0 || point.pos.x > uWidth)
	{
		point.vel *= -1.;
	}

// WRITE OUT
	np[gl_GlobalInvocationID.x ] = point;
}