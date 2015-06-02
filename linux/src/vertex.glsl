#version 330 core
attribute vec3 vertexPosition;

void main() 
{
    gl_Position = vec4(vertexPosition, 1.0);
}