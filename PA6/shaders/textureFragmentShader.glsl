#version 330

flat in vec2 uv;

out vec4 frag_color;

uniform sampler2D textureSampler;

void main(void)
{
  frag_color = texture2D( textureSampler, uv.xy );
}