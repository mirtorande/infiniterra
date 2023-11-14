#include "terrain.h"

Terrain::Terrain() : terrainShader("shaders/terrain.vs", "shaders/terrain.fs") {
    unsigned char* heightmapData = stbi_load("resources/textures/terrain.png", &width, &height, &channels, 0);

    // Check if data loading was successful
    if (!heightmapData)
        exit(1);
    
    // Generate terrain vertices
    generateTerrain(vertices, indices, heightmapData, width, height, 0.2f);

    // Free the heightmap data
    stbi_image_free(heightmapData);

    terrainVAO.Bind();
    VBO terrainVBO = VBO(vertices.data(), vertices.size() * sizeof(float));
    EBO terrainEBO = EBO(indices.data(), indices.size() * sizeof(int));
    terrainVAO.LinkVBO(terrainVBO, 0);
    terrainVAO.Unbind();

    terrainTexture.loadTexture("resources/textures/grass.png");

    terrainTexture.bindTexture();
    terrainShader.use();
    terrainShader.setInt("terrainTexture", 0);
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
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    terrainVAO.Unbind();
}

float Terrain::getHeightFromImage(const unsigned char* data, int width, int x, int y) {
    return static_cast<float>(data[y * width + x]) / 25.0f;
}

void Terrain::generateTerrain(std::vector<float>& vertices, std::vector<GLuint>& indices, const unsigned char* data, int width, int height, float scale) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float xPos = static_cast<float>(x) * scale;
            float yPos = getHeightFromImage(data, width, x, y) * scale;
            float zPos = static_cast<float>(y) * scale;

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }

    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            unsigned int topLeft = (y + 1) * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = y * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    /*DEBUG
    int count = 0;
    for (auto it = indices.begin(); it != indices.end() && count < 256; ++it, ++count) {
        std::cout << *it << ": " << vertices[3 * (*it)] << ", " << vertices[3 * (*it) + 2] << std::endl;
    }

    std::cout << std::endl;*/
}
