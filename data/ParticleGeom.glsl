#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2 pVel[];
in vec4 pCol[];

uniform vec2 pixelSize;

out vec4 colorFrag;

void main(void)
{
    vec4 currPos = gl_in[0].gl_Position;
	gl_Position = currPos;
	gl_Position.x -= pixelSize.x;
	gl_Position.y -= pixelSize.y;
	colorFrag = pCol[0];
	EmitVertex();
	gl_Position = currPos;
	gl_Position.x += pixelSize.x;
	gl_Position.y -= pixelSize.y;
	colorFrag = pCol[0];
	EmitVertex();
	gl_Position = currPos;
	gl_Position.x -= pixelSize.x;
	gl_Position.y += pixelSize.y;
	colorFrag = pCol[0];
	EmitVertex();
	gl_Position = currPos;
	gl_Position.x += pixelSize.x;
	gl_Position.y += pixelSize.y;
	colorFrag = pCol[0];
	EmitVertex();
    EndPrimitive();
} 