#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

smooth out vec2 uv;
smooth out vec3 fN;
smooth out vec3 fE;
smooth out vec3 fL;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void calculateLighting( vec4 lPos, vec4 vPos );

void main(void)
{
    vec4 vPos = vec4(v_position, 1.0 );

	calculateLighting( vec4( 1.0, 1.0, 1.0, 1.0 ), vPos );

    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vPos;
    uv = v_UV;
}

void calculateLighting( vec4 lPos, vec4 vPos )
{
    fN = vNormal;
	fE = vPos.xyz;
	fL = lPos.xyz;

	if( lPos.w != 0.0 )
	{
	    fL = lPos.xyz - vPos.xyz;
	}
}
