#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "shader.h"
#include "texture.h"
#include "VAO.h"
#include "EBO.h"

class Terrain {
public:
    Terrain();
    ~Terrain();

    void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

private:
    int width, height, channels;
    std::vector<float> vertices;
    std::vector<GLuint> indices;
    VAO terrainVAO;
    VBO terrainVBO;
    EBO terrainEBO;
    Shader terrainShader;
    float getHeightFromImage(const unsigned char* data, int width, int x, int y);
    void generateTerrain(std::vector<float>& vertices, std::vector<GLuint>& indices, const unsigned char* data, int width, int height, float scale);
};

#endif // !TERRAIN_H
