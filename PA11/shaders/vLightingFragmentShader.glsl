#version 330

#define LIGHTING_TYPE 0
#define NO_LIGHTING_TYPE 1
#define WAVE_TYPE 2

smooth in vec2 uv;
smooth in vec4 color;

out vec4 frag_color;

uniform sampler2D textureSampler;

uniform int typeOfObject;


void main(void)
{
	if( typeOfObject == WAVE_TYPE )
	{
		frag_color = color * mix( vec4( 0.0, 0.53, 0.658, 1.0 ), 
								  texture2D( textureSampler, uv.xy ), 
								  uv.x * uv.y / ( uv.x - uv.y ) );
	}
	else
	{
		frag_color = color * texture2D( textureSampler, uv.xy );
	}
    
}
