#version 450 core

uniform sampler2D texUnit;

in vec2 textureCoordV;
in vec4 destColor;

out vec4 color;

void main(void)
{
    vec4 texColor = texture(texUnit, textureCoordV);
    color = texColor * destColor;
    color.a = texColor.a;
}