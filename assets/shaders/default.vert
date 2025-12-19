#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// output the color vector to the fragment shader
out vec3 color;
// uniforms to get the model, view, and projection matrix from the CPU
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos,1.0f);
    color = aColor;
}