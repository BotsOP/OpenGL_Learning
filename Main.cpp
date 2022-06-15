
#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "Shader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void loadFromFile(const char* url, char** target) {
    std::ifstream stream(url, std::ios::binary);

    stream.seekg(0, stream.end);
    int total = stream.tellg();
    *target = new char[total + 1];

    stream.seekg(0, stream.beg);
    stream.read(*target, total);

    (*target)[total] = '\0';

    stream.close();
}

unsigned int loadTexture(std::string url, GLenum format) {
    // gen & bind IDs
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // instellen texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // shit inladen
    int width, height, channels;
    unsigned char* data;
    data = stbi_load(url.c_str(), &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cout << "Error loading file: " << url.c_str() << std::endl;
    }
    else {
        // shit aan opengl geven
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

int main()
{
    std::cout << "Hello World2!\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800;
    int height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello OpenGL!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, width, height);

    ///SETUP SHADER PROGRAM///

    Shader ourShader("vertexShader.shader", "fragmentShader.shader");

    //Image and depth settings
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    //Model setup
    Model backpack("backpack/pillar.obj");

    //Image setup
    unsigned int heightMapID = loadTexture("backpack/displacementPillar.jpg", GL_RGB);

    ourShader.use();

    glUniform1i(glGetUniformLocation(ourShader.ID, "heightMap"), heightMapID);
    glActiveTexture(GL_TEXTURE0 + 5); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, heightMapID);

    /// MATRIX SETUP ///

    int worldLoc = glGetUniformLocation(ourShader.ID, "world");
    int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    int projLoc = glGetUniformLocation(ourShader.ID, "projection");

    /// END MATRIX SETUP ///

    // OPENGL SETTINGS //

    /*glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);*/

    while (!glfwWindowShouldClose(window)) {
        

        float t = glfwGetTime();

        ourShader.use();
        glUniform1f(glGetUniformLocation(ourShader.ID, "time"), t);

        float r = (float)sin(t * 1.1f);
        float g = (float)sin(t * 1.3f);
        float b = (float)sin(t * 1.7f);

        glm::mat4 world = glm::mat4(1.f);
        world = glm::rotate(world, glm::radians((float)t * 45.0f), glm::vec3(0, 1, 0));
        world = glm::scale(world, glm::vec3(1, 1, 1));
        world = glm::translate(world, glm::vec3(0, 0, 1));

        glm::mat4 view = glm::lookAt(glm::vec3(0, 10, -10), glm::vec3(0, 3, 0), glm::vec3(0, 3, 0));
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), width / (float)height, 0.1f, 100.0f);

        glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // iets tekenen
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, heightMapID);
        
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        backpack.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
