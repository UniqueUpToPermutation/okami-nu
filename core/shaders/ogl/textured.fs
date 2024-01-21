#version 330 core

in vec2 vsUV;

uniform sampler2D uTextureSampler;
uniform vec4 uColor;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(uTextureSampler, vsUV) * uColor;
}