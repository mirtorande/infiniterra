#version 330 core

in vec3 FragPos;

out vec4 FragColor;

void main()
{
    // Simple color based on height, you can modify this part
    float height = FragPos.y;
    vec3 color = vec3(0.0, 1.0, 0.0); // Green color

    // Adjust colors based on height, e.g., add snow on mountain tops
    if (height > 0.75)
        color =  vec3(0.0, 1.0, 0.5);

    if (height > 1.5)
        color = vec3(1.0); // White color

    FragColor = vec4(color, 1.0);
}
