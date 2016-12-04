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
void ProcessOcean( );

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient );

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, int index );

void main(void)
{
	

	if( typeOfObject == NO_LIGHTING_TYPE )
	{
		ProcessUnlitObject( );
	}
	else if( typeOfObject == WAVE_TYPE )
	{
		//ProcessWave( );
		ProcessLitObject( );		
		ProcessOcean( );
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

/************************

@brief ProcessOcean

@details generates waves. based on code from the following link(s):
			http://ogre3d.org/forums/viewtopic.php?f=2&t=59928

@note none

*************************/


void ProcessOcean( )
{
	int index, numLights, numSpotLights;

	vec4 finalLight = vec4( 0.0, 0.0f, 0.0f, 1.0f );
	vec3 normedNormal;
	vec3 normedE = normalize( -fE );
	vec3 normedL;
	vec3 normedSL;
	vec3 halfVec;

	float mixLevel;
	//waves ///////////////////////////////////////////////////////
	/************************************************************************
	Based off of code from http://ogre3d.org/forums/viewtopic.php?f=2&t=59928
	*************************************************************************/
	vec3 waveRise = vec3( 0, 0, 0 );
	vec2 waveUV;
	vec4 waveHeight;
	vec3 reflection;
	vec4 reflColor;
	float interp;

	waveUV = vec2( uv.x + 0.13 * time, uv.y + 0.18 * time );
	waveHeight = texture2D( waveMap, waveUV );
	
	//make this work on all 3 axes
	waveRise.y = 2.0 * waveHeight.x  - 1.0;

	waveRise = normalize( fN + waveRise ); //add in anti-aliasing

	reflection = reflect( fE, waveRise );

	reflColor = texture2D( textureSampler, reflection.xy );

	interp = 1.0 - dot( -normalize( fE ), waveRise );

	mixLevel = clamp( 0.3 + pow( interp, 6.08 ), 0.0, 1.0 );

	frag_color = mix( vec4( 0.0, 0.123, 0.54, 1.0 ), reflColor, mixLevel );
	// end waves /////////////////////////////////////////////////////////	

	//lighting////////////////////////////////////////////////////////////

	normedNormal = waveRise;

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

	//end lighting///////////////////////////////////////////////////////

	//result

	frag_color = mix( frag_color, finalLight * vec4( 0.0, 0.123, 0.54, 1.0 ), 1.0 - mixLevel );
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