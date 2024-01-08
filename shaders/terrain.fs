#version 410 core

in float Height;

out vec4 FragColor;

void main()
{
	float h = (Height + 16)/64.0f;
	FragColor = mix(vec4(0.5, 0.4, 0.1, 1.0), vec4(0.9, 0.9, 1.0, 1.0), h);
}