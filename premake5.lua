--[[
Premake Build Configuration for EsoxTorrent
----------------------------------------------------------------------------------------------------------
The Project currrently supports 32 bit(x86) and 64 bit(x86_64) architectures
--]]

-- Version of EsoxTorrent.
esox_version_string = "0.0.1-beta"
esox_version_major = 0
esox_version_minor = 0
esox_version_patch = 1
esox_version_beta = true

-- Name of the 'bin' folder
bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Declare all the paths here:
Dir = {}
Dir["GLFW_source"]              = "%{wks.location}/deps/glfw/src"
Dir["ImGui"]                    = "%{wks.location}/deps/imgui"

Dir["GLFW_include"]             = "%{wks.location}/deps/glfw/include"
Dir["glad_include"]             = "%{wks.location}/deps/glad/include"
Dir["asio_include"]             = "%{wks.location}/deps/asio/include"
Dir["bencode_hpp"]              = "%{wks.location}/deps/bencode.hpp/include"


workspace "EsoxTorrent"
    platforms       { "x86", "x64" }
    configurations  { "Debug", "Release", "Production" }
    
    -- Include the build scripts for dependencies
    include "deps/GLFW.premake.lua"
    include "deps/ImGui.premake.lua"

project "EsoxTorrent"
    kind "ConsoleApp"
    language "C++"
    targetdir ("%{wks.location}/bin/" .. bin_folder)
    objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj")
    cppdialect "C++20"

    staticruntime "On"

    files {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{Dir.GLFW_include}",
        "%{Dir.glad_include}",
        "%{Dir.asio_include}",
        "%{Dir.bencode_hpp}",
        "%{Dir.ImGui}",

        -- Using the source directory as the include directory to avoid going back 
        -- a folder to import stuff
        "src"
    }

    defines {
        "ESOX_VERSION_STRING=\"%{esox_version_string}\"",
        "ESOX_VERSION_MAJOR=%{esox_version_major}",
        "ESOX_VERSION_MINOR=%{esox_version_minor}",
        "ESOX_VERSION_PATCH=%{esox_version_patch}",
        "ESOX_VERSION_BETA=%{esox_version_beta}"
    }

    links {
        "GLFW", "ImGui"
    }

    filter "system:Windows"
        openssl_path = os.getenv("OPENSSL_PATH")
        
        includedirs {
            "%{openssl_path}/include"
        }
        libdirs "%{openssl_path}/lib/VC/x64/MT"
        links { 
            "opengl32",
            "libcrypto", 
            "libssl"
        }

    filter { "system:Windows", "action:vs*" }
        defines "_CRT_SECURE_NO_WARNINGS"

    filter "system:linux"
        links {
            "GL",
            "ssl", "crypto" -- For OpenSSL
        }
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

        -- Add -d suffix to identify a debug build
        targetsuffix "-d"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

        targetsuffix "-r"

    filter "configurations:Production"
        defines { "NDEBUG", "PRODUCTION" }
        optimize "On"

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

    -- Remove the console window and just use the GUI window in case of a Production Windows build
    filter { "system:Windows", "configurations:Production" }
        kind "WindowedApp"
