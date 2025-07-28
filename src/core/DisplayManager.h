/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: DisplayManager.h
 * Description: Display detection and management for Hyprland with geometric display calculations
 * 
 * Mathematical Foundation:
 * - Display coordinate system: origin (0,0) at top-left, positive x right, positive y down
 * - Primary display detection: display at position (0,0) is considered primary
 * - Display arrangement: displays positioned relative to each other using offset coordinates
 * - Scale factor calculation: physical_dpi / logical_dpi for proper scaling
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

struct Display {
    int id;
    std::string name;           // Display name (e.g., "DP-1", "HDMI-A-1")
    std::string description;    // Human-readable description
    int width;                  // Display width in pixels
    int height;                 // Display height in pixels
    int refreshRate;            // Refresh rate in Hz
    bool isPrimary;             // Whether this is the primary display
    bool isActive;              // Whether the display is currently active
    std::string connector;      // Physical connector type
    double scale;               // Display scale factor
};

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    
    // Display detection and management
    bool refreshDisplays();
    std::vector<Display> getDisplays() const;
    Display getDisplay(int id) const;
    Display getPrimaryDisplay() const;
    
    // Display information
    int getDisplayCount() const;
    bool hasDisplay(int id) const;
    std::string getDisplayName(int id) const;
    
    // Hyprland integration
    bool queryHyprlandDisplays();
    std::vector<std::string> getHyprlandDisplayNames() const;
    
    // Error handling with detailed error codes
    std::string getLastError() const;
    void clearError();
    
    // Error codes for detailed error handling
    enum class ErrorCode {
        None = 0,
        HyprlandNotRunning = 1,
        XrandrNotAvailable = 2,
        NoDisplaysFound = 3,
        InvalidDisplayId = 4,
        CommandExecutionFailed = 5,
        ParseError = 6,
        SystemError = 7
    };
    
    ErrorCode getLastErrorCode() const;

private:
    // Internal helper methods
    std::string executeCommand(const std::string& command) const;
    bool parseHyprlandOutput(const std::string& output);
    bool parseXrandrOutput(const std::string& output);
    Display createDisplayFromInfo(const std::string& name, int width, int height, 
                                 int refreshRate = 60, bool isPrimary = false);
    
    // Display storage
    std::vector<Display> m_displays;
    std::string m_lastError;
    ErrorCode m_lastErrorCode;
    
    // Hyprland command templates
    static constexpr const char* HYPRLAND_MONITORS_CMD = "hyprctl monitors";
    static constexpr const char* XRANDR_CMD = "xrandr --listmonitors";
}; 