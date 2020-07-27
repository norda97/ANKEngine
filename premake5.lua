workspace "AnkaEngine"
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
        defines "ANK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ANK_RELEASE"
        runtime "Release"
        optimize "on"

OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.architecture}"

includeList = {}
includeList["XTK"] = "packages/directxtk_desktop_2017.2020.6.2.1/include/"  -- DirecXTK
includeList["Assimp"] = "Externals/Assimp/include/"  -- Assimp
includeList["stbi"] = "Externals/STBI/include/"  -- stbi
includeList["imgui"] = "Externals/imgui/imgui-master/"  -- imgui

project "IMGUI"
    location "Externals/imgui"
    kind "StaticLib"

    targetdir ("build/bin/" .. OUTPUT_DIR .. "/%{prj.name}")
    objdir ("build/obj/" .. OUTPUT_DIR .. "/%{prj.name}")

    includedirs
    {
        "%{includeList.imgui}",
    }

    files
    {
        includeList["imgui"] .. "/examples/imgui_impl_win32.cpp",
        includeList["imgui"] .. "/examples/imgui_impl_win32.h",
        includeList["imgui"] .. "/examples/imgui_impl_dx11.cpp",
        includeList["imgui"] .. "/examples/imgui_impl_dx11.h",
        includeList["imgui"] .. "imconfig.h",
        includeList["imgui"] .. "imgui.h",
        includeList["imgui"] .. "imgui.cpp",
        includeList["imgui"] .. "imgui_draw.cpp",
        includeList["imgui"] .. "imgui_internal.h",
        includeList["imgui"] .. "imgui_widgets.cpp",
        includeList["imgui"] .. "imstb_rectpack.h",
        includeList["imgui"] .. "imstb_textedit.h",
        includeList["imgui"] .. "imstb_truetype.h",
        includeList["imgui"] .. "imgui_demo.cpp",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "ANK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ANK_RELEASE"
        runtime "Release"
        optimize "on"


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
        -- "assimp-vc140-mt"
        "assimp-vc140-mt",
        "IMGUI",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{includeList.XTK}",
        "%{includeList.Assimp}",
        "%{includeList.stbi}",
        "%{includeList.imgui}",
    }

    removefiles
    {
        "%{prj.name}/src/Core/main.cpp",
    }

    libdirs
    {
        "Externals/Assimp/lib"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "ANK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ANK_RELEASE"
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
        "%{includeList.Assimp}",
        "%{includeList.stbi}",
        "%{includeList.imgui}",
        "Engine/src",
    }

    links 
    {
        "Engine",
        "D3DCompiler",
        "assimp-vc140-mt",
        "IMGUI",
    }

    libdirs
    {
        "Externals/Assimp/lib",
        "build/bin/" .. OUTPUT_DIR .. "/IMGUI"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "ANK_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ANK_RELEASE"
        runtime "Release"
        optimize "on"