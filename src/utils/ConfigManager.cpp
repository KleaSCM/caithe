/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: ConfigManager.cpp
 * Description: Implementation of JSON configuration management for Caithe Wallpaper Manager
 */

#include "ConfigManager.h"
#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

ConfigManager::ConfigManager() {
    // Set default config path
    m_configPath = FileUtils::getConfigDirectory() + "/config.json";
    
    // Initialize with default values
    createDefaultApplicationConfig();
    
    // Try to load existing config
    if (configExists()) {
        loadConfig();
    } else {
        // Create default config file
        createDefaultConfig();
    }
}

ConfigManager::~ConfigManager() {
    // Auto-save on destruction
    saveConfig();
}

bool ConfigManager::loadConfig(const std::string& configPath) {
    std::string path = configPath.empty() ? m_configPath : configPath;
    
    if (!std::filesystem::exists(path)) {
        m_lastError = "Config file does not exist: " + path;
        return false;
    }
    
    return parseConfigFile(path);
}

bool ConfigManager::saveConfig(const std::string& configPath) {
    std::string path = configPath.empty() ? m_configPath : configPath;
    
    // Ensure config directory exists
    std::filesystem::path configDir = std::filesystem::path(path).parent_path();
    if (!std::filesystem::exists(configDir)) {
        if (!std::filesystem::create_directories(configDir)) {
            m_lastError = "Failed to create config directory: " + configDir.string();
            return false;
        }
    }
    
    return writeConfigFile(path);
}

bool ConfigManager::createDefaultConfig() {
    createDefaultApplicationConfig();
    return saveConfig();
}

ApplicationConfig& ConfigManager::getConfig() {
    return m_config;
}

const ApplicationConfig& ConfigManager::getConfig() const {
    return m_config;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    try {
        nlohmann::json json = configToJson();
        if (json.contains(key)) {
            return json[key].get<bool>();
        }
    } catch (const std::exception& e) {
        m_lastError = "Error reading bool setting '" + key + "': " + e.what();
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    try {
        nlohmann::json json = configToJson();
        if (json.contains(key)) {
            return json[key].get<int>();
        }
    } catch (const std::exception& e) {
        m_lastError = "Error reading int setting '" + key + "': " + e.what();
    }
    return defaultValue;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    try {
        nlohmann::json json = configToJson();
        if (json.contains(key)) {
            return json[key].get<std::string>();
        }
    } catch (const std::exception& e) {
        m_lastError = "Error reading string setting '" + key + "': " + e.what();
    }
    return defaultValue;
}

std::vector<std::string> ConfigManager::getStringArray(const std::string& key) const {
    try {
        nlohmann::json json = configToJson();
        if (json.contains(key) && json[key].is_array()) {
            return json[key].get<std::vector<std::string>>();
        }
    } catch (const std::exception& e) {
        m_lastError = "Error reading string array setting '" + key + "': " + e.what();
    }
    return {};
}

void ConfigManager::setBool(const std::string& key, bool value) {
    try {
        nlohmann::json json = configToJson();
        json[key] = value;
        jsonToConfig(json);
    } catch (const std::exception& e) {
        m_lastError = "Error setting bool '" + key + "': " + e.what();
    }
}

void ConfigManager::setInt(const std::string& key, int value) {
    try {
        nlohmann::json json = configToJson();
        json[key] = value;
        jsonToConfig(json);
    } catch (const std::exception& e) {
        m_lastError = "Error setting int '" + key + "': " + e.what();
    }
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    try {
        nlohmann::json json = configToJson();
        json[key] = value;
        jsonToConfig(json);
    } catch (const std::exception& e) {
        m_lastError = "Error setting string '" + key + "': " + e.what();
    }
}

void ConfigManager::setStringArray(const std::string& key, const std::vector<std::string>& value) {
    try {
        nlohmann::json json = configToJson();
        json[key] = value;
        jsonToConfig(json);
    } catch (const std::exception& e) {
        m_lastError = "Error setting string array '" + key + "': " + e.what();
    }
}

DisplayConfig* ConfigManager::getDisplayConfig(const std::string& displayName) {
    for (auto& display : m_config.displays) {
        if (display.name == displayName) {
            return &display;
        }
    }
    return nullptr;
}

bool ConfigManager::setDisplayConfig(const std::string& displayName, const DisplayConfig& config) {
    // Find existing display config
    for (auto& display : m_config.displays) {
        if (display.name == displayName) {
            display = config;
            return true;
        }
    }
    
    // Add new display config
    m_config.displays.push_back(config);
    return true;
}

std::string ConfigManager::getLastError() const {
    return m_lastError;
}

void ConfigManager::clearError() {
    m_lastError.clear();
}

bool ConfigManager::validateConfig() const {
    // Basic validation
    if (m_config.windowWidth <= 0 || m_config.windowHeight <= 0) {
        return false;
    }
    
    if (m_config.slideshowInterval < 0) {
        return false;
    }
    
    return true;
}

bool ConfigManager::migrateConfig() {
    // Future: implement config migration logic
    return true;
}

std::string ConfigManager::getConfigPath() const {
    return m_configPath;
}

bool ConfigManager::configExists() const {
    return std::filesystem::exists(m_configPath);
}

bool ConfigManager::backupConfig() const {
    if (!configExists()) {
        return false;
    }
    
    std::string backupPath = m_configPath + ".backup";
    try {
        std::filesystem::copy_file(m_configPath, backupPath, 
                                  std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        m_lastError = "Failed to backup config: " + std::string(e.what());
        return false;
    }
}

bool ConfigManager::parseConfigFile(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            m_lastError = "Failed to open config file: " + path;
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        return jsonToConfig(json);
        
    } catch (const std::exception& e) {
        m_lastError = "Failed to parse config file: " + std::string(e.what());
        return false;
    }
}

bool ConfigManager::writeConfigFile(const std::string& path) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            m_lastError = "Failed to create config file: " + path;
            return false;
        }
        
        nlohmann::json json = configToJson();
        file << json.dump(2); // Pretty print with 2 spaces
        
        return true;
        
    } catch (const std::exception& e) {
        m_lastError = "Failed to write config file: " + std::string(e.what());
        return false;
    }
}

void ConfigManager::createDefaultApplicationConfig() {
    // Window settings
    m_config.windowWidth = DEFAULT_WINDOW_WIDTH;
    m_config.windowHeight = DEFAULT_WINDOW_HEIGHT;
    m_config.windowX = DEFAULT_WINDOW_X;
    m_config.windowY = DEFAULT_WINDOW_Y;
    m_config.windowMaximized = false;
    
    // UI settings
    m_config.showDemoWindow = false;
    m_config.selectedDisplay = 0;
    m_config.lastWallpaperPath = "";
    
    // Wallpaper settings
    m_config.wallpaperDirectories = {
        "~/Pictures/Wallpapers",
        "~/Downloads",
        "/usr/share/backgrounds"
    };
    m_config.defaultWallpaperMode = "Scale";
    m_config.autoApplyToAllDisplays = false;
    
    // Advanced settings
    m_config.enableHotplugEvents = true;
    m_config.enableLiveSync = true;
    m_config.slideshowInterval = DEFAULT_SLIDESHOW_INTERVAL;
    m_config.enableSlideshow = false;
    
    // Display configurations
    m_config.displays.clear();
}

void ConfigManager::createDefaultDisplayConfig() {
    // This will be called when new displays are detected
}

nlohmann::json ConfigManager::configToJson() const {
    nlohmann::json json;
    
    // Window settings
    json["window"]["width"] = m_config.windowWidth;
    json["window"]["height"] = m_config.windowHeight;
    json["window"]["x"] = m_config.windowX;
    json["window"]["y"] = m_config.windowY;
    json["window"]["maximized"] = m_config.windowMaximized;
    
    // UI settings
    json["ui"]["showDemoWindow"] = m_config.showDemoWindow;
    json["ui"]["selectedDisplay"] = m_config.selectedDisplay;
    json["ui"]["lastWallpaperPath"] = m_config.lastWallpaperPath;
    
    // Wallpaper settings
    json["wallpaper"]["directories"] = m_config.wallpaperDirectories;
    json["wallpaper"]["defaultMode"] = m_config.defaultWallpaperMode;
    json["wallpaper"]["autoApplyToAll"] = m_config.autoApplyToAllDisplays;
    
    // Advanced settings
    json["advanced"]["enableHotplugEvents"] = m_config.enableHotplugEvents;
    json["advanced"]["enableLiveSync"] = m_config.enableLiveSync;
    json["advanced"]["slideshowInterval"] = m_config.slideshowInterval;
    json["advanced"]["enableSlideshow"] = m_config.enableSlideshow;
    
    // Display configurations
    json["displays"] = nlohmann::json::array();
    for (const auto& display : m_config.displays) {
        nlohmann::json displayJson;
        displayJson["name"] = display.name;
        displayJson["wallpaperMode"] = display.wallpaperMode;
        displayJson["wallpaperPath"] = display.wallpaperPath;
        displayJson["scale"] = display.scale;
        displayJson["enabled"] = display.enabled;
        json["displays"].push_back(displayJson);
    }
    
    return json;
}

bool ConfigManager::jsonToConfig(const nlohmann::json& json) {
    try {
        // Window settings
        if (json.contains("window")) {
            const auto& window = json["window"];
            m_config.windowWidth = window.value("width", DEFAULT_WINDOW_WIDTH);
            m_config.windowHeight = window.value("height", DEFAULT_WINDOW_HEIGHT);
            m_config.windowX = window.value("x", DEFAULT_WINDOW_X);
            m_config.windowY = window.value("y", DEFAULT_WINDOW_Y);
            m_config.windowMaximized = window.value("maximized", false);
        }
        
        // UI settings
        if (json.contains("ui")) {
            const auto& ui = json["ui"];
            m_config.showDemoWindow = ui.value("showDemoWindow", false);
            m_config.selectedDisplay = ui.value("selectedDisplay", 0);
            m_config.lastWallpaperPath = ui.value("lastWallpaperPath", "");
        }
        
        // Wallpaper settings
        if (json.contains("wallpaper")) {
            const auto& wallpaper = json["wallpaper"];
            m_config.wallpaperDirectories = wallpaper.value("directories", std::vector<std::string>{});
            m_config.defaultWallpaperMode = wallpaper.value("defaultMode", "Scale");
            m_config.autoApplyToAllDisplays = wallpaper.value("autoApplyToAll", false);
        }
        
        // Advanced settings
        if (json.contains("advanced")) {
            const auto& advanced = json["advanced"];
            m_config.enableHotplugEvents = advanced.value("enableHotplugEvents", true);
            m_config.enableLiveSync = advanced.value("enableLiveSync", true);
            m_config.slideshowInterval = advanced.value("slideshowInterval", DEFAULT_SLIDESHOW_INTERVAL);
            m_config.enableSlideshow = advanced.value("enableSlideshow", false);
        }
        
        // Display configurations
        m_config.displays.clear();
        if (json.contains("displays") && json["displays"].is_array()) {
            for (const auto& displayJson : json["displays"]) {
                DisplayConfig display;
                display.name = displayJson.value("name", "");
                display.wallpaperMode = displayJson.value("wallpaperMode", 0);
                display.wallpaperPath = displayJson.value("wallpaperPath", "");
                display.scale = displayJson.value("scale", 1.0);
                display.enabled = displayJson.value("enabled", true);
                m_config.displays.push_back(display);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        m_lastError = "Failed to parse JSON config: " + std::string(e.what());
        return false;
    }
} 