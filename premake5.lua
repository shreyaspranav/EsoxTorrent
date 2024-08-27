--[[
Premake Build Configuration for EsoxTorrent
----------------------------------------------------------------------------------------------------------
The Project currrently supports 32 bit(x86) and 64 bit(x86_64) architectures
--]]

-- Name of the 'bin' folder
bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Declare all the paths here:
Dir = {}
Dir["GLFW_source"]      = "deps/glfw/src"
Dir["ImGui"]            = "deps/imgui"

Dir["GLFW_include"]             = "deps/glfw/include"
Dir["glad_include"]             = "deps/glad/include"
Dir["asio_include"]             = "deps/asio/include"
Dir["bencode_hpp_include"]      = "deps/bencode.hpp/include"

workspace "EsoxTorrent"
    platforms       { "x86", "x64" }
    configurations  { "Debug", "Release" }

project "EsoxTorrent"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" .. bin_folder)
    objdir ("bin/" .. bin_folder .. "/obj")
    cppdialect "C++20"

    files {
        -- The core project source files: --------------------
        "src/**.h",
        "src/**.cpp",

        -- GLFW source files: --------------------------------
        "%{Dir.GLFW_include}/GLFW/glfw3.h",
        "%{Dir.GLFW_include}/GLFW/glfw3native.h",

        "%{Dir.GLFW_source}/internal.h",
        "%{Dir.GLFW_source}/platform.h",
        "%{Dir.GLFW_source}/mappings.h",
        "%{Dir.GLFW_source}/context.c",
        "%{Dir.GLFW_source}/init.c",
        "%{Dir.GLFW_source}/input.c", 
        "%{Dir.GLFW_source}/monitor.c", 
        "%{Dir.GLFW_source}/platform.c", 
        "%{Dir.GLFW_source}/vulkan.c",
        "%{Dir.GLFW_source}/window.c",
        "%{Dir.GLFW_source}/egl_context.c", 
        "%{Dir.GLFW_source}/osmesa_context.c", 
        "%{Dir.GLFW_source}/null_platform.h",
        "%{Dir.GLFW_source}/null_joystick.h",
        "%{Dir.GLFW_source}/null_init.c",
        "%{Dir.GLFW_source}/null_monitor.c", 
        "%{Dir.GLFW_source}/null_window.c",
        "%{Dir.GLFW_source}/null_joystick.c",

        -- ImGui source files: ----------------------------------
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
        ---------------------------------------------------------
    }

    includedirs {
        "%{Dir.GLFW_include}",
        "%{Dir.glad_include}",
        "%{Dir.asio_include}",
        "%{Dir.bencode_hpp_include}",
        "%{Dir.ImGui}",

        -- Using the source directory as the include directory to avoid going back 
        -- a folder to import stuff
        "src"
    }

    filter "system:Windows"
        files {
            -- Include Windows only GLFW files -----------------
            "%{Dir.GLFW_source}/win32_time.h",
            "%{Dir.GLFW_source}/win32_thread.h", 
            "%{Dir.GLFW_source}/win32_module.c",
            "%{Dir.GLFW_source}/win32_time.c",
            "%{Dir.GLFW_source}/win32_thread.c",
            "%{Dir.GLFW_source}/win32_platform.h",
            "%{Dir.GLFW_source}/win32_joystick.h",
            "%{Dir.GLFW_source}/win32_init.c",
            "%{Dir.GLFW_source}/win32_joystick.c", 
            "%{Dir.GLFW_source}/win32_monitor.c",
            "%{Dir.GLFW_source}/win32_window.c",
            "%{Dir.GLFW_source}/wgl_context.c"
            ----------------------------------------------------
        }

        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }
        
        -- 
        links {
            "gdi32",  -- gdi32 is required by GLFW
            "opengl32"
        }

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

    -- Remove the console window and just use the GUI window in case of a Release Windows build
    filter { "system:Windows", "configurations:Release" }
        kind "WindowedApp"
