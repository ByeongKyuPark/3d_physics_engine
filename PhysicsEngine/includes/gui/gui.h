#ifndef GUI_H
#define GUI_H
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "simulator/object.h"
#include "simulator/event.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>//unique_ptr
#include <string>

namespace gui
{
    class GUI
    {
        //typedef std::vector<RigidObject*> std::vector<RigidObject*>;

        static constexpr float MAIN_SCREEN_WIDTH_PERCENTAGE = 0.8f;
        static constexpr float SETTINGS_SCREEN_WIDTH_PERCENTAGE = 0.2f;

    private:
        unsigned int textureBufferID;
        bool shouldShowLoadPopup;
        bool isObjectAttributeWindowFocused;
        bool isObjectDetailsOpen;// Flag to track whether the objectDetails window is open or closed.

    public:
        GUI(GLFWwindow* window, unsigned int textureBufferID);

        //void updateWindowSize();
        void renderAll(
            unsigned* textures,
             std::queue<std::unique_ptr<Event>>& eventQueue,
            //EventQueue&,
            const std::vector<RigidObject*>&,
            const bool& isRunning,
            const std::vector<RigidObject*>& selectedObjectIDs
        );

    private:
        void showLoadMapPopup(std::queue<std::unique_ptr<Event>>& eventQueue);

        void renderScene(
            ImGuiWindowFlags,
             std::queue<std::unique_ptr<Event>>& eventQueue
            //EventQueue&,
            //const bool& isRunning
        );
        void renderContentBrowser(unsigned* textures,ImGuiWindowFlags, std::queue<std::unique_ptr<Event>>& eventQueue);
        void renderSimulationControl(
            unsigned* textures,
            ImGuiWindowFlags,
             std::queue<std::unique_ptr<Event>>& eventQueue,
            const bool& isRunning
        );
        void renderObjectList(
            ImGuiWindowFlags,
             std::queue<std::unique_ptr<Event>>& eventQueue,
            const std::vector<RigidObject*>&
        );
        void renderObjectDetails(
            unsigned* textures,
            ImGuiWindowFlags,
            std::queue<std::unique_ptr<Event>>& eventQueue,
            const std::vector<RigidObject*>&,
            const std::vector<RigidObject*>& selectedObjectIDs,
            const bool& isRunning
        );
        void renderObjectAttribute(
            unsigned* textures,
            ImGuiWindowFlags,
             std::queue<std::unique_ptr<Event>>& eventQueue,
            const std::vector<RigidObject*>&,
            const std::vector<RigidObject*>& selectedObjectIDs,
            const bool& isRunning
        );
        void renderAttributeDetails(ImGuiWindowFlags, std::queue<std::unique_ptr<Event>>& eventQueue);
        void renderWorldSettings(ImGuiWindowFlags, std::queue<std::unique_ptr<Event>>& eventQueue);

        bool isInMainSceneWindow(ImVec2);
    };
} // namespace gui


#endif // GUI_H
