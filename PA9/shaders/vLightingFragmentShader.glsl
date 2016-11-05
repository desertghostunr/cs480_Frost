#version 330

smooth in vec2 uv;
smooth in vec4 color;

out vec4 frag_color;

uniform sampler2D textureSampler;


void main(void)
{
    frag_color = color * texture2D( textureSampler, uv.xy );
}
