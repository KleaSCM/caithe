/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: Application.cpp
 * Description: Implementation of the main application class for Caithe Wallpaper Manager
 */

#include "Application.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cmath>

Application::Application() 
    : m_window(nullptr)
    , m_showDemoWindow(false)
    , m_selectedDisplay(0) {
    
    if (!initializeWindow()) {
        throw std::runtime_error("Failed to initialize window");
    }
    
    if (!initializeImGui()) {
        throw std::runtime_error("Failed to initialize ImGui");
    }
    
    // Initialize managers
    m_wallpaperManager = std::make_unique<WallpaperManager>();
    m_displayManager = std::make_unique<DisplayManager>();
}

Application::~Application() {
    cleanupImGui();
    cleanupWindow();
}

int Application::run() {
    // Main application loop
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        
            // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Render our UI
    renderFrame();
    
    // Render ImGui
    ImGui::Render();
        
        // Clear and render
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_window);
    }
    
    return 0;
}

bool Application::initializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW for OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create window
    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync
    
    return true;
}

void Application::cleanupWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool Application::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Not available in this ImGui version
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Not available in this ImGui version
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void Application::cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::renderFrame() {
    renderMainWindow();
    
    if (m_showDemoWindow) {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }
    
    // Render about dialog
    renderAboutDialog();
}

void Application::renderMainWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    
    ImGui::Begin("Caithe Wallpaper Manager", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Wallpapers")) {
            renderWallpaperPanel();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Displays")) {
            renderDisplayPanel();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Settings")) {
            renderSettingsPanel();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void Application::renderWallpaperPanel() {
    ImGui::Text("Wallpaper Management");
    ImGui::Separator();
    
    // Current wallpaper display
    ImGui::Text("Current Wallpaper: %s", m_currentWallpaperPath.c_str());
    
    if (ImGui::Button("Select Wallpaper")) {
        std::vector<std::string> filters = {".png", ".jpg", ".jpeg"};
        std::string path = FileUtils::openFileDialog("Select Wallpaper", "", filters);
        if (!path.empty()) {
            m_currentWallpaperPath = path;
            m_wallpaperManager->setWallpaper(path, 0);
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Remove Wallpaper")) {
        m_wallpaperManager->removeWallpaper(0);
        m_currentWallpaperPath.clear();
    }
    
    // Wallpaper mode selection
    const char* modes[] = {"Stretch", "Center", "Tile", "Scale"};
    static int currentMode = 0;
    if (ImGui::Combo("Wallpaper Mode", &currentMode, modes, IM_ARRAYSIZE(modes))) {
        m_wallpaperManager->setWallpaperMode(0, static_cast<WallpaperMode>(currentMode));
    }
}

void Application::renderDisplayPanel() {
    ImGui::Text("Display Management");
    ImGui::Separator();
    
    // Display selection
    const auto& displays = m_displayManager->getDisplays();
    if (ImGui::BeginCombo("Select Display", 
                          m_selectedDisplay < displays.size() ? displays[m_selectedDisplay].name.c_str() : "None")) {
        for (size_t i = 0; i < displays.size(); ++i) {
            const bool isSelected = (m_selectedDisplay == i);
            if (ImGui::Selectable(displays[i].name.c_str(), isSelected)) {
                m_selectedDisplay = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    // Display information
    if (m_selectedDisplay < displays.size()) {
        const auto& display = displays[m_selectedDisplay];
        ImGui::Text("Resolution: %dx%d", display.width, display.height);
        ImGui::Text("Refresh Rate: %d Hz", display.refreshRate);
        ImGui::Text("Connector: %s", display.connector.c_str());
    }
}

void Application::renderSettingsPanel() {
    ImGui::Text("Settings");
    ImGui::Separator();
    
    // Demo window toggle
    ImGui::Checkbox("Show Demo Window", &m_showDemoWindow);
    
    // Settings save/load
    if (ImGui::Button("Save Settings")) {
        // TODO: Implement settings save
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Load Settings")) {
        // TODO: Implement settings load
    }
}

void Application::renderAboutDialog() {
    if (ImGui::Begin("About Caithe", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Caithe Wallpaper Manager");
        ImGui::Text("Version: 1.0.0");
        ImGui::Text("A modern wallpaper manager for Hyprland");
        ImGui::Separator();
        ImGui::Text("Features:");
        ImGui::BulletText("Hyprland integration");
        ImGui::BulletText("Multi-display support");
        ImGui::BulletText("Multiple wallpaper modes");
        ImGui::BulletText("Real-time preview");
        ImGui::End();
    }
} 