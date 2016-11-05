#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 vNormal;

smooth out vec2 uv;
smooth out vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal );

void main(void)
{
    vec4 vPos = vec4(v_position, 1.0);
    mat4 modelView = viewMatrix * modelMatrix;
	vec3 pos = (modelView * vPos).xyz;
    vec3 normedNormal = normalize( modelView * vec4( vNormal, 0.0 ) ).xyz;
	vec3 normedEmmission = normalize( -pos );
	vec3 normedL = normalize( vec4( 1.0, 2.0, 3.0, 1.0)/*light pos*/.xyz  - pos );
	vec3 halfVec = normalize( normedL + normedEmmission );


    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vPos;
    uv = v_UV;

	color = getLight( normedL, halfVec, normedNormal );
}

vec4 getLight( vec3 incoming, vec3 halfway, vec3 normal )
{
    vec4 retColor;
    vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float ks;
	float kd;

	ambient = vec4( 1.0, 1.0, 1.0, 1.0); //replace vec4 with ambient product

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
