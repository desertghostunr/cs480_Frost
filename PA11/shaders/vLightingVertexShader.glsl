#version 330

#define MAX_NUM_LIGHTS 8

#define LIGHTING_TYPE 0
#define NO_LIGHTING_TYPE 1
#define WAVE_TYPE 2

//vertex buffer information
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

//outgoing
smooth out vec2 uv;
smooth out vec4 color;

//uniform object information
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

//light info
struct Light
{
    vec4 position;
    vec4 ambient;
};

uniform Light light[ MAX_NUM_LIGHTS ];
uniform int numberOfLights;

//spot light info
struct SpotLight
{
    vec4 position;
    vec4 ambient;
    vec3 direction;
    float clip;
};

uniform SpotLight spotLight[ MAX_NUM_LIGHTS ];
uniform int numberOfSpotLights;

//object info about lighting
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;
uniform float Shininess;

//type of object
uniform int typeOfObject;

//heightmap for waves
uniform sampler2D waveMap;
uniform float time;

void ProcessLitObject( );
void ProcessUnlitObject( );


vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient );
vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition, int index );

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

	vec2 waveUV = vec2( uv.x + time, uv.y + time );
	vec4 waveHeight = texture2D( waveMap, waveUV );

	if( waveHeight.x < 0 )
	{
		waveHeight.x = 1 + waveHeight.x;
	}

	if( typeOfObject == WAVE_TYPE )
	{
		color.r = color.r * waveHeight.x;
		color.g = color.g * waveHeight.x;
		color.b = color.b * waveHeight.x;
		color.a = color.a * waveHeight.x;
	}
}

void ProcessLitObject( )
{
	int index, numLights, numSpotLights;

	vec4 vPos = vec4( v_position, 1.0 );
	mat4 modelView = viewMatrix * modelMatrix;
	vec3 pos = ( modelView * vPos ).xyz;
	vec3 normedNormal = normalize( modelView * vec4( vNormal, 0.0 ) ).xyz;
	vec3 normedE = normalize( -pos );
	vec3 normedL;
	vec3 halfVec;


	gl_Position = ( projectionMatrix * viewMatrix * modelMatrix ) * vPos;
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

	color = vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	for( index = 0; index < numLights; index++ )
	{
		normedL = normalize( light[ index ].position.xyz - pos );
		halfVec = normalize( normedL + normedE );

		color += getLight( normedL, halfVec, normedNormal, light[ index ].ambient );
	}

	for( index = 0; index < numSpotLights; index++ )
	{
		color += getSpotLight( normalize( spotLight[ index ].position.xyz ),
							   normalize( spotLight[ index ].position.xyz
										  + ( modelMatrix * vPos ).xyz ),
							   normedNormal, modelMatrix * vPos, index );
	}
}

void ProcessUnlitObject( )
{
	vec4 vPos = vec4( v_position, 1.0 );
	gl_Position = ( projectionMatrix * viewMatrix * modelMatrix ) * vPos;
	uv = v_UV;
	color = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
}

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient )
{
    vec4 retColor;
	vec4 diffuse;
	vec4 specular;
	float ks;
	float kd;

	kd = max( dot( incoming, normal ), 0.0 );

	diffuse = kd * DiffuseColor; 

	ks = pow( max( dot( normal, halfway ), 0.0 ) , Shininess);

	specular = ks * SpecularColor;

	if( dot(incoming, normal ) < 0.0 )
	{
	    specular = vec4( 0.0, 0.0, 0.0, 1.0 );
	}

	retColor = ambient + diffuse + specular;
	retColor.a = 1.0;

	return retColor;
}

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition, int index )
{
    vec4 finalColor = vec4( 0.0, 0.0, 0.0, 0.0 );
    float angle = 1.0;
    vec3 lightDirection;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float ks;
    float kd;

    float intensity;

    float brightnessMultiplier;

    lightDirection = normalize( spotLight[index].position.xyz - vPosition.xyz );
    
    angle = dot( lightDirection, normalize( -spotLight[index].direction ) );

    if( angle > spotLight[index].clip )
    {

        ambient = spotLight[ index ].ambient;

        kd = max( dot( incoming, normal ), 0.0 );

        diffuse = kd * DiffuseColor;

        ks = pow( max( dot( normal, halfway ), 0.0 ), Shininess );

        specular = ks * SpecularColor;

        if( dot( incoming, normal ) < 0.0 )
        {
            specular = vec4( 0.0, 0.0, 0.0, 1.0 );
        }

        intensity = max( dot( normal, lightDirection ), 0.0 );

        brightnessMultiplier = max( max( max( ambient.r, ambient.g ), ambient.b ), 0.0 );

        finalColor = brightnessMultiplier * max( intensity * diffuse + specular, ambient );
    }

    return finalColor;
    
}
