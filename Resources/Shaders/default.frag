#version 330 core
// Output the fragment vector
out vec4 FragColor;
// inputs a vec3 from the vertex shader
in vec3 color;

void main()
{
    // Sets the fragment color to be the colors from the vertex shader.
    FragColor = vec4(color,1.0);
}