#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

smooth out vec2 uv;
smooth out vec3 fN;
smooth out vec3 fE;
smooth out vec3 fL;
smooth out vec3 sFL;
smooth out vec3 spotPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec4 LightArray;

uniform vec4 lightPosition;

void calculateLighting( vec4 lPos, vec4 vPos, vec3 normal );

void main(void)
{
    vec4 vPos = vec4(v_position, 1.0 );
    mat4 modelView = viewMatrix * modelMatrix;
    mat4 mvp;
	vec4 adjustedPos;
	vec4 adjustedNorm;

    mvp = ( projectionMatrix * modelView );

    gl_Position = mvp * vPos;

	adjustedPos = (modelView * vPos);
	adjustedNorm = (modelView * vec4( vNormal, 0.0 ) );

	calculateLighting( LightArray, adjustedPos, adjustedNorm.xyz );
	
    uv = v_UV;

    spotPosition = (modelMatrix * vPos).xyz;
}

void calculateLighting( vec4 lPos, vec4 vPos, vec3 normal )
{
    fN = normal;
	fE = vPos.xyz;
	fL = lPos.xyz;

    if( lPos.w != 0.0 )
    {
        fL = lPos.xyz - vPos.xyz;
    }

    sFL = lightPosition.xyz;
}
