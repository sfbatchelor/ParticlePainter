#version 440 core

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.8;



void main() {



	vec4 texCol = texture(tex0, Texcoord);
	outputColor = texCol;
	//outputColor = vec4(1,0,0,1);

}