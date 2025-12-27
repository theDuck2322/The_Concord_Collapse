#version 450 core

out vec4 FragColor;

uniform sampler2D texture1;

in vec2 TexCoord;

void main()
{
    vec4 color = texture(texture1, TexCoord);
    if (color.a < 0.1)
    {
        discard;
    }
    FragColor = color;

}