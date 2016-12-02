#version 450 core

uniform sampler2D texUnit;

in vec2 textureCoordV;

out vec3 color;

void main(void)
{
	//vec3 LightColor = vec3(1,1,1);
	//float LightPower = 50.0f;

	//vec3 mDiffuseColor = vec3(0.5,0.5,0.5);
	//vec3 mAmbientColor = vec3(0.1,0.1,0.1);
	//vec3 mSpecularColor = vec3(0.2,0.2,0.2);

	//float distSquared = pow(LightPosition_worldspace.x - Position_worldspace.x, 2) + pow(LightPosition_worldspace.y - Position_worldspace.y, 2) + pow(LightPosition_worldspace.z - Position_worldspace.z, 2);

    //vec3 n = normalize(Normal_cameraspace);
    //vec3 l = normalize(LightDirection_cameraspace);
    //vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1);
    //float cosTheta = clamp( dot( n,l ), 0,1 );

    //vec3 E = normalize(EyeDirection_cameraspace);
    //vec3 R = reflect(-l,n);
    //float cosAlpha = clamp( dot( E,R ), 0,1 );
    vec4 texColor = texture(texUnit, textureCoordV);

    color = vec3(1.0f, 1.0f, texColor.z);
    //color.x = 1.0f;
}