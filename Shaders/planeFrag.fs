#version 410 core

vec3 GetDirectionalLight(vec3 norm, vec3 viewDir, float shadow);
vec3 GetPointLight(vec3 norm, vec3 viewDir);
vec3 GetSpotLight(vec3 norm, vec3 viewDir);
vec2 ParalaxMapping(vec2 texCoords, vec3 viewDir);
float BakeShadows(vec4 fragPosLightSpace);

// out vec4 FragColor;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 brightColor;

in vec3 normal ;
in vec3 positionWorldSpace;
in vec2 uv;
in mat3 TBN;

struct pointLight {
    vec3 position;
    vec3 color; //more color for stuff;
    float Kc;
    float Kl;
    float Ke;
};

struct spotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float Kc;
    float Kl;
    float Ke;

    float inRad;
    float outRad;

};

uniform float bloomBrightness;
uniform vec3 lightCol; 
uniform vec3 objectCol;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;

uniform mat4 lightSpaceMatrix;

uniform pointLight pLight;
uniform spotLight sLight;

// struct these and add ambient
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform sampler2D displacementTexture;
uniform sampler2D shadowMap;

uniform float PXscale;

float ambientFactor = 0.3f;
float shinePower = 256;
float specularStrenght = 0.75;
//float rimSharpness = 4;
//float rimBrightness = 2;

void main()
{   

    vec4 positionLightSpace = lightSpaceMatrix * vec4(positionWorldSpace, 1.0);
    float shadow = BakeShadows(positionLightSpace);
    // normalize normal to be sure
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPosition - positionWorldSpace);
    vec2 texCoords = uv;
    texCoords = ParalaxMapping(uv, viewDir);

    norm = texture(normalTexture, texCoords).xyz;
    norm = (norm*2.0)-1.0;
    norm = normalize(TBN*norm); //inverse function to take from world to tangent space


    vec3 result = vec3(0.0);
    //result = GetDirectionalLight(norm, viewDir);
    // vec3 spotLight = GetSpotLight(norm, viewDir);
    // vec3 pointLight = GetPointLight(norm, viewDir);

    vec3 directionalLight = GetDirectionalLight(norm, viewDir, shadow);
    vec3 pointLight = GetPointLight(norm, viewDir);
    result = result + directionalLight;
    //result = result + directionalLight + pointLight;
    //result = result + pointLight;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(norm, 1.0);

    float brightness = max(max(result.x, result.y), result.z);
    if (brightness > bloomBrightness)
        brightColor = FragColor;
    else
        brightColor = vec4(vec3(0.0), 1.0);

}

vec3 GetDirectionalLight(vec3 norm, vec3 viewDir, float shadow) {

    vec3 diffMapColor = texture(diffuseTexture, uv).xyz; 
    vec3 specMapColor = vec3(texture(specularTexture, uv).x); 

    float diffuseFactor = dot(norm, -lightDirection);
    diffuseFactor = max(diffuseFactor, 0.0);
    vec3 diffuseColor = lightCol * diffMapColor * diffuseFactor;
    vec3 ambientColor = lightCol * diffMapColor * ambientFactor;
    
    vec3 halfwayVec = normalize(-lightDirection + viewDir);
    float blinnFactor = dot(norm, halfwayVec);

    blinnFactor = max(blinnFactor, 0.0);
    blinnFactor = pow(blinnFactor, shinePower);
    
    vec3 blinnColor = lightCol * blinnFactor * specMapColor ;

    vec3 result = ambientColor + (1.0 - shadow) * (diffuseColor + blinnColor);
    return result;
}

vec3 GetPointLight(vec3 norm, vec3 viewDir)
{
    
   vec3 diffMapColor = texture(diffuseTexture, uv).xyz;
   //vec3 specMapColor = vec3(texture(specularTexture, uv).x); //do I want a specular map for this?
    //point light;
   float dist = length(pLight.position - positionWorldSpace);
   float attn = 1.0/(pLight.Kc + (pLight.Kl * dist) + (pLight.Ke * (dist*dist)));
   vec3 plightDir = normalize(pLight.position - positionWorldSpace);

   //point light ambient - maybe
   //vec3 ambientColor = lightCol * diffMapColor * ambientFactor; //set to be equal to point light ambient color once its in struct
   vec3 ambientColor = lightCol * diffMapColor * ambientFactor; //set to be equal to point light ambient color once its in struct
   ambientColor = ambientColor * attn; //optional, see if you like;

   //point light diffuse
   float diffuseFactor = dot(norm, plightDir);
   diffuseFactor = max(diffuseFactor, 0.0);
   vec3 diffuseColor = lightCol * diffMapColor * diffuseFactor; //set to be equal to point light diffuse color once its in struct
   diffuseColor = diffuseColor * attn;

   //point light spec
   vec3 halfwayVec = normalize(-lightDirection + viewDir);
   float blinnFactor = dot(norm, halfwayVec);
   blinnFactor = max(blinnFactor, 0.0);
   blinnFactor = pow(blinnFactor, shinePower);
   vec3 blinnColor = lightCol * blinnFactor * specularStrenght;  //set to be equal to point light blinn color once its in struct
   blinnColor = blinnColor * attn;

   vec3 pointLightResult = ambientColor + diffuseColor + blinnColor;
   return pointLightResult;
}

vec3 GetSpotLight(vec3 norm, vec3 viewDir) {

    float dist = length(sLight.position - positionWorldSpace);
    float attn = 1.0/(sLight.Kc + (sLight.Kl * dist) + (sLight.Ke * (dist*dist)));
    vec3 slightDir = normalize(sLight.position - positionWorldSpace);

    //spot light diffuse
    float diffuseFactor = dot(norm, slightDir);
    diffuseFactor = max(diffuseFactor, 0.0);
    vec3 diffuseColor = sLight.color * objectCol * diffuseFactor;
    diffuseColor = diffuseColor * attn;


    //spot light spec
    vec3 halfwayVec = normalize(-lightDirection + viewDir);
    float blinnFactor = dot(norm, halfwayVec);
    blinnFactor = max(blinnFactor, 0.0);
    blinnFactor = pow(blinnFactor, shinePower);
    vec3 blinnColor = sLight.color * blinnFactor * specularStrenght;
    blinnColor = blinnColor * attn;

    float theta = dot(-slightDir, normalize(sLight.direction));
    float denom = (sLight.inRad - sLight.outRad);
    float illum = (theta - sLight.outRad) / denom;
    illum = clamp(illum, 0.0, 1.0);

    diffuseColor = diffuseColor * illum;
    blinnColor = blinnColor * illum;

    //float intensity = pow((1 - dot(normal, viewDir)), rimSharpness);  revisit later
    //flaot Rim = rimBrightness * intensity;

    vec3 spotLightResult = diffuseColor + blinnColor;
    return spotLightResult;
}

vec2 ParalaxMapping(vec2 texCoords, vec3 viewDir) 
{
    float layers = 10;
    float layerDepth = 1.0 / layers;
    float currentLayerDepth = 0;
    vec2 P = viewDir.xy * PXscale;
    vec2 deltaTexCoords = P / layers;
    vec2 currentTexCoods = texCoords;
    float currentDepthMapValue = texture(displacementTexture, currentTexCoods).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoods -= deltaTexCoords;
        currentDepthMapValue = texture(displacementTexture, currentTexCoods).r;
        currentLayerDepth += layerDepth;
    }
    //float height = texture(displacementTexture, texCoords).r;
    //return texCoords - (viewDir.xy) * (height * PXscale);
    return currentTexCoods;
}

float BakeShadows(vec4 fragPosLightSpace) {

    float offsetBias = 0.015f;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = 0;
    if(currentDepth - offsetBias > closestDepth) 
    {
        shadow = 1;
    }
    
    if (projCoords.z > 1.0)
    {
        shadow = 0;
    }

    return shadow;
}