project "PhysicsEngine"
    kind "ConsoleApp"
    language "C++"

    -- Directories where the compiler should look for source files
    files 
    { 
        "**.h", 
        "**.cpp",
        "glad.c"--"**.c" --glad.c
    }
    

    -- to ensures the root of Project1 is included
    includedirs { "." }

