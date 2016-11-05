#version 330

smooth in vec2 uv;
smooth in vec3 fN;
smooth in vec3 fL;
smooth in vec3 fE;

out vec4 frag_color;

uniform sampler2D textureSampler;

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal );

void main(void)
{
    vec4 finalLight;
	vec3 normedNormal = normalize( fN );
	vec3 normedEmmission = normalize( fE );
	vec3 normedL = normalize( fL );
	vec3 halfVec = normalize( normedL + normedEmmission );

	finalLight = getLight( normedL, halfVec, normedNormal );

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

	ambient = vec4( 0.1, 0.0, 1.0, 1.0); //replace vec4 with ambient product

	kd = max( dot( incoming, normal ), 0.0 );

	diffuse = kd * vec4( 0.5, 0.5, 0.5, 1.0 ); //replace vec4 with diffuse product

	ks = pow( max( dot( normal, halfway ), 0.0 ) , 0.5 /*shininess*/);

	specular = ks * vec4( 0.5, 0.5, 0.5, 0.5 ); //replace vec4 with specular product

	if( dot(incoming, normal ) < 0.0 )
	{
	    specular = vec4( 0.0, 0.0, 0.0, 1.0 );
	}

	retColor = ambient + diffuse + specular;
	retColor.a = 1.0;

	return retColor;
}

