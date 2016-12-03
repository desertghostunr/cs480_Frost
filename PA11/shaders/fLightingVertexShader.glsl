#version 330

#define MAX_NUM_LIGHTS 8

#define LIGHTING_TYPE 0
#define NO_LIGHTING_TYPE 1
#define WAVE_TYPE 2

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

//information to pass to the fragment shader
struct SpotLightInfo
{
    vec3 sFL;
    vec3 spotPosition;
};

smooth out vec2 uv;
smooth out vec3 fN;
smooth out vec3 fE;

smooth out vec3 fL[ MAX_NUM_LIGHTS ];
smooth out SpotLightInfo sLInfo[ MAX_NUM_LIGHTS ];

//uniforms

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

//lighting information
struct Light
{
    vec4 position;
    vec4 ambient;
};

uniform Light light[ MAX_NUM_LIGHTS ];

uniform int numberOfLights;

//spot light
struct SpotLight
{
    vec4 position;
    vec4 ambient;
    vec3 direction;
    float clip;
};

uniform SpotLight spotLight[ MAX_NUM_LIGHTS ];

uniform int numberOfSpotLights;

//type of object
uniform int typeOfObject;

void ProcessLitObject( );
void ProcessUnlitObject( );

void main(void)
{
	if( typeOfObject == NO_LIGHTING_TYPE )
	{
		ProcessUnlitObject( );
	}
	else
	{
		ProcessLitObject( );
	}
}

void ProcessLitObject( )
{
	int index, numLights, numSpotLights;

	vec4 vPos = vec4( v_position, 1.0 );
	mat4 modelView = viewMatrix * modelMatrix;
	mat4 mvp;
	vec4 adjustedPos;
	vec4 adjustedNorm;

	mvp = ( projectionMatrix * modelView );

	gl_Position = mvp * vPos;

	uv = v_UV;

	if( numberOfLights > MAX_NUM_LIGHTS )
	{
		numLights = MAX_NUM_LIGHTS;
	}
	else
	{
		numLights = numberOfLights;
	}

	if( numberOfSpotLights > MAX_NUM_LIGHTS )
	{
		numSpotLights = MAX_NUM_LIGHTS;
	}
	else
	{
		numSpotLights = numberOfSpotLights;
	}

	//general light information

	adjustedPos = ( modelView * vPos );
	adjustedNorm = ( modelView * vec4( vNormal, 0.0 ) );

	fN = adjustedNorm.xyz;
	fE = adjustedPos.xyz;

	for( index = 0; index < numLights; index++ )
	{
		fL[ index ] = light[ index ].position.xyz;

		if( light[ index ].position.w != 0.0 )
		{
			fL[ index ] = light[ index ].position.xyz - adjustedPos.xyz;
		}
	}

	for( index = 0; index < numSpotLights; index++ )
	{
		//spot information
		sLInfo[ index ].spotPosition = ( modelMatrix * vPos ).xyz;
		sLInfo[ index ].sFL = spotLight[ index ].position.xyz;
	}
}

void ProcessUnlitObject( )
{
	vec4 vPos = vec4( v_position, 1.0 );
	mat4 modelView = viewMatrix * modelMatrix;
	mat4 mvp;

	mvp = ( projectionMatrix * modelView );

	gl_Position = mvp * vPos;

	uv = v_UV;
}
