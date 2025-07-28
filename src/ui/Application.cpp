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
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        return false;
    }
    
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
                  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Wallpaper", "Ctrl+O")) {
                // Implement file dialog with proper error handling
                std::vector<std::string> filters = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".webp", ".gif"};
                std::string selectedFile = FileUtils::openFileDialog("Select Wallpaper", "", filters);
                
                if (!selectedFile.empty()) {
                    m_currentWallpaperPath = selectedFile;
                    // Apply to selected display immediately
                    if (m_wallpaperManager) {
                        if (!m_wallpaperManager->setWallpaper(selectedFile, m_selectedDisplay)) {
                            // Show error in UI
                            std::cerr << "Failed to set wallpaper: " << m_wallpaperManager->getLastError() << std::endl;
                        }
                    }
                }
            }
            if (ImGui::MenuItem("Save Settings", "Ctrl+S")) {
                // Implement settings save with proper error handling
                std::string configPath = FileUtils::getConfigDirectory() + "/settings.json";
                if (FileUtils::createDirectory(FileUtils::getConfigDirectory())) {
                    // Create JSON settings file with current application state
                    std::ofstream settingsFile(configPath);
                    if (settingsFile.is_open()) {
                        settingsFile << "{\n";
                        settingsFile << "  \"autoStart\": " << (m_showDemoWindow ? "true" : "false") << ",\n";
                        settingsFile << "  \"selectedDisplay\": " << m_selectedDisplay << ",\n";
                        settingsFile << "  \"currentWallpaper\": \"" << m_currentWallpaperPath << "\",\n";
                        settingsFile << "  \"wallpaperMode\": 0,\n";
                        settingsFile << "  \"windowWidth\": " << WINDOW_WIDTH << ",\n";
                        settingsFile << "  \"windowHeight\": " << WINDOW_HEIGHT << "\n";
                        settingsFile << "}\n";
                        settingsFile.close();
                        std::cout << "Settings saved to: " << configPath << std::endl;
                    } else {
                        std::cerr << "Failed to write settings file" << std::endl;
                    }
                } else {
                    std::cerr << "Failed to create config directory" << std::endl;
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                glfwSetWindowShouldClose(m_window, true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Demo Window", nullptr, &m_showDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Show about dialog
                ImGui::OpenPopup("About Caithe");
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // Main content area
    ImGui::BeginChild("Content", ImVec2(0, 0), true);
    
    // Split into panels
    ImGui::Columns(3, "MainPanels", true);
    
    // Wallpaper panel
    ImGui::BeginChild("WallpaperPanel");
    renderWallpaperPanel();
    ImGui::EndChild();
    
    ImGui::NextColumn();
    
    // Display panel
    ImGui::BeginChild("DisplayPanel");
    renderDisplayPanel();
    ImGui::EndChild();
    
    ImGui::NextColumn();
    
    // Settings panel
    ImGui::BeginChild("SettingsPanel");
    renderSettingsPanel();
    ImGui::EndChild();
    
    ImGui::Columns(1);
    ImGui::EndChild();
    
    ImGui::End();
}

void Application::renderWallpaperPanel() {
    ImGui::Text("Wallpaper Management");
    ImGui::Separator();
    
    if (ImGui::Button("Load Wallpaper", ImVec2(ImGui::GetWindowWidth() - 20, 30))) {
        // Open file dialog to select wallpaper
        std::vector<std::string> filters = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".webp", ".gif"};
        std::string selectedFile = FileUtils::openFileDialog("Select Wallpaper", "", filters);
        
        if (!selectedFile.empty()) {
            m_currentWallpaperPath = selectedFile;
        }
    }
    
    ImGui::Spacing();
    
    if (!m_currentWallpaperPath.empty()) {
        ImGui::Text("Current: %s", m_currentWallpaperPath.c_str());
        
        if (ImGui::Button("Apply to Selected Display", ImVec2(ImGui::GetWindowWidth() - 20, 30))) {
            if (m_wallpaperManager && m_displayManager) {
                m_wallpaperManager->setWallpaper(m_currentWallpaperPath, m_selectedDisplay);
            }
        }
        
        if (ImGui::Button("Apply to All Displays", ImVec2(ImGui::GetWindowWidth() - 20, 30))) {
            if (m_wallpaperManager) {
                m_wallpaperManager->setWallpaperAllDisplays(m_currentWallpaperPath);
            }
        }
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No wallpaper selected");
    }
}

void Application::renderDisplayPanel() {
    ImGui::Text("Display Management");
    ImGui::Separator();
    
    if (m_displayManager) {
        auto displays = m_displayManager->getDisplays();
        
        for (size_t i = 0; i < displays.size(); ++i) {
            const auto& display = displays[i];
            bool isSelected = (m_selectedDisplay == static_cast<int>(i));
            
            if (ImGui::Selectable(display.name.c_str(), isSelected)) {
                m_selectedDisplay = static_cast<int>(i);
            }
            
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Resolution: %dx%d", display.width, display.height);
            }
        }
    }
}

void Application::renderSettingsPanel() {
    ImGui::Text("Settings");
    ImGui::Separator();
    
    static bool autoStart = false;
    static bool rememberLastWallpaper = true;
    static int wallpaperMode = 0;
    
    ImGui::Checkbox("Auto-start with system", &autoStart);
    ImGui::Checkbox("Remember last wallpaper", &rememberLastWallpaper);
    
    ImGui::Spacing();
    ImGui::Text("Wallpaper Mode:");
    ImGui::RadioButton("Stretch", &wallpaperMode, 0);
    ImGui::RadioButton("Center", &wallpaperMode, 1);
    ImGui::RadioButton("Tile", &wallpaperMode, 2);
    ImGui::RadioButton("Scale", &wallpaperMode, 3);
    
    ImGui::Spacing();
    ImGui::Separator();
    
    if (ImGui::Button("Save Settings", ImVec2(ImGui::GetWindowWidth() - 20, 30))) {
        // Save settings to JSON file
        std::string configPath = FileUtils::getConfigDirectory() + "/settings.json";
        if (FileUtils::createDirectory(FileUtils::getConfigDirectory())) {
            // Create comprehensive settings file with all application state
            std::ofstream settingsFile(configPath);
            if (settingsFile.is_open()) {
                // Calculate settings hash for integrity verification
                // Hash = sum(settings_values * prime_factors) mod large_prime
                unsigned long long hash = 0;
                const unsigned long long prime = 1000000007;
                
                // Hash calculation for settings integrity
                hash = (hash * 31 + autoStart) % prime;
                hash = (hash * 31 + rememberLastWallpaper) % prime;
                hash = (hash * 31 + wallpaperMode) % prime;
                hash = (hash * 31 + m_selectedDisplay) % prime;
                
                settingsFile << "{\n";
                settingsFile << "  \"autoStart\": " << (autoStart ? "true" : "false") << ",\n";
                settingsFile << "  \"rememberLastWallpaper\": " << (rememberLastWallpaper ? "true" : "false") << ",\n";
                settingsFile << "  \"wallpaperMode\": " << wallpaperMode << ",\n";
                settingsFile << "  \"selectedDisplay\": " << m_selectedDisplay << ",\n";
                settingsFile << "  \"currentWallpaper\": \"" << m_currentWallpaperPath << "\",\n";
                settingsFile << "  \"windowWidth\": " << WINDOW_WIDTH << ",\n";
                settingsFile << "  \"windowHeight\": " << WINDOW_HEIGHT << ",\n";
                settingsFile << "  \"settingsHash\": " << hash << "\n";
                settingsFile << "}\n";
                settingsFile.close();
                std::cout << "Settings saved to: " << configPath << " (hash: " << hash << ")" << std::endl;
            } else {
                std::cerr << "Failed to write settings file" << std::endl;
            }
        } else {
            std::cerr << "Failed to create config directory" << std::endl;
        }
    }
}

void Application::renderAboutDialog() {
    // Center the about dialog on screen
    ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    
    if (ImGui::BeginPopupModal("About Caithe", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Caithe Wallpaper Manager");
        ImGui::Text("Version 1.0.0");
        ImGui::Separator();
        
                       ImGui::Text("Features:");
        ImGui::BulletText("Aspect ratio preservation algorithms");
        ImGui::BulletText("Geometric display coordinate systems");
        ImGui::BulletText("Path normalization with O(n) complexity");
        ImGui::BulletText("Error handling with detailed codes");
        
        ImGui::Separator();
        ImGui::Text("Author: KleaSCM");
        ImGui::Text("Email: KleaSCM@gmail.com");
        
        if (ImGui::Button("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
} 