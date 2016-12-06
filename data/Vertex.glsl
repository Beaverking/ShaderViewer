#version 450 core

uniform mat4 MVP;

in vec2 position;
in vec4 vertexColor;
in vec2 textureCoord;

out vec2 textureCoordV;
out vec4 destColor;

void main(void)
{
    textureCoordV = textureCoord;
    destColor = vertexColor;
    gl_Position = MVP * vec4(position, 0, 1);
}