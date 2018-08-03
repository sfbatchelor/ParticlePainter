#version 440 core

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 Texcoord;

void main()
{
    // send the vertices to the fragment shader
	gl_Position = modelViewProjectionMatrix * position;

	Texcoord = texcoord;
}