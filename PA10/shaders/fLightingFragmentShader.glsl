#version 330

#define MAX_NUM_LIGHTS 8

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

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 ambient );

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, int index );

void main(void)
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