#version 450

layout(location = 0) in vec3 aPos;   // vertex position

uniform mat4 viewProj;

void main()
{
    gl_Position = viewProj * vec4(aPos, 1.0);
}