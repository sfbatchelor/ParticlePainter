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

uniform float uTime = 1.0;
uniform int uNumPoints = 0;
uniform int uWidth = 1000;
uniform int uHeight = 1000;
uniform int uDepth = 1000;
uniform ivec2 uPixSampleSize = ivec2(50);

uniform float uMaxCohDist = 5;
uniform float uMinCohDist = 0;
float uCohDistRange = uMaxCohDist - uMinCohDist;
uniform float uMaxCohAccel = .01;


uniform float uNearAlignDist = 1.;
uniform float uFarAlignDist = 50.;
uniform float uAlignSpeedMax = 50;
uniform float uAlignSpeedMag = .0000001;

uniform float uNearSepDist = 0.;
uniform float uFarSepDist = 50000.;
uniform float uSepMag = .00000001;

// in 2d on the x-y plane, assuming a & b are normalized
// returns -1 if heading is left, +1 if right

int leftOrRight(vec3 a, vec3 b)
{
	vec3 cp = normalize(cross(a, b));
	if(cp.y < 0.0 )
		return 1;
	else 
		return -1;
}

float map(float value, float min1, float max1, float min2, float max2)
{
	// Convert the current value to a percentage
	// 0% - min1, 100% - max1
	float perc = (value - min1) / (max1 - min1);
	perc = clamp(perc, 0.0, 1.0);
	
	// Do the same operation backwards with min2 and max2
	return  perc * (max2 - min2) + min2;

}

vec3 safeNormalize(vec3 vec) 
{
	if(length(vec) == 0 ) 
		return vec3(0);
	return normalize(vec);
}


layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

void main(){

// SETUP
	// point and some precomputed info
	Point point = pp[gl_GlobalInvocationID.x];
	vec3 pDir = vec3(0);
	vec3 correctionRt = vec3(0);
	if(length(point.vel.xyz) != 0)
	{
		vec3 pDir = safeNormalize(point.vel.xyz);
		vec3 correctionRt = safeNormalize(cross(pDir, vec3(0,0,1)));
	}

	// values for particle sampling
	uint m = uint(1024.0*14.0*uTime);
	uint start = uint(mod(m, 1024*14-512));
	uint end = start + 512;



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
	vec3 toTargetDir = safeNormalize(toTarget);
	// 3 - Get distance to centre (float), clamp to min and max values (0 to 2). 
	//     Calc accel strength = dist2Centre/distRange. So strength is faster the further away the point is
	float toTargetDist = length(toTarget);
	toTargetDist = clamp(toTargetDist, uMinCohDist, uMaxCohDist);
	float cohAccelMag =  (toTargetDist/ uCohDistRange) * uMaxCohAccel;
	// 4 - Add dir*accel strength to accel;
	totalAccel += toTargetDir * cohAccelMag;



// POINT LOOKUP BEHAVIOURS: FUZZY-ALLIGNMENT PARTICLES & SEPERATION 
	vec3 fuzzySpeed = vec3(0);
	vec3 fuzzyHeading = vec3(0);
	vec3 seperation = vec3(0);
	for(uint i=start;i<end;i++){
		if(i!=gl_GlobalInvocationID.x){
			// significance based on distance away
			Point samplePoint = pp[i];
			vec3 distVec = samplePoint.pos.xyz - point.pos.xyz;
			float dist = length(distVec);
			float sig = 1. - map(dist, uNearAlignDist, uFarAlignDist, 0.0, 1.0);
			
			// work out heading-correction
			vec3 sDir = safeNormalize(samplePoint.vel.xyz);
			float hc = 1. - dot(pDir, sDir);
			//compute fuzzy-heading
			if(leftOrRight(pDir, sDir) == 1)
				fuzzyHeading += correctionRt *hc* sig;
			else
				fuzzyHeading -= correctionRt*hc* sig;


			//compute fuzzy-speed & speed correction
			vec3 speedDiffV = samplePoint.vel.xyz - point.vel.xyz;
			float speedDiffF = length(speedDiffV);
			float sc = map ( speedDiffF, 0, uAlignSpeedMax, 0., 1.);
			fuzzySpeed += speedDiffV * sc * sig * uAlignSpeedMag;


			float colDiff = length(samplePoint.col.rgb - point.col.rgb);
			sig = map(colDiff, 0., 300., .0, 1.0);
			float sepStrength = map(dist, uNearSepDist, uFarSepDist, uSepMag, 0);
			seperation += (-dist* sig) *sepStrength ;

		}
	}
	totalAccel += fuzzySpeed;
	totalAccel += fuzzyHeading;
	//totalAccel += seperation;
	totalAccel.z = 0.;



// FUZZY-ALLIGNMENT COLOUR 



// INTEGRATION 
	// 1 - Velocity += Accel
	point.vel.xyz += totalAccel;
	// 2- Boundary check
	if( point.pos.y < 0 || point.pos.y > uHeight || point.pos.x < 0 || point.pos.x > uWidth || point.pos.z < -uDepth || point.pos.z > uDepth)
	{
		point.vel *= -.8;

		if(point.pos.y < 0)
			point.pos.y = 0;
		if(point.pos.y > uHeight)
			point.pos.y = uHeight;
		if(point.pos.x < 0)
			point.pos.x = 0;
		if(point.pos.x > uWidth)
			point.pos.x = uWidth;

		if(point.pos.z > uDepth)
			point.pos.z = uDepth;
		if(point.pos.z < -uDepth)
			point.pos.z = -uDepth;
	}
	else 
		point.pos += point.vel;

	

// WRITE OUT
	np[gl_GlobalInvocationID.x ] = point;
}