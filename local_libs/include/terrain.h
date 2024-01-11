#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "shader.h"

class Terrain {
public:
    Terrain(const int width, const int height, std::vector<float> geometry, static unsigned char* heights, unsigned res);
    Terrain(Terrain& o) = delete;
    inline Terrain(Terrain&& o) : tessHeightMapShader(o.tessHeightMapShader), tessHeightMapWireShader(o.tessHeightMapWireShader) {
        resolution = o.resolution;
        terrainVAO = o.terrainVAO;
        terrainVBO = o.terrainVBO;
        heightsTexture = o.heightsTexture;
        vertices = std::move(o.vertices);

        o.terrainVAO = 0;
        o.terrainVBO = 0;
    }
    ~Terrain();

    void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

    operator Terrain&() = delete;


private:
    unsigned resolution, terrainVAO, terrainVBO, heightsTexture;
    std::vector<float> vertices;
    Shader tessHeightMapShader, tessHeightMapWireShader;
};

#endif // !TERRAIN_H
