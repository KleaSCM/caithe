/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_core_functionality_simple.cpp
 * Description: Simple core functionality test for mathematical algorithms, display detection, and file operations
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include "core/WallpaperManager.h"
#include "core/DisplayManager.h"
#include "utils/FileUtils.h"

int main() {
    std::cout << "🧪 Testing Caithe Wallpaper Manager Core Functionality" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    try {
        // Test WallpaperManager
        std::cout << "\n📋 Testing WallpaperManager..." << std::endl;
        WallpaperManager wallpaperManager;
        
        // Test mathematical functions
        std::cout << "  Testing aspect ratio scaling..." << std::endl;
        double scale = wallpaperManager.calculateAspectRatioScale(1920, 1080, 3840, 2160);
        std::cout << "    Scale for 3840x2160 on 1920x1080: " << scale << " (expected: 0.5)" << std::endl;
        
        auto [offsetX, offsetY] = wallpaperManager.calculateCenteringOffsets(1920, 1080, 800, 600);
        std::cout << "    Centering offsets for 800x600 on 1920x1080: (" << offsetX << ", " << offsetY << ") (expected: 560, 240)" << std::endl;
        
        auto [tilesX, tilesY] = wallpaperManager.calculateTilingCount(1920, 1080, 400, 300);
        std::cout << "    Tiling count for 400x300 on 1920x1080: (" << tilesX << ", " << tilesY << ") (expected: 5, 4)" << std::endl;
        
        // Test DisplayManager
        std::cout << "\n🖥️  Testing DisplayManager..." << std::endl;
        DisplayManager displayManager;
        
        if (displayManager.refreshDisplays()) {
            auto displays = displayManager.getDisplays();
            std::cout << "  Found " << displays.size() << " displays:" << std::endl;
            for (const auto& display : displays) {
                std::cout << "    Display " << display.id << ": " << display.name 
                          << " (" << display.width << "x" << display.height 
                          << " @ " << display.refreshRate << "Hz)" << std::endl;
            }
        } else {
            std::cout << "  Failed to refresh displays: " << displayManager.getLastError() << std::endl;
        }
        
        // Test FileUtils
        std::cout << "\n📁 Testing FileUtils..." << std::endl;
        std::string configDir = FileUtils::getConfigDirectory();
        std::cout << "  Config directory: " << configDir << std::endl;
        
        if (FileUtils::createDirectory(configDir)) {
            std::cout << "  ✓ Config directory created successfully" << std::endl;
        } else {
            std::cout << "  ✗ Failed to create config directory" << std::endl;
        }
        
        // Test image format validation
        std::cout << "\n🖼️  Testing image format validation..." << std::endl;
        std::vector<std::string> testFiles = {
            "test.png",
            "test.jpg", 
            "test.txt",
            "test.unknown"
        };
        
        for (const auto& file : testFiles) {
            bool isValid = wallpaperManager.isValidImageFile(file);
            std::cout << "  " << file << ": " << (isValid ? "✓ Valid" : "✗ Invalid") << std::endl;
        }
        
        std::cout << "\n🎉 All core functionality tests completed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
} 