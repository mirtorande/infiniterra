#version 410 core

in vec3 Position, Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
	//float h = (Height + 16)/64.0f;
	//FragColor = mix(vec4(0.5, 0.4, 0.1, 1.0), vec4(0.9, 0.9, 1.0, 1.0), h);
    // Light is sun color
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(camPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;


    vec3 objectColor = vec3(0.5, 0.4, 0.1);
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}