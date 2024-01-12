#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "shader.h"

class Terrain {
public:
    // Constructor
    Terrain(const int width, const int height, std::vector<float> geometry, unsigned char* heights, unsigned res);
    // Copy Constructor
    Terrain(Terrain& o) = delete;
    // Move Constructor
    inline Terrain(Terrain&& o) noexcept : tessHeightMapShader(o.tessHeightMapShader), tessHeightMapWireShader(o.tessHeightMapWireShader)  {
        resolution = o.resolution;
        terrainVAO = o.terrainVAO;
        terrainVBO = o.terrainVBO;
        heightsTexture = std::move(o.heightsTexture);
        vertices = std::move(o.vertices);

        // Unbind the VAO and VBO from the other object
        o.terrainVAO = 0;
        o.terrainVBO = 0;
    }
    // Copy Operator
    operator Terrain&() = delete;
    // Destructor
    ~Terrain();

    void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);



private:
    unsigned resolution, terrainVAO, terrainVBO, heightsTexture;
    std::vector<float> vertices;
    Shader tessHeightMapShader, tessHeightMapWireShader;
};

#endif // !TERRAIN_H
