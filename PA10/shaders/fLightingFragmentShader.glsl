#version 330

smooth in vec2 uv;
smooth in vec3 fN;
smooth in vec3 fE;
smooth in vec3 fL;
smooth in vec3 sFL;
smooth in vec3 spotPosition;

out vec4 frag_color;

uniform sampler2D textureSampler;

uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;
uniform float Shininess;

//spot light info
uniform vec4 sAmbient;
uniform vec3 lightDir;
uniform float clip;

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal );

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal );

void main(void)
{
    vec4 finalLight;
	vec3 normedNormal = normalize( fN );
	vec3 normedE = normalize( -fE );
	vec3 normedL = normalize( fL );
	vec3 normedSL = normalize( sFL );
	vec3 halfVec = normalize( normedL + normedE );

	finalLight = getLight( normedL, halfVec, normedNormal ) 
        + getSpotLight( normedSL,  normalize( sFL + spotPosition ), normedNormal );

    frag_color = finalLight * texture2D( textureSampler, uv.xy );
}

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal )
{
    vec4 retColor;
    vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float ks;
	float kd;

	ambient = AmbientColor;

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

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal )
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

    lightDirection = normalize( sFL.xyz - spotPosition.xyz );

    angle = dot( lightDirection, normalize( -lightDir ) );

    if( angle > clip )
    {

        ambient = sAmbient;

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