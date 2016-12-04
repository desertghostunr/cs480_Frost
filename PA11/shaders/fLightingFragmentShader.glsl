#version 330

#define MAX_NUM_LIGHTS 8

#define LIGHTING_TYPE 0
#define NO_LIGHTING_TYPE 1
#define WAVE_TYPE 2

#define STEP_NUMBER 8

struct SpotLightInfo
{
    vec3 sFL;
    vec3 spotPosition;
};

smooth in vec2 uv;
smooth in vec3 fN;
smooth in vec3 fE;
smooth in vec3 fL[ MAX_NUM_LIGHTS ];
smooth in SpotLightInfo sLInfo[ MAX_NUM_LIGHTS ];


out vec4 frag_color;

uniform sampler2D textureSampler;

//light info
struct Light
{
    vec4 position;
    vec4 ambient;
};

uniform Light light[ MAX_NUM_LIGHTS ];

uniform int numberOfLights;

//object specific light info

uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;
uniform float Shininess;

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

//type of object
uniform int typeOfObject;

uniform sampler2D waveMap;
uniform float time;

void ProcessLitObject( );
void ProcessUnlitObject( );
/*
void ProcessWave( );

vec3 calculateNormal( vec3 eye );

float getWaveMapPoint( vec3 input );*/

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient );

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, int index );

void main(void)
{
	vec2 waveUV;

	if( typeOfObject == NO_LIGHTING_TYPE )
	{
		ProcessUnlitObject( );
	}
	else if( typeOfObject == WAVE_TYPE )
	{
		//ProcessWave( );
		ProcessLitObject( );

		waveUV = vec2( uv.x + time, uv.y + time );
		vec4 waveHeight = texture2D( waveMap, waveUV );

		if( waveHeight.x < 0 )
		{
			waveHeight.x = 1 + waveHeight.x;
		}

		frag_color.r = frag_color.r * waveHeight.x;
		frag_color.g = frag_color.g * waveHeight.x;
		frag_color.b = frag_color.b * waveHeight.x;
		frag_color.a = frag_color.a * waveHeight.x;

	}
	else
	{
		ProcessLitObject( );
	}
}

void ProcessLitObject( )
{
	int index, numLights, numSpotLights;

	vec4 finalLight = vec4( 0.0, 0.0f, 0.0f, 1.0f );
	vec3 normedNormal = normalize( fN );
	vec3 normedE = normalize( -fE );
	vec3 normedL;
	vec3 normedSL;
	vec3 halfVec;

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

	for( index = 0; index < numLights; index++ )
	{
		normedL = normalize( fL[ index ] );
		halfVec = normalize( normedL + normedE );

		finalLight += getLight( normedL, halfVec, normedNormal, light[ index ].ambient );
	}

	for( index = 0; index < numSpotLights; index++ )
	{
		normedSL = normalize( sLInfo[ index ].sFL );

		finalLight += getSpotLight( normedSL,
									normalize( sLInfo[ index ].sFL + sLInfo[ index ].spotPosition ),
									normedNormal, index );
	}

	frag_color = finalLight * texture2D( textureSampler, uv.xy );
}

void ProcessUnlitObject( )
{
	frag_color = texture2D( textureSampler, uv.xy );
}

/***************************************************

@brief ProcessWave

@details generates waves. partially based off of Alexander Alekseev's "Seascape"
		 which was published under the 
		 Creative Commons Attribution-NonCommercial-ShareAlike3.0 Unported License
		 see: https://www.shadertoy.com/view/Ms2SD1

@note None

***************************************************/
/*void ProcessWave( )
{
	vec3 wNormal;
	//use fE from the vertex shader lighting calculations to get the dir of eye
	vec3 eyePos = fE;
	

	//calculate a new normal for the wave
	wNormal = calculateNormal( eyePos );

}

vec3 calculateNormal( vec3 eye )
{
	vec3 wNorm;
	float tempVal;

	tempVal = getWaveMapPoint( eye );
}

float getWaveMapPoint( vec3 input )
{

}*/

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient )
{
    vec4 retColor;
	vec4 diffuse;
	vec4 specular;
	float ks;
	float kd;

	kd = max( dot( incoming, normal ), 0.0 );

	diffuse = kd * DiffuseColor; 

	ks = pow( max( dot( normal, halfway ), 0.0 ) , Shininess );

	specular = ks * DiffuseColor;

	if( dot(incoming, normal ) < 0.0 )
	{
	    specular = vec4( 0.0, 0.0, 0.0, 1.0 );
	}

	retColor = ambient + diffuse + specular;
	retColor.a = 1.0;

	return retColor;
}

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, int index )
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

    lightDirection = normalize( sLInfo[ index ].sFL.xyz - sLInfo[ index ].spotPosition.xyz );

    angle = dot( lightDirection, normalize( -spotLight[ index ].direction ) );

    if( angle > spotLight[ index ].clip )
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