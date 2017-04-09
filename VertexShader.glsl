// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

layout(location = 0) in vec4 VertexPosition;

flat out int type;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix = mat4(1.f);
// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	type = int(VertexPosition.w);
	gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*vec4(VertexPosition.xyz, 1.0);
}
