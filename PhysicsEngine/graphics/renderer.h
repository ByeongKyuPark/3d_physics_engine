#pragma once

#include "shader.h"
#include "shape.h"
#include "math/vector3.h"
#include "simulator/object.h"
#include "textureImage.h"
#include <glfw3.h>
#include <unordered_map>
#include <array>
#include <memory>//unique_ptr

class CameraManager;

namespace graphics
{
    constexpr float GRID_SCALE = 50.0f;
    constexpr std::array<float,30> GROUND_VERTICES = {
        // Positions                      // Texture Coords <-- 50X50 grids
        -GRID_SCALE, 0.0f, -GRID_SCALE,        0.0f,   50.0f,
         GRID_SCALE, 0.0f, -GRID_SCALE,        50.0f , 50.0f,
         GRID_SCALE, 0.0f,  GRID_SCALE,        50.0f , 0.0f,
        -GRID_SCALE, 0.0f, -GRID_SCALE,        0.0f  , 50.0f,
         GRID_SCALE, 0.0f,  GRID_SCALE,        50.0f , 0.0f,
        -GRID_SCALE, 0.0f,  GRID_SCALE,        0.0f  ,  0.0f
    };

    constexpr std::array<float,6> CONTACT_NORMAL_VERTICES = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    constexpr int WINDOW_WIDTH = 1280;//1280
    constexpr int WINDOW_HEIGHT = 720;//720
    constexpr float SCENE_RATIO = 0.82f;
    //const int SCENE_WIDTH = 1024;//1024
    //const int SCENE_HEIGHT = 576;//576


    const float PERSPECTIVE_NEAR = 0.1f;
    const float PERSPECTIVE_FAR = 300.0f;

    class Renderer
    {
    public:
        constexpr static std::array<const char*, TextureID::NUM_TEXTURES> ImagePaths{
            "PhysicsEngine/images/bricks.png", "PhysicsEngine/images/face.png", "PhysicsEngine/images/jeans.png",
            "PhysicsEngine/images/baloons.png","PhysicsEngine/images/fireworks.jpg","PhysicsEngine/images/pause.png",
            "PhysicsEngine/images/play.png","PhysicsEngine/images/cube.png","PhysicsEngine/images/sphere.png"
        };

        typedef std::unordered_map<RigidObject*, std::unique_ptr<Shape>> Shapes;

    private:
        GLFWwindow *window;
        CameraManager& cameraManager;

        int sceneWidth, sceneHeight;

        Shader objectShader;
        Shader contactInfoShader;

        GLuint textures[NUM_TEXTURES];

        Shapes shapes;

        unsigned int backgroundVAO;
        unsigned int worldYaxisVAO;
        unsigned int sceneFrameBufferID;
        unsigned int textureBufferID;

        void SetupTextures();
        void SetFullscreenWindow(GLFWwindow*);
        void AddDebugSphere();//renders ContactInfo

    public:
        Renderer(CameraManager& camManager,const char* title);
        ~Renderer();

        // Public Interface
        void RenderObject(RigidObject* obj);
        void RenderGround();
        void RenderCollisionContacts(const math::Vector3& pos, const math::Vector3& normal);
        void RenderWorldAxisAt(int axisIdx, float posX, float posY, float posZ);
        void RenderObjectAxis(int axisIdx, float modelMatrix[]);

        GLFWwindow* GetWindow() const;
        unsigned int GetTextureBufferID() const;
        unsigned* GetTextures() { return static_cast<unsigned*>(textures); }

        void AddGraphicalShape(RigidObject*);
        void RemoveShape(RigidObject* obj);

        void BindSceneFrameBuffer();
        void BindDefaultFrameBuffer();

        void SetSceneViewport();
        void Clear() const;

        //for ray casting
        glm::vec3 ConvertScreenToWorld(glm::vec2 screenPt);
    };
}