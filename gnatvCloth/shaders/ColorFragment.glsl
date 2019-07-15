#version 410 core

layout (location = 0) out vec4 fragColor;

uniform vec4 vertColor;

void main()
{
    fragColor = vertColor;
}
