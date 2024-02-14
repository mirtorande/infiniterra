#include "terrain.h"

#include <compute_shader.h>
#include "GLErrors.h"

const unsigned int NUM_PATCH_PTS = 4;


Terrain::Terrain(const int xOffset, const int yOffset, const int size, unsigned res, float freq, std::vector<float> vertices) :
    terrainShader("shaders/terrain.vert", "shaders/terrain.frag", "shaders/terrain.tesc", "shaders/terrain.tese")
{
    resolution = res;

    // NEW GENERATION
    ComputeShader terrainGenerator("shaders/terrain_generator.comp");

    terrainShader.use();
    terrainShader.setInt("heightMap", 0);
    terrainShader.setFloat("heightFactor", 256.0f);
    terrainShader.setVec3("grassColor", GRASS_COLOR);
    terrainShader.setVec3("dryColor", DRY_COLOR);
    terrainShader.setVec3("snowColor", SNOW_COLOR);

    // Generate terrain

    GLCall(glGenTextures(1, &heightsTexture));
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, heightsTexture));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, NULL));
    
    GLCall(glBindImageTexture(0, heightsTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));

    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, heightsTexture));

    terrainGenerator.use();
    terrainGenerator.setFloat("xOffset", xOffset);
    terrainGenerator.setFloat("yOffset", yOffset);
    terrainGenerator.setFloat("size", size);
    terrainGenerator.setFloat("freq", freq);

    GLCall(glDispatchCompute((unsigned int)size / 5, (unsigned int)size / 5, 1));

    // make sure writing to image has finished before read
    GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
    
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind texture when done so we won't accidentily mess up our texture.
    GLCall(glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));

    // configure the cube's VAO (and terrainVBO)
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    // Risky(?), keep an eye on this
    //vert.clear();
}


void Terrain::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
    const float height, const glm::vec3& cameraPos,
    const int minTess, const int maxTess, const float minDist, const float maxDist, const float fogDist,
    const glm::vec3 grassColor, const glm::vec3 dryColor, const glm::vec3 snowColor, const glm::vec3 skyColor) {
    // be sure to activate shader when setting uniforms/drawing objects
    terrainShader.use();


    // view/projection transformations

    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);

    // world transformation
    terrainShader.setMat4("model", model);
    terrainShader.setVec3("lightPos", glm::vec3(100.0f, 200.0f, 100.0f));
    terrainShader.setVec3("camPos", cameraPos);

    // tessellation control
    terrainShader.setInt("minTessLevel", minTess);
    terrainShader.setInt("maxTessLevel", maxTess);
    terrainShader.setFloat("minDistance", minDist);
    terrainShader.setFloat("maxDistance", maxDist);

    // other uniforms
    terrainShader.setFloat("fogDistance", fogDist);
    terrainShader.setFloat("heightFactor", height);
    terrainShader.setVec3("grassColor", grassColor);
    terrainShader.setVec3("dryColor", dryColor);
    terrainShader.setVec3("snowColor", snowColor);
    terrainShader.setVec3("skyColor", skyColor);

    // render the terrain
    glBindTexture(GL_TEXTURE_2D, heightsTexture);
    glBindVertexArray(terrainVAO);

    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * resolution * resolution);
}