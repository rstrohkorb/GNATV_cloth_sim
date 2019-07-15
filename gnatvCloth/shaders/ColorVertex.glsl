#version 410 core

layout (location = 0) in vec3 inVertex; //whenever get data, location 0 on 16x4 input is a vec3 named inVertex
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

uniform mat4 MVP; //Model, View, Project
//Product of those three matrices - one for messing w/ model, one to get it into the view port, one to put it in the view volume
//Typically you only set a uniform once, and you do it yourself (not with a constructor)

void main()
{
    gl_Position = MVP * vec4(inVertex, 1.0);
}
