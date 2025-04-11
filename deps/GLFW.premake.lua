project "GLFW"
    kind "StaticLib"
    language "C"
    targetdir ("%{wks.location}/bin/" .. bin_folder)
    objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj")
    cdialect "C99"

    files {
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
        "%{Dir.GLFW_source}/null_joystick.c"
    }

    includedirs {
        "%{Dir.GLFW_include}"
    }

    filter "system:Windows"
        files {
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
        }

        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

        links {
            "gdi32"
        }

        staticruntime "On"

    filter "system:linux"
		pic "On"

		systemversion "latest"

		files
		{
            "%{Dir.GLFW_source}/x11_init.c",
            "%{Dir.GLFW_source}/x11_monitor.c",
            "%{Dir.GLFW_source}/x11_window.c",
            "%{Dir.GLFW_source}/x11_platform.h",
            "%{Dir.GLFW_source}/xkb_unicode.c",
            "%{Dir.GLFW_source}/xkb_unicode .h",
            "%{Dir.GLFW_source}/posix_module.c",
            "%{Dir.GLFW_source}/posix_poll.c",
            "%{Dir.GLFW_source}/posix_poll.h",
            "%{Dir.GLFW_source}/posix_thread.c",
            "%{Dir.GLFW_source}/posix_thread.h",
            "%{Dir.GLFW_source}/posix_time.h",
            "%{Dir.GLFW_source}/posix_time.c",
            "%{Dir.GLFW_source}/glx_context.c",
            "%{Dir.GLFW_source}/egl_context.c",
            "%{Dir.GLFW_source}/osmesa_context.c",
            "%{Dir.GLFW_source}/linux_joystick.c",
            "%{Dir.GLFW_source}/linux_joystick.h",
		}

		defines
		{
			"_GLFW_X11", "_GNU_SOURCE"
		}

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
