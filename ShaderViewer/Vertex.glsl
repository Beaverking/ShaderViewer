#version 450 core

uniform mat4 MVP;

in vec2 position;
in vec2 textureCoord;

out vec2 textureCoordV;

void main(void)
{
    textureCoordV = textureCoord;
    gl_Position = MVP * position
}