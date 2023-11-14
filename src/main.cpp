#include <iostream>
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
#include <window_io.h>
#include <VAO.h>
#include <EBO.h>

#include <skybox.h>
#include <terrain.h>


int main()
{
    GLFWwindow* window = initializeWindow();
    

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Terrain terrain = Terrain();
    Skybox sky = Skybox();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        // Terrain
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        terrain.Render(model, view, projection);

        // Skybox
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        sky.Render(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}