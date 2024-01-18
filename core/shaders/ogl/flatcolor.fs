#version 330 core

in vec2 vsUV;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vsUV.x, vsUV.y, 1.0, 1.0);
}