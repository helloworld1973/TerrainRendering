#version 400

layout (location = 0) in vec4 vertPosition;

void main()
{
    gl_Position = vertPosition;
}