#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

smooth out vec2 uv;
smooth out vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

//light info
uniform vec4 LightArray;
uniform vec4 AmbientColor;

//object info
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;
uniform float Shininess;

//spot light info
uniform vec4 lightPosition;
uniform vec4 sAmbient;
uniform vec3 lightDir;
uniform float clip;

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal );
vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition );

void main(void)
{
    vec4 vPos = vec4(v_position, 1.0);
    mat4 modelView = viewMatrix * modelMatrix;
	vec3 pos = (modelView * vPos).xyz;
    vec3 normedNormal = normalize( modelView * vec4( vNormal, 0.0 ) ).xyz;
	vec3 normedE = normalize( -pos );
	vec3 normedL = normalize( LightArray.xyz  - pos );
	vec3 halfVec = normalize( normedL + normedE );


    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vPos;
    uv = v_UV;

	color = getLight( normedL, halfVec, normedNormal ) 
        + getSpotLight( normalize( lightPosition.xyz ),  
                        normalize( lightPosition.xyz + (modelMatrix * vPos).xyz ), 
                        normedNormal, modelMatrix * vPos );
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

vec4 getSpotLight( vec3 incoming, vec3 halfway, vec3 normal, vec4 vPosition )
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

    lightDirection = normalize( lightPosition.xyz - vPosition.xyz );
    
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
