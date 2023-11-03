workspace "PhysicsEngineWorkspace"
    configurations { "Debug", "Release" }
    architecture "x86_64" 
    cppdialect "C++17"
    startproject "PhysicsEngine"
    location "." --build

    -- Global defines
    defines { "IMGUI_IMPL_OPENGL_LOADER_GLAD" }

    -- External dependencies
    includedirs 
    { 
        "PhysicsEngine/includes",
        "PhysicsEngine/includes/glad",
        "PhysicsEngine/includes/GLFW",
        "PhysicsEngine/includes/gui",
        "PhysicsEngine/includes/imgui",
        "PhysicsEngine/includes/KHR",
        "PhysicsEngine/includes/opengl",
        "PhysicsEngine/includes/opengl/glm",
        "PhysicsEngine/includes/stb_image",
        "PhysicsEngine/includes/json",
    }

    libdirs { "PhysicsEngine/libraries" }
    -- Since you're using GLFW (which likely has a .lib file), link against it
    links { "glfw3", "opengl32" }

    -- Configuration Specific Settings
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    -- Projects
    include "PhysicsEngine"
    include "Matrix-Test"
