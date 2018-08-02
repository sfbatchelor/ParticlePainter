#version 440

struct Ray{
    ivec2 id;
	vec4 origin;
	vec3 dir;
};

struct Point{
	vec3 pos;
	vec4 col;
};

const float EPSILON = 0.0001;

layout(std140, binding=0) buffer rays{
    Ray r[];
};

layout(std140, binding=1) buffer points{
    Point p[];
};

layout(rgba32f, binding=0) uniform image2D dst;


uniform int u_numPoints = 0;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

float sdSphere(in vec3 pos, in vec3 centre, in float radius )
{

	return length(pos - centre) - radius;

}

float distMap(in vec3 pos, out vec4 col)
{
	float d = 0;
	for( int i = 0; i< u_numPoints; i++)
	{
		if(d > sdSphere(pos, p[i].pos, 150))
		{
			col = p[i].col;
			d = sdSphere(pos, p[i].pos, 150);
		}
	}
	return d;
}


float worldIntersect(in vec3 ro, in vec3 rd, out vec4 col)
{
	float tmax = 100000.;
	for (float t = .01; t <tmax;)
	{
		float h = distMap(ro + rd*vec3(t), col);
		if(h<.001)
		{
			return t;
		}
		t += h.x;
	}
	col = vec4(0,0,0,1);
	return float(-1.);
}

void main(){

	Ray ray = r[gl_GlobalInvocationID.x * gl_GlobalInvocationID.y];

	vec4 col = vec4(0,0,0,1);
	worldIntersect(ray.origin.xyz, ray.dir, col);
	col = p[gl_GlobalInvocationID.x * gl_GlobalInvocationID.y].col;

	imageStore( dst, ray.id.yx, vec4(col)); 

}