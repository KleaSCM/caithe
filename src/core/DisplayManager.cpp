/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: DisplayManager.cpp
 * Description: Implementation of display detection and management for Hyprland
 */

#include "DisplayManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <regex>

DisplayManager::DisplayManager() {
    clearError();
    m_lastErrorCode = ErrorCode::None;
    refreshDisplays();
}

DisplayManager::~DisplayManager() = default;

bool DisplayManager::refreshDisplays() {
    clearError();
    
    // Try Hyprland first,  fallback to xrandr
    if (!queryHyprlandDisplays()) {
        // Fallback to xrandr for X11 systems
        std::string xrandrOutput = executeCommand(XRANDR_CMD);
        if (!xrandrOutput.empty()) {
            if (!parseXrandrOutput(xrandrOutput)) {
                m_lastError = "Failed to parse xrandr output";
                m_lastErrorCode = ErrorCode::ParseError;
                return false;
            }
        } else {
            // Create default display if no detection method works
            m_displays.clear();
            m_displays.push_back(createDisplayFromInfo("DP-1", 1920, 1080, 60, true));
            m_lastError = "No display detection method available, using default";
            m_lastErrorCode = ErrorCode::NoDisplaysFound;
        }
    }
    
    if (m_displays.empty()) {
        m_lastError = "No displays found";
        m_lastErrorCode = ErrorCode::NoDisplaysFound;
        return false;
    }
    
    return true;
}

std::vector<Display> DisplayManager::getDisplays() const {
    return m_displays;
}

Display DisplayManager::getDisplay(int id) const {
    for (const auto& display : m_displays) {
        if (display.id == id) {
            return display;
        }
    }
    
    // Return empty display if not found
    return Display{};
}

Display DisplayManager::getPrimaryDisplay() const {
    for (const auto& display : m_displays) {
        if (display.isPrimary) {
            return display;
        }
    }
    
    // Return first display if no primary found
    if (!m_displays.empty()) {
        return m_displays[0];
    }
    
    return Display{};
}

int DisplayManager::getDisplayCount() const {
    return static_cast<int>(m_displays.size());
}

bool DisplayManager::hasDisplay(int id) const {
    for (const auto& display : m_displays) {
        if (display.id == id) {
            return true;
        }
    }
    return false;
}

std::string DisplayManager::getDisplayName(int id) const {
    for (const auto& display : m_displays) {
        if (display.id == id) {
            return display.name;
        }
    }
    return "";
}

bool DisplayManager::queryHyprlandDisplays() {
    std::string output = executeCommand(HYPRLAND_MONITORS_CMD);
    if (output.empty()) {
        m_lastError = "Failed to query Hyprland displays - Hyprland may not be running";
        m_lastErrorCode = ErrorCode::HyprlandNotRunning;
        return false;
    }
    
    if (!parseHyprlandOutput(output)) {
        m_lastError = "Failed to parse Hyprland display output";
        m_lastErrorCode = ErrorCode::ParseError;
        return false;
    }
    
    return true;
}

std::vector<std::string> DisplayManager::getHyprlandDisplayNames() const {
    std::vector<std::string> names;
    names.reserve(m_displays.size());
    
    for (const auto& display : m_displays) {
        names.push_back(display.name);
    }
    
    return names;
}

std::string DisplayManager::getLastError() const {
    return m_lastError;
}

DisplayManager::ErrorCode DisplayManager::getLastErrorCode() const {
    return m_lastErrorCode;
}

void DisplayManager::clearError() {
    m_lastError.clear();
    m_lastErrorCode = ErrorCode::None;
}

bool DisplayManager::parseHyprlandOutput(const std::string& output) {
    m_displays.clear();
    
    std::istringstream stream(output);
    std::string line;
    int displayId = 0;
    
    // Hyprland monitor output format:
    // Monitor DP-1 (ID 0): 2560x1440 @ 165.001Hz at 0x0
    // Monitor HDMI-A-1 (ID 1): 1920x1080 @ 60.001Hz at 1920x0
    
    std::regex monitorRegex(R"(Monitor (\w+-\w+) \(ID (\d+)\): (\d+)x(\d+) @ ([\d.]+)Hz at (\d+)x(\d+))");
    
    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, monitorRegex)) {
            std::string name = match[1].str();
            int id = std::stoi(match[2].str());
            int width = std::stoi(match[3].str());
            int height = std::stoi(match[4].str());
            double refreshRate = std::stod(match[5].str());
            int x = std::stoi(match[6].str());
            int y = std::stoi(match[7].str());
            
            // Primary display is at origin (0,0) in coordinate system
            bool isPrimary = (x == 0 && y == 0);
            
            Display display = createDisplayFromInfo(name, width, height, static_cast<int>(refreshRate), isPrimary);
            display.id = id;
            display.isActive = true;
            
            // Determine connector type from name
            if (name.find("DP-") != std::string::npos) {
                display.connector = "DisplayPort";
            } else if (name.find("HDMI-") != std::string::npos) {
                display.connector = "HDMI";
            } else if (name.find("DVI-") != std::string::npos) {
                display.connector = "DVI";
            } else {
                display.connector = "Unknown";
            }
            
            m_displays.push_back(display);
        }
    }
    
    return !m_displays.empty();
}

bool DisplayManager::parseXrandrOutput(const std::string& output) {
    m_displays.clear();
    
    std::istringstream stream(output);
    std::string line;
    int displayId = 0;
    
    // xrandr output format:
    // Monitors: 2
    //  0: +*DP-1 2560/597x1440/336+0+0  DP-1
    //  1: +HDMI-A-1 1920/509x1080/286+2560+0  HDMI-A-1
    
    std::regex monitorRegex(R"(\s*\d+:\s+\+?\*?(\w+-\w+)\s+(\d+)/(\d+)x(\d+)/(\d+)\+(\d+)\+(\d+)\s+(\w+-\w+))");
    
    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, monitorRegex)) {
            std::string name = match[1].str();
            int width = std::stoi(match[2].str());
            int height = std::stoi(match[4].str());
            int x = std::stoi(match[6].str());
            int y = std::stoi(match[7].str());
            
            // Primary display is at origin (0,0) in coordinate system
            bool isPrimary = (x == 0 && y == 0);
            
            Display display = createDisplayFromInfo(name, width, height, 60, isPrimary);
            display.id = displayId++;
            display.isActive = true;
            
            // Determine connector type from name
            if (name.find("DP-") != std::string::npos) {
                display.connector = "DisplayPort";
            } else if (name.find("HDMI-") != std::string::npos) {
                display.connector = "HDMI";
            } else if (name.find("DVI-") != std::string::npos) {
                display.connector = "DVI";
            } else {
                display.connector = "Unknown";
            }
            
            m_displays.push_back(display);
        }
    }
    
    return !m_displays.empty();
}

Display DisplayManager::createDisplayFromInfo(const std::string& name, int width, int height, 
                                            int refreshRate, bool isPrimary) {
    Display display;
    display.id = 0;
    display.name = name;
    display.width = width;
    display.height = height;
    display.refreshRate = refreshRate;
    display.isPrimary = isPrimary;
    display.isActive = true;
    display.scale = 1.0;
    
    // Create readable description
    std::ostringstream desc;
    desc << name << " (" << width << "x" << height << "@" << refreshRate << "Hz)";
    display.description = desc.str();
    
    return display;
}

std::string DisplayManager::executeCommand(const std::string& command) const {
    std::string result;
    
    // Use popen to execute command and capture output
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    pclose(pipe);
    return result;
} 