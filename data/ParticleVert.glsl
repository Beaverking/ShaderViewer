#version 450 core

layout (location = 0) in vec4 partPosition;
layout (location = 1) in vec2 partVelocity;
layout (location = 2) in vec4 partColor;

uniform mat4 MVP;

out vec2 pVel;
out vec4 pCol;

void main(void)
{
	pVel = partVelocity;
	pCol = partColor;
   	//vec2 worldPos = partPosition;
    gl_Position = MVP * partPosition;//vec4(worldPos, 0, 1);
}