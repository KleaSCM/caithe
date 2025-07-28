/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: Application.h
 * Description: Main application class for Caithe Wallpaper Manager
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "../core/WallpaperManager.h"
#include "../core/DisplayManager.h"
#include "../utils/FileUtils.h"

// Forward declarations
enum class WallpaperMode;

class Application {
public:
    Application();
    ~Application();
    
    // Main application loop
    int run();
    
    // Disable copy constructor and assignment
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    // Window management
    bool initializeWindow();
    void cleanupWindow();
    
    // ImGui setup
    bool initializeImGui();
    void cleanupImGui();
    
    // Main rendering loop
    void renderFrame();
    void renderMainWindow();
    void renderWallpaperPanel();
    void renderDisplayPanel();
    void renderSettingsPanel();
    void renderAboutDialog();
    
    // Event handling
    void handleInput();
    
    // Member variables
    GLFWwindow* m_window;
    std::unique_ptr<WallpaperManager> m_wallpaperManager;
    std::unique_ptr<DisplayManager> m_displayManager;
    
    // UI state
    bool m_showDemoWindow;
    int m_selectedDisplay;
    std::string m_currentWallpaperPath;
    
    // Window properties
    static constexpr int WINDOW_WIDTH = 1200;
    static constexpr int WINDOW_HEIGHT = 800;
    static constexpr const char* WINDOW_TITLE = "Caithe Wallpaper Manager";
}; 