#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include "coords_cube.h"



#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.colors.vs", "1.colors.fs");

    Shader LightCube("1.light_cube.vs", "1.light_cube.fs");

    Shader Plane("plane.vs", "plane.fs");

    //Shader Trans("blending.vs", "blending.fs");

    Shader Skybox("cubemap.vs", "cubemap.fs");

    // load models
    // -----------

    //Model ourModel("uploads_files_2216833___Subaru-Loyale--OBJ/_Subaru-Loyale.obj");

    Model ourModel("N64 Logo/n64_logo.obj");

    //Model ourSecondModel ("")

    unsigned int LightVAO;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, siz_of_verts, &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    unsigned int PlaneVAO, PlaneVBO;
    glGenVertexArrays(1, &PlaneVAO);
    glGenBuffers(1, &PlaneVBO);
    glBindVertexArray(PlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, siz_of_plane, &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int CubemapVAO, CubemapVBO;
    glGenVertexArrays(1, &CubemapVAO);
    glGenBuffers(1, &CubemapVBO);
    glBindVertexArray(CubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, siz_of_skybox, &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    /*
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, siz_of_trans, transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    */

    unsigned int texture = loadTexture("sand_grey.png");

    unsigned int cubemapTexture = loadCubemap(faces);

    // unsigned int transtexture = loadTexture("blending_transparent_window.png");

    /*
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < vegetation.size(); i++)
    {
        float distance = glm::length(camera.Position - vegetation[i]);
        sorted[distance] = vegetation[i];
    }
    */

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // skybox cube
    Skybox.use();
    Skybox.setInt("skybox", 0);

    cout << cubemapTexture << std::endl;

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

        // don't forget to enable shader before setting uniforms
        ourShader.use();


        ourShader.setVec3("light.position", lightPosition);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

       // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.02f, 0.02f, 0.02f);
        ourShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.specular", 0.9f, 0.9f, 0.9f);
        // point light 1
        ourShader.setVec3("pointlights[0].position", pointlightpositions[0]);
        ourShader.setVec3("pointlights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointlights[0].diffuse", 0.08f, 0.08f, 0.08f);
        ourShader.setVec3("pointlights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointlights[0].constant", 1.0f);
        ourShader.setFloat("pointlights[0].linear", 0.09f);
        ourShader.setFloat("pointlights[0].quadratic", 0.032f);
        // point light 2
        ourShader.setVec3("pointlights[1].position", pointlightpositions[1]);
        ourShader.setVec3("pointlights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointlights[1].diffuse", 0.08f, 0.08f, 0.08f);
        ourShader.setVec3("pointlights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointlights[1].constant", 1.0f);
        ourShader.setFloat("pointlights[1].linear", 0.09f);
        ourShader.setFloat("pointlights[1].quadratic", 0.032f);
        // point light 3
        ourShader.setVec3("pointlights[2].position", pointlightpositions[2]);
        ourShader.setVec3("pointlights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointlights[2].diffuse", 0.08f, 0.08f, 0.08f);
        ourShader.setVec3("pointlights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointlights[2].constant", 1.0f);
        ourShader.setFloat("pointlights[2].linear", 0.09f);
        ourShader.setFloat("pointlights[2].quadratic", 0.032f);
        // point light 4
        ourShader.setVec3("pointlights[3].position", pointlightpositions[3]);
        ourShader.setVec3("pointlights[3].ambient", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("pointlights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointlights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointlights[3].constant", 1.0f);
        ourShader.setFloat("pointlights[3].linear", 0.09f);
        ourShader.setFloat("pointlights[3].quadratic", 0.032f);

        float time = glfwGetTime();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        LightCube.use();
        LightCube.setMat4("projection", projection);
        LightCube.setMat4("view", view);
        glBindVertexArray(LightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(pointlightpositions[i]));
            model = glm::scale(model, glm::vec3(0.4f));
            LightCube.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        
        Plane.use();
        Plane.setInt("texture1", 0);
        Plane.setMat4("projection", projection);
        Plane.setMat4("view", view);
        Plane.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(PlaneVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //Trans.use();
        //Trans.setInt("tranz", 0);
        //Trans.setMat4("projection", projection);
        //Trans.setMat4("view", view);
        // LEFT OFF HERE
        /*
        glBindVertexArray(transparentVAO);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            Trans.setMat4("model", model);
            glBindTexture(GL_TEXTURE_2D, transtexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        */

        glDepthFunc(GL_LEQUAL);
        Skybox.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from view matrix

        Skybox.setMat4("view", view);
        Skybox.setMat4("projection", projection);

        // DRAW skybox cube
        glBindVertexArray(CubemapVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap Texture Failed to Load Path" << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;

}