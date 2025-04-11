project "ImGui"
    kind "StaticLib"
    language "C++"
    targetdir ("%{wks.location}/bin/" .. bin_folder)
    objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj")
    cppdialect "C++17"

    files {
        "%{Dir.ImGui}/imgui.h",
        "%{Dir.ImGui}/imgui_internal.h",
        "%{Dir.ImGui}/backends/imgui_impl_opengl2.h",
        "%{Dir.ImGui}/backends/imgui_impl_glfw.h",

        "%{Dir.ImGui}/imgui.cpp",
        "%{Dir.ImGui}/imgui_demo.cpp",
        "%{Dir.ImGui}/imgui_draw.cpp",
        "%{Dir.ImGui}/imgui_tables.cpp",
        "%{Dir.ImGui}/imgui_widgets.cpp",
        "%{Dir.ImGui}/backends/imgui_impl_opengl2.cpp",
        "%{Dir.ImGui}/backends/imgui_impl_glfw.cpp",
    }

    includedirs {
        "%{Dir.GLFW_include}",
        "%{Dir.ImGui}"
    }

    filter "system:Windows"
        staticruntime "On"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

        -- Add -d suffix to identify a debug build
        targetsuffix "-d"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
    filter "platforms:x86"
        architecture "x86"
        
    filter "platforms:x64"
        architecture "x86_64"
