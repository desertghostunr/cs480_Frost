#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

smooth out vec2 uv;
smooth out vec3 fN;
smooth out vec3 fE;
smooth out vec3 fL;
smooth out vec3 sFL;

smooth out vec4 inPosition;
smooth out vec4 lPosition;

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
    /*vec3 rotNormal;
    mat3 normalMat;*/
	vec4 adjustedPos;
	vec4 adjustedNorm;

    mvp = ( projectionMatrix * modelView );

    gl_Position = mvp * vPos;

    /*normalMat = mat3( modelMatrix );
    normalMat = inverse( normalMat );
    normalMat = transpose( normalMat );

    rotNormal = normalize( vNormal  * normalMat );


	calculateLighting( LightArray, vPos, rotNormal );*/

	adjustedPos = (modelMatrix * vPos);
	adjustedNorm = (modelMatrix * vec4( vNormal, 1.0 ) );

	calculateLighting( LightArray, adjustedPos, adjustedNorm.xyz );
	
    uv = v_UV;
}

void calculateLighting( vec4 lPos, vec4 vPos, vec3 normal )
{
    fN = normal;
	fE = vPos.xyz;
	fL = lPos.xyz;

	sFL = lightPosition.xyz;
}
