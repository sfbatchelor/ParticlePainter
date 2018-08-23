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
uniform int uNumPointsSF = 1;
uniform float uWidth = 2000.;
uniform float uHeight = 2000.;
uniform float uDepth = 1000.;
uniform float uAccelScale = .01;
uniform vec2 uPixSampleSize = vec2(10.);// 100 sample pixels
uniform float uAlignPointSampleRadius = 5; // ~10 pointrs - very close 
uniform float uSeperationPointSampleRadius = 20; // ~100 points - surrounding area

uniform float uMaxCohDist = 100;
uniform float uMinCohDist = 0;
float uCohDistRange = uMaxCohDist - uMinCohDist;
uniform float uMaxCohAccel = 50;


uniform float uNearAlignDist = 1.;
uniform float uFarAlignDist = 10.;
uniform float uAlignSpeedDiffMax= 5.;
uniform float uAlignSpeedMag = 0.0003;

uniform float uSepFalloffDist = 10.;
uniform float uSepDistMag = .1;

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
	uint m = uint(1024.0*uNumPointsSF*uTime);
	uint start = uint(mod(m, 1024*uNumPointsSF-512));
	uint end = start + 512;

	//accel value to add to vel, vel to add to pos
	vec3 totalAccel = vec3(0);

	// Lookup Pixels from underlying image
	vec2 lookup = vec2(point.pos.xy);
	float xMin = lookup.x - (uPixSampleSize.x/2.);
	float xMax = lookup.x + (uPixSampleSize.x/2.);
	float yMin = lookup.y - (uPixSampleSize.y/2.);
	float yMax = lookup.y + (uPixSampleSize.y/2.);




// COLOUR COHESION
	// 1- find the closest matching pixel colour in an area, set that pixel pos to be the new 'target'
	float closestColL = 100000.;
	vec3 cohesionTarget = point.pos.xyz;
	for (float x = xMin; x <= xMax; x+= 1.)
	{
		for (float y = yMin; y <= yMax; y+= 1.)
		{

			vec2 tmpXY = vec2(x,y);
			if(x < lookup.x) //round up if needed - due to pixel int value lookups
				tmpXY.x = ceil(x);
			if(y < lookup.y)
				tmpXY.y = ceil(y);
			ivec2 imageXY = ivec2(tmpXY);
			vec4 imageCol = imageLoad(src, imageXY);
			float imageDiff = length(  point.col.rgb -imageCol.rgb  );
			cohesionTarget = vec3(x,y,point.pos.z);//using same depth for now

			// 2- 'Target' - point pos, normalize for direction
			vec3 toTarget = cohesionTarget - point.pos.xyz;
			toTarget.z = 0;
			vec3 toTargetDir = safeNormalize(toTarget);
			// 3 - Get distance to centre (float), clamp to min and max values (0 to 2). 
			//     Calc accel strength = dist2Centre/distRange. So strength is faster the further away the point is
			float toTargetDist = length(toTarget);
			toTargetDist = clamp(toTargetDist, uMinCohDist, uMaxCohDist);
			float cohAccelMag =  (toTargetDist/ uCohDistRange) * uMaxCohAccel;

			float colSig = 1.0 - imageDiff; //force is more the more similar they are
			
			// 4 - Add dir*accel strength to accel;
			totalAccel += toTargetDir * cohAccelMag * colSig;
		}
	}




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
			if(dist < uAlignPointSampleRadius) // only points within a certain radius
			{
				float distSig= 1. - (dist-uNearAlignDist / uFarAlignDist - uNearAlignDist);
			
				// work out heading-correction
				vec3 sDir = safeNormalize(samplePoint.vel.xyz);
				float hc = 1. - dot(pDir, sDir);
				//compute fuzzy-heading
				if(leftOrRight(pDir, sDir) == 1)
					fuzzyHeading += correctionRt *hc* distSig;
				else
					fuzzyHeading -= correctionRt*hc* distSig;


				//compute fuzzy-speed & speed correction
				vec3 speedDiffV = samplePoint.vel.xyz - point.vel.xyz;
				float speedDiffF = length(speedDiffV);
				float sc = clamp( max(speedDiffF,0.)/ uAlignSpeedDiffMax, 0,  uAlignSpeedDiffMax);
				fuzzySpeed += speedDiffV* sc * distSig * uAlignSpeedMag;
			}
			if(dist < uSeperationPointSampleRadius) // only points within a certain radius
			{

				float colDiff = length(samplePoint.col.rgb - point.col.rgb);
				float colSig = colDiff/255.; //force is less the more similar they are
				float distSig = 1. - (dist / uSepFalloffDist); // force is less the further away the point is
				distSig*= uSepDistMag;
				vec3 sepDir = -safeNormalize(distVec); // repulsive so minus the direction
				seperation += sepDir*colSig *distSig;
			}

		}
	}
	totalAccel += fuzzySpeed;
	totalAccel += fuzzyHeading;
	totalAccel += seperation;
	totalAccel.z = 0.;



// FUZZY-ALLIGNMENT COLOUR 



// INTEGRATION 
	// 1 - Velocity += Accel
	point.vel.xyz += totalAccel* uAccelScale;
	// 2- Boundary check
	if( point.pos.y < 0 || point.pos.y > uHeight || point.pos.x < 0 || point.pos.x > uWidth || point.pos.z < -uDepth || point.pos.z > uDepth)
	{
		point.vel *= -.2;

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

	

	//point.col.rg = (.7 + normalize(point.vel.xy) * .5)*255.;

// WRITE OUT
	np[gl_GlobalInvocationID.x ] = point;
}