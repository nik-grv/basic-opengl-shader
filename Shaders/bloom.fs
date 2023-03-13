#version 410 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D image;
uniform sampler2D bloomBlur;
uniform sampler2D depthMap;

vec2 focusPoint = vec2(0.5, 0.5);


void main()
{
    vec4 SampleFragmentAtFocusPoint = texture(depthMap, focusPoint);
    vec4 SamplerCurrentFragment = texture(depthMap, uv);

    float depthDistance = length(SampleFragmentAtFocusPoint - SamplerCurrentFragment);

    vec3 focusColor = texture(image, uv).rgb;
    vec3 blurColor = texture(bloomBlur, uv).rgb;
    //mix(image, bloomBlur, depthDistance);
    //vec3 mixedTexture = mix(focusColor, blurColor, depthDistance);

    vec3 hdrColor = texture(image, uv).rgb;
    vec3 bloomColor = texture(bloomBlur, uv).rgb;
    vec3 mixedTexture = mix(hdrColor, bloomColor, depthDistance);

    hdrColor += bloomColor + mixedTexture;

    vec3 reinhard = hdrColor/(hdrColor + vec3(1.0));
    FragColor = vec4(reinhard, 1.0);
}