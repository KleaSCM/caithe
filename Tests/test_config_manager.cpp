/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_config_manager.cpp
 * Description: Test the ConfigManager functionality
 */

#include <iostream>
#include <string>
#include "../src/utils/ConfigManager.h"

int main() {
    std::cout << "Testing ConfigManager..." << std::endl;
    
    // Create config manager
    ConfigManager config;
    
    std::cout << "Config path: " << config.getConfigPath() << std::endl;
    std::cout << "Config exists: " << (config.configExists() ? "Yes" : "No") << std::endl;
    
    // Test default values
    std::cout << "\nDefault values:" << std::endl;
    std::cout << "Window width: " << config.getInt("window.width", 1200) << std::endl;
    std::cout << "Window height: " << config.getInt("window.height", 800) << std::endl;
    std::cout << "Show demo window: " << (config.getBool("ui.showDemoWindow", false) ? "Yes" : "No") << std::endl;
    std::cout << "Default wallpaper mode: " << config.getString("wallpaper.defaultMode", "Scale") << std::endl;
    
    // Test setting values
    std::cout << "\nSetting values..." << std::endl;
    config.setInt("window.width", 1400);
    config.setInt("window.height", 900);
    config.setBool("ui.showDemoWindow", true);
    config.setString("wallpaper.defaultMode", "Fill");
    
    // Test reading back values
    std::cout << "\nUpdated values:" << std::endl;
    std::cout << "Window width: " << config.getInt("window.width", 1200) << std::endl;
    std::cout << "Window height: " << config.getInt("window.height", 800) << std::endl;
    std::cout << "Show demo window: " << (config.getBool("ui.showDemoWindow", false) ? "Yes" : "No") << std::endl;
    std::cout << "Default wallpaper mode: " << config.getString("wallpaper.defaultMode", "Scale") << std::endl;
    
    // Test saving config
    std::cout << "\nSaving config..." << std::endl;
    if (config.saveConfig()) {
        std::cout << "Config saved successfully!" << std::endl;
        std::cout << "Config file: " << config.getConfigPath() << std::endl;
    } else {
        std::cout << "Failed to save config: " << config.getLastError() << std::endl;
    }
    
    // Test validation
    std::cout << "\nConfig validation: " << (config.validateConfig() ? "Passed" : "Failed") << std::endl;
    
    std::cout << "\nConfigManager test completed!" << std::endl;
    return 0;
} 