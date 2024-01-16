#version 330 core

// Camera uniform block
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uViewProj;

// World uniform block
uniform mat4 uWorld;
uniform mat4 uWorldInvTrans;

out vec4 vsColor;
out vec4 vsWorld;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

void main()
{   
    vsWorld = uWorld * vec4(aPos, 1.0);
    gl_Position = uViewProj * vsWorld;
    
    vsColor = aColor;
}