#version 330



uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;
uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;

/* uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLight;
uniform SpotLight gSpotLight;
uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;
uniform int gLightType; */


vec4 CalcLightInternal(vec3 LightDirection,
					   vec3 WorldPos,
					   vec3 Normal)
{
    vec4 AmbientColor = lightColour * 0.3;
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = lightColour * 0.3 * DiffuseFactor;

        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, 33);
        if (SpecularFactor > 0.0) {
            SpecularColor = lightColour * 0.33 * SpecularFactor;
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}



vec4 CalcPointLight(vec3 WorldPos, vec3 Normal)
{
    vec3 LightDirection = WorldPos - lightPos;
	
		

    float Distance = length(LightDirection);
	float atten = 1.0 - clamp ( Distance / lightRadius , 0.0 , 1.0);
    LightDirection = normalize(LightDirection);

    vec4 Color = atten*CalcLightInternal(LightDirection, WorldPos, Normal);

   /*  float Attenuation =  gPointLight.Atten.Constant +
                         gPointLight.Atten.Linear * Distance +
                         gPointLight.Atten.Exp * Distance * Distance;

    Attenuation = max(1.0, Attenuation); */
	

    return Color;
}


vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}


out vec4 FragColor;

void main()
{
    vec2 TexCoord = CalcTexCoord();
	vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
	vec3 Color = texture(gColorMap, TexCoord).xyz;
	vec3 Normal = texture(gNormalMap, TexCoord).xyz;
	Normal = normalize(Normal);

    FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal);
	
//	FragColor = texture(gColorMap, TexCoord);
//	FragColor = vec4(1,0,0,1);
}
