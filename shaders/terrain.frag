#version 410 core

in float worldPositionY;
in vec3 viewPosition;
in vec3 groundColor;

out vec4 FragColor;

uniform mat4 view;
uniform vec3 lightPos;
uniform vec3 camPos;

float anisotropicFog() {
	float fog = (0.1f-viewPosition.z)/(6500.0 -0.1);
    //float eyePosY = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).y;
    //const float fogHeight = 10.0;
    //float percentInside = fogHeight / (eyePosY - worldPositionY);
    //percentInside = clamp(percentInside, 0.0, 1.0);
	return clamp(fog, 0, 1);
}

void main()
{
    // Light is sun color
    vec3 lightColor = vec3(1.0, 0.9, 0.9);
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    float fog = anisotropicFog();


    vec3 norm = normalize(cross(dFdx(viewPosition), dFdy(viewPosition)));

    //vec3 lightDir = normalize(lightPos - viewPosition);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 5.0));
    float diff = dot(norm, lightDir);
    diff = max(diff, -diff * 0.2);

    // specular
    float specularStrength = 0.1;
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;


    vec3 skyColor = vec3(0.6, 0.6, 0.7);
    vec3 result = (diff + specular) * groundColor;
    FragColor.rgb = mix(result, skyColor, fog);
}