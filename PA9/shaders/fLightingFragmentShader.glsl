#version 330

smooth in vec2 uv;
smooth in vec3 fN;
smooth in vec3 fL;
smooth in vec3 fE;

smooth in vec4 inPosition; 
smooth in vec4 lPosition;

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

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition, vec4 lPos );

void main(void)
{
    vec4 finalLight;
	vec3 normedNormal = normalize( fN );
	vec3 normedEmmission = normalize( fE );
	vec3 normedL = normalize( fL );
	vec3 halfVec = normalize( normedL + normedEmmission );

	finalLight = getLight( normedL, halfVec, normedNormal ) + getSpotLight( normedL, halfVec, normedNormal, inPosition, lPosition );

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

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition, vec4 lPos )
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

    lightDirection = normalize( lPos.xyz - vPosition.xyz );

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

        finalColor = max( intensity * diffuse + specular, ambient );
    }

    return finalColor;

}