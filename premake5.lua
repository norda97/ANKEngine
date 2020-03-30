workspace "TechniqueSandbox"
    architecture "x64"
    language "C++"
    cppdialect "C++17"
    startproject "Engine"

    configurations
	{
		"Debug",
		"Release"
    }
    
    flags
	{
		"MultiProcessorCompile"
    }
    
    
	filter "configurations:Debug"
        defines "TS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "TS_RELEASE"
        runtime "Release"
        optimize "on"

OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.architecture}"

project "Engine"
    location "Engine"
    kind "WindowedApp"

    pchheader "pch.h"
    pchsource "Engine/src/pch.cpp"

    targetdir ("build/bin/" .. OUTPUT_DIR)
    objdir ("build/obj/" .. OUTPUT_DIR)

    files
    {
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
    }

    links
    {
        --"glfw3"
    }

    includedirs
    {
        "%{prj.name}/src",
        --"externals/glfw/include"
    }

    libdirs
    {
        --"externals/glfw/lib-vc2019",
    }
    
    filter "system:windows"
        systemversion "latest"