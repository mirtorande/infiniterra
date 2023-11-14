#version 330 core

in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D terrainTexture;

void main()
{
    // Simple color based on height, you can modify this part
    float height = FragPos.y;
    vec3 color = texture(terrainTexture, FragPos.xz).rgb; // Use texture color

    // Adjust colors based on height, e.g., add snow on mountain tops
    if (height > 0.75)
        color = mix(color, vec3(0.0, 1.0, 0.5), height/10); // Smooth transition

    if (height > 1.5)
        color = mix(color, vec3(1.0), height/10); // Smooth transition

    FragColor = vec4(color, 1.0);
}
