workspace "MiniEngine"
    architecture "x64"
    language "C++"
    cppdialect "C++17"
    startproject "Sandbox"

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
        defines "MINI_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "MINI_RELEASE"
        runtime "Release"
        optimize "on"

OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.architecture}"

includeList = {}
includeList["XTK"] = "packages/directxtk_desktop_2017.2020.6.2.1/include"  -- DirecXTK

project "Engine"
    location "Engine"
    kind "StaticLib"


    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    targetdir ("build/bin/" .. OUTPUT_DIR .. "/%{prj.name}")
    objdir ("build/obj/" .. OUTPUT_DIR .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
    }

    links
    {
        "D3DCompiler",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{includeList.XTK}",
    }

    removefiles
    {
        "%{prj.name}/src/Core/main.cpp",
    }

    libdirs
    {
        --"Externals/glfw/lib-vc2019",
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "MINI_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "MINI_RELEASE"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "WindowedApp"
    language "C++"

    -- pchheader "%{prj.name}/src/pch.h"
    -- pchsource "%{prj.name}/src/pch.cpp"

    targetdir ("Build/bin/" .. OUTPUT_DIR .. "/%{prj.name}")
    objdir ("Build/obj/" .. OUTPUT_DIR .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "Engine/src/Core/main.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{includeList.XTK}", 
        "Engine/src",
    }

    links 
    {
        "Engine",
        "D3DCompiler"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "MINI_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "MINI_RELEASE"
        runtime "Release"
        optimize "on"