#version 410 core

layout (location = 0) in vec3 aPosition;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPosition, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}