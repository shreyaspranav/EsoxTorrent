--[[
Premake Build Configuration for EsoxTorrent
----------------------------------------------------------------------------------------------------------
The Project currrently supports 32 bit(x86) and 64 bit(x86_64) architectures
--]]

bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "EsoxTorrent"
    platforms       { "x86", "x64" }
    configurations  { "Debug", "Release" }

project "EsoxTorrent"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" .. bin_folder)
    objdir ("bin/" .. bin_folder .. "/obj")
    cppdialect "C++17"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        targetsuffix "-d"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"
    


