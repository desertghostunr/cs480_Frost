#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;

smooth out vec2 uv;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void)
{
  gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(v_position, 1.0);
}