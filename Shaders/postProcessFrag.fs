#version 410 core

out vec4 FragColor;

uniform sampler2D image;

in vec2 uv;

void main()
{
	vec3 result = texture(image, uv).rgb;
	result = result / (result + vec3(1.0f));	// basic tone mapping
	FragColor = vec4(result, 1.0);
}