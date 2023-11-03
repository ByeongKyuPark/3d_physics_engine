project "Matrix-Test"
    kind "ConsoleApp"
    language "C++"

    -- files to compile
    files 
    { 
        "**.h", 
        "**.cpp",
        "../googletest/src/gtest-all.cc"   --test impl
    }

    -- Google Test dirs
    includedirs 
    { 
        "../googletest",
        "../googletest/include"
    }
     
    -- includes root of Matrix-Test
    includedirs { "." }