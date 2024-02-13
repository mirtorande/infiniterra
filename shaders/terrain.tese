#version 410 core
layout(quads, fractional_even_spacing, ccw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 TextureCoord[];

out float worldPositionY;
out vec3 viewPosition;
out vec3 groundColor;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(0.02, 0.01)))); }

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // Sample height map using adjusted texture coordinates
    float heightFactor = 256.0;
    float textureValue = texture(heightMap, texCoord).x;
    float height = textureValue * heightFactor - heightFactor/4;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0 + normal * height;

    gl_Position = projection * view * model * p;
    worldPositionY = (model * p).y;

    viewPosition = (view * model * p).xyz;
    float grassHeight = -50.0 + textureValue * 100;
    float snowHeight = 30.0 - textureValue * 100;
    const vec3 grassColor = vec3(0.2, 0.3, 0.1);
    const vec3 dryColor = vec3(0.5, 0.4, 0.3);
    const vec3 snowColor = vec3(1.0, 1.0, 1.0);

    //Mix between the three different ground colors based on height, normalizing
    groundColor = mix(grassColor, dryColor, clamp((height - grassHeight)/30, 0, 1));
    groundColor = mix(groundColor, snowColor, clamp((height - snowHeight)/30, 0, 1));


}

