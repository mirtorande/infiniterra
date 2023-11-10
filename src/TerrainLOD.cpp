#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <texture.h>
#include <cubemap.h>
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <WindowIO.h>
#include <skybox.h>
#include <VAO.h>
#include <EBO.h>

#include <iostream>

float getHeightFromImage(const unsigned char* data, int width, int x, int y) {
    // Assuming the image is grayscale, so we take only one channel
    return static_cast<float>(data[y * width + x]) / 255.0f;
}

void generateTerrain(std::vector<float>& vertices, const unsigned char* data, int width, int height, float scale) {
    // Generate vertices for a grid
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float xPos = static_cast<float>(x) * scale;
            float yPos = getHeightFromImage(data, width, x, y) * scale;
            float zPos = static_cast<float>(y) * scale;

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }
}

int main()
{
    GLFWwindow* window = initializeWindow();

    // configure global opengl state
    // -----------------------------
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    
    //Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");

    Skybox sky = Skybox();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        
        // draw skybox
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        sky.Render(view, projection);

        // glfw: swap buffers and poll IO events
        // --------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glfwTerminate();
    return 0;
}