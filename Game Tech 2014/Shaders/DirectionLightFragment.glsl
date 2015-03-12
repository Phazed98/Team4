#version 330

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;


uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;
vec4 lightColour =vec4(1,1,1,1);


vec4 CalcLightInternal(vec3 LightDirection,
					   vec3 WorldPos,
					   vec3 Normal)
{
    vec4 AmbientColor = lightColour * 0.1;
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = lightColour *0.3* DiffuseFactor;

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


vec4 CalcDirectionalLight(vec3 Direction, vec3 WorldPos, vec3 Normal)
{
	
    return CalcLightInternal(
							 Direction,
							 WorldPos,
							 Normal);
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

	
	/* vec3 Dir = vec3(1,0,0);
    FragColor = vec4(Color, 1.0) * CalcDirectionalLight(Dir, WorldPos, Normal);
	Dir = vec3(0,1,0);
	FragColor *= vec4(Color, 1.0) * CalcDirectionalLight(Dir, WorldPos, Normal); */
	FragColor = vec4(Color*0.3, 1.0);

}
