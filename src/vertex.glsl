R"(

#version 320 es

layout (location = 0) in vec2 position;

uniform float uPointSize;

void main()
{
    gl_PointSize = uPointSize;
    gl_Position = vec4(position, 0.0f, 1.0f);
}

)"
