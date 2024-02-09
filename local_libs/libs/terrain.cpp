#include "terrain.h"

#include <compute_shader.h>
#include "GLErrors.h"

const unsigned int NUM_PATCH_PTS = 4;


Terrain::Terrain(const int width, const int height, std::vector<float> vertices, unsigned res) :
    tessHeightMapShader("shaders/terrain.vert", "shaders/terrain.frag", "shaders/terrain.tessc", "shaders/terrain.tesse")
{

    resolution = res;

    // NEW GENERATION
    ComputeShader computeShader("shaders/compute_shader.comp");

    tessHeightMapShader.use();
    tessHeightMapShader.setInt("heightMap", 0);

    // Generate terrain

    GLCall(glGenTextures(1, &heightsTexture));
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, heightsTexture));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL));
    
    GLCall(glBindImageTexture(0, heightsTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F));

    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, heightsTexture));

    computeShader.use();
    computeShader.setFloat("t", 0);
    GLCall(glDispatchCompute((unsigned int)width / 10, (unsigned int)height / 10, 1));

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
    vertices.clear();
}

void Terrain::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    // be sure to activate shader when setting uniforms/drawing objects
    tessHeightMapShader.use();


    // view/projection transformations

    tessHeightMapShader.setMat4("projection", projection);
    tessHeightMapShader.setMat4("view", view);

    // world transformation
    tessHeightMapShader.setMat4("model", model);
    tessHeightMapShader.setVec3("lightPos", glm::vec3(100.0f, 200.0f, 100.0f));
    tessHeightMapShader.setVec3("camPos", cameraPos);

    // render the terrain
    glBindTexture(GL_TEXTURE_2D, heightsTexture);
    glBindVertexArray(terrainVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * resolution * resolution);
}
