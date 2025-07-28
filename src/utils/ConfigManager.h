/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: ConfigManager.h
 * Description: JSON configuration management for Caithe Wallpaper Manager
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// Forward declarations
enum class WallpaperMode;

struct DisplayConfig {
    std::string name;
    int wallpaperMode;
    std::string wallpaperPath;
    double scale;
    bool enabled;
};

struct ApplicationConfig {
    // Window settings
    int windowWidth;
    int windowHeight;
    int windowX;
    int windowY;
    bool windowMaximized;
    
    // UI settings
    bool showDemoWindow;
    int selectedDisplay;
    std::string lastWallpaperPath;
    
    // Wallpaper settings
    std::vector<std::string> wallpaperDirectories;
    std::string defaultWallpaperMode;
    bool autoApplyToAllDisplays;
    
    // Display configurations
    std::vector<DisplayConfig> displays;
    
    // Advanced settings
    bool enableHotplugEvents;
    bool enableLiveSync;
    int slideshowInterval;
    bool enableSlideshow;
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Configuration file operations
    bool loadConfig(const std::string& configPath = "");
    bool saveConfig(const std::string& configPath = "");
    bool createDefaultConfig();
    
    // Configuration access with type safety
    ApplicationConfig& getConfig();
    const ApplicationConfig& getConfig() const;
    
    // Individual setting access
    bool getBool(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    std::vector<std::string> getStringArray(const std::string& key) const;
    
    // Setting modification
    void setBool(const std::string& key, bool value);
    void setInt(const std::string& key, int value);
    void setString(const std::string& key, const std::string& value);
    void setStringArray(const std::string& key, const std::vector<std::string>& value);
    
    // Display-specific configuration
    DisplayConfig* getDisplayConfig(const std::string& displayName);
    bool setDisplayConfig(const std::string& displayName, const DisplayConfig& config);
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration validation
    bool validateConfig() const;
    bool migrateConfig();
    
    // Utility functions
    std::string getConfigPath() const;
    bool configExists() const;
    bool backupConfig() const;

private:
    // Internal helper methods
    bool parseConfigFile(const std::string& path);
    bool writeConfigFile(const std::string& path);
    void createDefaultApplicationConfig();
    void createDefaultDisplayConfig();
    
    // JSON conversion helpers
    nlohmann::json configToJson() const;
    bool jsonToConfig(const nlohmann::json& json);
    
    // Configuration data
    ApplicationConfig m_config;
    std::string m_configPath;
    mutable std::string m_lastError;
    
    // Default configuration values
    static constexpr int DEFAULT_WINDOW_WIDTH = 1200;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 800;
    static constexpr int DEFAULT_WINDOW_X = 100;
    static constexpr int DEFAULT_WINDOW_Y = 100;
    static constexpr int DEFAULT_SLIDESHOW_INTERVAL = 300; // 5 minutes
}; 