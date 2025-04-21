#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec3 spotDirection; // added spot direction
    float spotCutoff;   // cutoff angle in degrees
};

in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;

/* texture */
uniform sampler2D image;

out vec4 fColor;

void main()
{
    vec3 lightVec, viewVec, reflectVec;
    vec3 normalView;
    vec3 ambient, diffuse, specular;
    float nDotL, rDotV;

    vec4 computedColor = vec4(0,0,0,1);

    for (int i=0;i<numLights;i++)
    {
        if (light[i].position.w != 0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        normalView = normalize(fNormal);
        nDotL = dot(normalView, lightVec);

        viewVec = normalize(-fPosition.xyz);
        reflectVec = reflect(-lightVec, normalView);
        reflectVec = normalize(reflectVec);

        rDotV = max(dot(reflectVec, viewVec), 0.0);

        ambient = material.ambient * light[i].ambient;
        diffuse = material.diffuse * light[i].diffuse * max(nDotL, 0.0);
        if (nDotL > 0.0)
            specular = material.specular * light[i].specular * pow(rDotV, material.shininess);
        else
            specular = vec3(0,0,0);
        computedColor += vec4(ambient + diffuse + specular, 1.0);
    }
    
    // Combine computed lighting with the texture color
    vec4 texColor = texture(image, fTexCoord.st);
    fColor = computedColor * texColor;
}
