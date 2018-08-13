#version 440 core

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 colour;

out vec4 vColour;

uniform float uAlpha = 1.;

void main()
{
    // send the vertices to the fragment shader
	gl_Position = position;

	vColour = colour/255.;
	vColour.rgb *= uAlpha;
	vColour.a = uAlpha;

}