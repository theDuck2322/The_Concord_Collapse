#version 450 core

layout(location = 0) in vec3 aPos;   // vertex position
layout(location = 1) in vec2 aTex;   // texture position

uniform mat4 model;
uniform mat4 viewproj;

out vec2 TexCoord;


void main()
{
    TexCoord = aTex;
    gl_Position = viewproj * model * vec4(aPos, 1.0);
}