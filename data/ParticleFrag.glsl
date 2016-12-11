#version 450 core

in vec4 colorFrag;

out vec4 color;

void main(void)
{
    color = colorFrag;
}