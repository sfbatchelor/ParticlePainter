#version 440

struct Ray{
    vec3 id;
    vec4 col;
	vec4 origin;
	vec3 dir;
};

struct Point{
	vec3 pos;
	vec4 col;
};

layout(std140, binding=0) buffer rays{
    Ray r[];
};

layout(std140, binding=1) buffer points{
    Point p[];
};

layout(rgba8, binding=0) uniform image2D dst;



layout(local_size_x = 20, local_size_y = 20, local_size_z = 3) in;

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
	//if (x0 > x1) std::swap(x0, x1);

	return true;
} 

void main(){

	Ray ray = r[gl_WorkGroupID.x* gl_WorkGroupID.y];
	Point point = p[2 * gl_LocalInvocationID.x * gl_LocalInvocationID.y * gl_LocalInvocationID.z ];
	float radius = 1;

	vec3 L = vec3(ray.origin.x, ray.origin.y, ray.origin.z)  - point.pos;
	float a  = dot(ray.dir, ray.dir);
	float b = 2* dot(ray.dir, L);
	float c = dot(L, L) - (radius*radius);


	float t1 = 0;
	float t2 = 0;
	vec3 col;
	if(solveQuadratic(a, b, c, t1, t2))
	{
		// get closest point of the 2 
		float f = distance(t1,t2);
		col = mix(vec3(0), point.col.xyz, (f/radius*2));
	}

	//ray.col.xyz += col;

	imageStore( dst, ivec2(gl_GlobalInvocationID.xy*200), (point.col));

}