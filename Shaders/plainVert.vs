#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBitan;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal ;
out vec3 positionWorldSpace;
out vec2 uv;
out mat3 TBN;

void main()
{  
    gl_Position = projection * view * model*vec4(aPos,1.0);
    positionWorldSpace = (model * vec4(aPos, 1.0)).xyz;

    normal = (model * vec4(aNormals, 0)).xyz; //to world space, add to tangent space later
    
    vec3 T = normalize(model * vec4(aTan, 0)).xyz; 
    vec3 Bt = normalize(model * vec4(aBitan, 0)).xyz;

    TBN = mat3(T, Bt, normal);

    uv = aUV;

}