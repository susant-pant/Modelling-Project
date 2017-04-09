// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
flat in int type;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

void main(void)
{
	vec3 color = vec3(0.f, 0.f, 1.f);
	switch (type) {
		case 0 : color = vec3(0.f, 1.f, 0.f); break;
		case 1 : color = vec3(1.f, 0.f, 0.f); break;
		case 2 : color = vec3(1.f, 1.f, 1.f); break;
	}
	FragmentColour = vec4(color, 1.0);
}
