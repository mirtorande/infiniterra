#include "terrain.h"

Terrain::Terrain() : terrainShader("shaders/terrain.vs", "shaders/terrain.fs", "shaders/terrain.tcs", "shaders/terrain.tes") {
    unsigned char* heightmapData = stbi_load("resources/textures/terrain.png", &width, &height, &channels, 0);

    // Check if data loading was successful
    if (!heightmapData)
        exit(1);
    
    // Generate terrain vertices
    generateTerrain(vertices, heightmapData, width, height, 0.2f);

    // Free the heightmap data
    stbi_image_free(heightmapData);

    terrainVAO.Bind();
    VBO terrainVBO = VBO(vertices.data(), vertices.size() * sizeof(float));
    terrainVAO.LinkVBO(terrainVBO, 0, 3, 5);
    terrainVAO.Unbind();

    //terrainTexture.loadTexture("resources/textures/grass.tif"); NON CARICA

    //terrainTexture.bindTexture();
    terrainShader.use();
    terrainShader.setInt("terrainTexture", 0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

Terrain::~Terrain() {
    terrainVAO.Delete();
}

void Terrain::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    terrainShader.use();
    terrainShader.setMat4("model", model);
    terrainShader.setMat4("view", view);
    terrainShader.setMat4("projection", projection);
    terrainVAO.Bind();
    glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
    terrainVAO.Unbind();
}

float Terrain::getHeightFromImage(const unsigned char* data, int width, int x, int y) {
    return static_cast<float>(data[y * width + x]) / 25.0f;
}

void Terrain::generateTerrain(std::vector<float>& vertices, const unsigned char* data, int width, int height, float scale) {
    rez = 20;
    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
}
