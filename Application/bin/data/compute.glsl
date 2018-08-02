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

bool solveQuadratic(in float a, in float b, in float c, out float x0, out float x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = - 0.5 * b / a;
	else {
	float q = (b > 0) ?
	-0.5 * (b + sqrt(discr)) :
	-0.5 * (b - sqrt(discr));
	x0 = q / a;
	x1 = c / q;
	}

	return true;
} 


float sdSphere(in vec3 pos, in vec3 centre, in float radius )
{

	return length(pos - centre) - radius;

}

float distMap(in vec3 pos, out vec3 col)
{
	float d = 0;
	for( int i = 0; i< u_numPoints; i++)
	{
		if(d > sdSphere(pos, p[i].pos, 500))
		{
			col = p[i].col.xyz;
			d = sdSphere(pos, p[i].pos, 500);
		}
	}
	return d;
}


float worldIntersect(in vec3 ro, in vec3 rd, out vec3 col)
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
	col = vec3(0);
	return float(-1.);
}

void main(){

	//Ray ray = r[gl_WorkGroupID.x* gl_WorkGroupID.y];



	Ray ray = r[gl_GlobalInvocationID.x * gl_GlobalInvocationID.y];

	vec3 col = vec3(0);
	worldIntersect(ray.origin.xyz, ray.dir, col);

	imageStore( dst, ray.id.yx, vec4(col/255., 1.)); 

}