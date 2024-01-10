#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "shader.h"

class Terrain {
public:
    Terrain(const int width, const int height, std::vector<float> geometry, static unsigned char* heights, unsigned res);
    ~Terrain();

    void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned resolution, terrainVAO, terrainVBO, heightsTexture;
    std::vector<float> vertices;
    Shader tessHeightMapShader, tessHeightMapWireShader;
};

#endif // !TERRAIN_H
