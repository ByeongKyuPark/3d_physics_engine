#include "glad/glad.h"
#include "renderer.h"
#include "simulator/cameraManager.h"
#include <iostream>
#include <typeinfo>

#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 26819)
#pragma warning(disable : 6262)
#pragma warning(disable : 26451)
#include "stb_image.h"
#pragma warning(pop)

using namespace graphics;

void Renderer::SetupTextures()
{
	glGenTextures(NUM_TEXTURES, textures);
    for (int i = 0; i < NUM_TEXTURES; ++i) {
        int imgWidth, imgHeight, numComponents;
        unsigned char* imgData = stbi_load(ImagePaths[i], &imgWidth, &imgHeight, &numComponents, 0);
        if (!imgData)
        {
            std::cerr << stbi_failure_reason() << std::endl;
            exit(1);
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glEnable(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        if (numComponents == 2)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, imgWidth, imgHeight, 0,
                GL_RG, GL_UNSIGNED_BYTE, imgData);
        else if (numComponents == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, imgWidth, imgHeight, 0,
                GL_RGB, GL_UNSIGNED_BYTE, imgData);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgWidth, imgHeight, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, imgData);

        stbi_image_free(imgData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::SetFullscreenWindow(GLFWwindow* window) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowSize(window, mode->width, mode->height);
    glfwSetWindowPos(window, 0, 0);
}

void graphics::Renderer::AddDebugSphere() {
    shapes[nullptr] = std::make_unique<Sphere>();//debug sphere
}

Renderer::Renderer(CameraManager& camManager,const char* title)
    :cameraManager{camManager}, 
    sceneWidth(static_cast<int>(WINDOW_WIDTH* SCENE_RATIO)), 
    sceneHeight(static_cast<int>(WINDOW_HEIGHT* SCENE_RATIO)), textures{ NUM_TEXTURES }
{
    if (!glfwInit()) {
        throw std::runtime_error("Renderer::glfwInit() error");
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);
    if (!window){
        glfwTerminate();
        throw std::runtime_error("Renderer::glfwCreateWindow() error");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //wait for device's vertical sync for front and back buffers to be swapped
    SetFullscreenWindow(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwTerminate();
        throw std::runtime_error("Renderer::gladLoadGLLoader() error");
    }

	objectShader = Shader({
		{ "PhysicsEngine/shaders/object.vert",GL_VERTEX_SHADER},
		{"PhysicsEngine/shaders/object.frag",GL_FRAGMENT_SHADER }
		});
    contactInfoShader = Shader({
        {"PhysicsEngine/shaders/frame.vert",GL_VERTEX_SHADER},
        {"PhysicsEngine/shaders/frame.frag",GL_FRAGMENT_SHADER}
        });

    glGenVertexArrays(1, &backgroundVAO);
    glBindVertexArray(backgroundVAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * GROUND_VERTICES.size(),&GROUND_VERTICES[0],GL_STATIC_DRAW);

    //(0) position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//(1) texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // world y-axis
    glGenVertexArrays(1, &worldYaxisVAO);
    glBindVertexArray(worldYaxisVAO);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CONTACT_NORMAL_VERTICES.size(), &CONTACT_NORMAL_VERTICES[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //frame buffer
    glGenFramebuffers(1, &sceneFrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBufferID);

    //textuer buffer
    glGenTextures(1, &textureBufferID);
    glBindTexture(GL_TEXTURE_2D, textureBufferID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBufferID, 0);

    unsigned int renderBufferID;
    glGenRenderbuffers(1, &renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sceneWidth, sceneHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Renderer::glCheckFramebufferStatus error");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SetupTextures();
    AddDebugSphere();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

}

Renderer::~Renderer(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Renderer::GetWindow() const
{
    return window;
}

unsigned int Renderer::GetTextureBufferID() const
{
    return textureBufferID;
}

void Renderer::AddGraphicalShape(RigidObject* obj)
{
    if (dynamic_cast<SphereObject*>(obj) != nullptr ) {
        shapes[obj] = std::make_unique<Sphere>();
        obj->SetShape(shapes[obj].get());
    }
    else if (dynamic_cast<BoxObject*>(obj) != nullptr) {
        shapes[obj] = std::make_unique<Box>();
        obj->SetShape(shapes[obj].get());
    }
    else {
        throw std::runtime_error("Renderer::addGraphicalShape, invalid object pointer");
    }
}

void Renderer::RemoveShape(RigidObject* obj){
    Shapes::iterator shapeIter = shapes.find(obj);
    shapes.erase(shapeIter);
}

void Renderer::RenderObject(RigidObject* obj)
{
    glm::mat4 view = cameraManager.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    objectShader.Bind();
    objectShader.SetMat4("model", obj->GetRigidBody()->GetLocalToWorldMatrix().ConvertToGLM());
    objectShader.SetMat4("view", view);
    objectShader.SetMat4("projection", projection);
    objectShader.SetVec3("viewPos", cameraManager.GetCameraPosition());

    Shape *objectShape = shapes.find(obj)->second.get();
    objectShader.SetInt("texture1", objectShape->textureID);
    glBindVertexArray(objectShape->polygonVAO);
    glDrawElements(GL_TRIANGLES, objectShape->polygonIndices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

void Renderer::RenderGround()
{
    glm::mat4 model(1.0f);
    //objectShader.SetMat4("model", model);
    glm::mat4 view = cameraManager.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    objectShader.Bind();
    objectShader.SetMat4("model", model);
    objectShader.SetMat4("view", view);
    objectShader.SetMat4("projection", projection);
    objectShader.SetVec3("objectColor", glm::vec3(0.1f, 0.2f, 0.4f));
    objectShader.SetVec3("viewPos", cameraManager.GetCameraPosition());
    objectShader.SetInt("texture1", 0);//just the first texture (temp)

    glBindVertexArray(backgroundVAO);
	objectShader.SetVec3("objectColor", glm::vec3(0.35f, 0.35f, 0.35f));
	glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void Renderer::RenderCollisionContacts(const math::Vector3& pos, const math::Vector3& normal)
{
    glm::mat4 view = cameraManager.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::GRID_SCALE(model, glm::vec3(0.1f, 0.1f, 0.1f));

    contactInfoShader.Bind();
    contactInfoShader.SetMat4("model", model);
    contactInfoShader.SetMat4("view", view);
    contactInfoShader.SetMat4("projection", projection);
    contactInfoShader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    contactInfoShader.SetVec3("viewPos", cameraManager.GetCameraPosition());

    Shape *objectShape = shapes.find(nullptr)->second.get();
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(objectShape->polygonVAO);
    glDrawElements(GL_TRIANGLES, objectShape->polygonIndices.size(), GL_UNSIGNED_INT, (void*)0);
    
    glm::vec3 defaultNormal(0.0f, 1.0f, 0.0f);
    glm::vec3 contactNormal(normal.x, normal.y, normal.z);
    glm::vec3 rotateAxis;
    if (glm::dot(defaultNormal, contactNormal) > 0.0f)
        rotateAxis = glm::cross(defaultNormal, contactNormal);
    else
        rotateAxis = glm::cross(contactNormal, defaultNormal);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::GRID_SCALE(model, glm::vec3(0.5f, 0.5f, 0.5f));
    if (glm::length(rotateAxis) != 0.0f)
    {
        model = glm::Rotate(
            model,
            glm::asin(glm::length(rotateAxis)),
            glm::normalize(rotateAxis)
        );
    }
    contactInfoShader.SetMat4("model", model);

    glBindVertexArray(worldYaxisVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(0);
}

void Renderer::RenderWorldAxisAt(int axisIdx, float posX, float posY, float posZ)
{
    glm::mat4 view = cameraManager.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(posX, posY, posZ));
    switch (axisIdx)
    {
    case 0:  // x
        model = glm::Rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        break;

    case 1:  // y
        break;

    case 2:  // z
        model = glm::Rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        break;

    default:
        break;
    }
    
    contactInfoShader.Bind();
    contactInfoShader.SetMat4("model", model);
    contactInfoShader.SetMat4("view", view);
    contactInfoShader.SetMat4("projection", projection);
    contactInfoShader.SetVec3("objectColor", glm::vec3{ 1.0f, 1.0f, 1.0f });//white
    contactInfoShader.SetVec3("viewPos", cameraManager.GetCameraPosition());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(worldYaxisVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderObjectAxis(int axisIdx, float modelMatrix[])
{
    glm::mat4 view = cameraManager.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 model = glm::make_mat4(modelMatrix);
    glm::vec3 color(0.0f, 0.0f, 0.0f);
    switch (axisIdx)
    {
    case 0:  // x 
        model = glm::Rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        color.x = 1.0f;
        break;

    case 1:  // y
        color.y = 1.0f;
        break;

    case 2:  // z
        model = glm::Rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        color.z = 1.0f;
        break;

    default:
        break;
    }

    contactInfoShader.Bind();
    contactInfoShader.SetMat4("model", model);
    contactInfoShader.SetMat4("view", view);
    contactInfoShader.SetMat4("projection", projection);
    contactInfoShader.SetVec3("objectColor", color);
    contactInfoShader.SetVec3("viewPos", cameraManager.GetCameraPosition());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(worldYaxisVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void graphics::Renderer::Clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::BindSceneFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBufferID);
}

void Renderer::BindDefaultFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetSceneViewport(){
    glViewport(0, 0, sceneWidth, sceneHeight);
}

glm::vec3 Renderer::ConvertScreenToWorld(glm::vec2 screenPt){
    glm::mat4 projectionMat = glm::perspective(
        glm::radians(cameraManager.GetFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 viewMat = cameraManager.GetViewMatrix();
    glm::mat4 inverseTransformMat = glm::Inverse(projectionMat * viewMat);

    //viewport space ->NDC space
    glm::vec4 clickedPt;
    clickedPt.w = 1.0f;
    clickedPt.x = screenPt.x / sceneWidth * 2.0f - 1.0f;
    clickedPt.y = screenPt.y / sceneHeight * 2.0f - 1.0f;
    clickedPt.y *= -1.0f;//In screen coordinates, increasing y goes downward, but in NDC, increasing y goes upward
    clickedPt.z = -1.0f; //near plane

    glm::vec4 worldPt = inverseTransformMat * clickedPt;
    worldPt.w = 1.0f / worldPt.w;
    worldPt.x *= worldPt.w;
    worldPt.y *= worldPt.w;
    worldPt.z *= worldPt.w;

    return glm::vec3(worldPt.x, worldPt.y, worldPt.z);
}
