add_rules("mode.debug", "mode.release")

-- Set C++ standard
set_languages("c++17")

-- Add packages
add_requires("imgui", "glfw", "stb", "nlohmann_json")

target("caithe")
    set_kind("binary")
    add_files("src/*.cpp", "src/*/*.cpp", "src/imgui/backends/*.cpp")
    
    -- Add packages
    add_packages("imgui", "glfw", "stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("GL", "GLU", "X11", "Xrandr", "Xinerama", "Xcursor", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_core_functionality_simple")
    set_kind("binary")
    add_files("Tests/test_core_functionality_simple.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_real_wallpaper")
    set_kind("binary")
    add_files("Tests/test_real_wallpaper.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_mathematical_algorithms")
    set_kind("binary")
    add_files("Tests/test_mathematical_algorithms.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_core_functionality")
    set_kind("binary")
    add_files("Tests/test_core_functionality.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_wallpaper_integration")
    set_kind("binary")
    add_files("Tests/test_wallpaper_integration.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_cache_implementation")
    set_kind("binary")
    add_files("Tests/test_cache_implementation.cpp", "src/core/*.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("stb", "nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")

target("test_config_manager")
    set_kind("binary")
    add_files("Tests/test_config_manager.cpp", "src/utils/*.cpp")
    
    -- Add packages (without imgui for testing)
    add_packages("nlohmann_json")
    
    -- Add system libraries for Linux
    add_syslinks("X11", "Xrandr", "Xinerama", "dl", "pthread")
    
    -- Add include directories
    add_includedirs("src")
    
    -- Add compilation flags
    add_cxflags("-Wall", "-Wextra", "-O2")
    
    -- Set output directory
    set_targetdir("build")



--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

