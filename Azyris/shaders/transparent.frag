#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 color = texture(texture_diffuse1, TexCoords);

    // discard fully transparent pixels (optional, avoids z-fighting and artifacts)
    if(color.a < 0.1)
        discard;

    FragColor = color;
}