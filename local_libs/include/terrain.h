#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "shader.h"

#define GRASS_COLOR glm::vec3(0.2, 0.3, 0.1)
#define DRY_COLOR glm::vec3(0.5, 0.4, 0.3)
#define SNOW_COLOR glm::vec3(1.0, 1.0, 1.0)
#define SKY_COLOR glm::vec3(0.6, 0.6, 0.7)

class Terrain {
private:
    unsigned resolution, terrainVAO, terrainVBO, heightsTexture;
    Shader terrainShader;
    std::vector<float> vertices;

public:
    // Constructor
    Terrain(const int xOffset, const int yOffset, const int size, unsigned res, float freq, std::vector<float> vertices);
    // Copy Constructor
    Terrain(Terrain& o) = delete;
    // Move Constructor
    inline Terrain(Terrain&& o) noexcept : terrainShader(o.terrainShader) {
        if (this != &o) {
            resolution = o.resolution;
            terrainVAO = o.terrainVAO;
            terrainVBO = o.terrainVBO;
            heightsTexture = std::move(o.heightsTexture);
            vertices = std::move(o.vertices);

            // Unbind the VAO and VBO from the other object
            o.terrainVAO = 0;
            o.terrainVBO = 0;
        }
    }
    // Copy Operator
    Terrain& operator=(const Terrain&) = delete;
    // Move Operator
    Terrain& operator=(Terrain&& o) noexcept {
        if (this != &o) {
			resolution = o.resolution;
			terrainVAO = o.terrainVAO;
			terrainVBO = o.terrainVBO;
			heightsTexture = std::move(o.heightsTexture);
			vertices = std::move(o.vertices);

			// Unbind the VAO and VBO from the other object
			o.terrainVAO = 0;
			o.terrainVBO = 0;
		}
		return *this;
	}
    // Destructor
    ~Terrain();

    void Render(
        const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
        const float height = 256.0, const glm::vec3& cameraPos = glm::vec3(0.0f, 0.0f, 0.0f),
        const int minTess = 1, const int maxTess = 64, const float minDist = 40.0f, const float maxDist = 1600.0f, const float fogDist = 5000.0f,
        const glm::vec3 grassColor = GRASS_COLOR, const glm::vec3 dryColor = DRY_COLOR, const glm::vec3 snowColor = SNOW_COLOR, const glm::vec3 skyColor = SKY_COLOR
        );

};

#endif // !TERRAIN_H
