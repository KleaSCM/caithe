/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_core_functionality.cpp
 * Description: Core functionality tests for display detection, wallpaper modes, and file operations
 */

#include <iostream>
#include <cassert>
#include <string>
#include "../src/core/WallpaperManager.h"
#include "../src/core/DisplayManager.h"
#include "../src/utils/FileUtils.h"

void testBasicFunctionality() {
    std::cout << "Testing basic functionality..." << std::endl;
    
    // Test WallpaperManager
    WallpaperManager wallpaperManager;
    assert(wallpaperManager.getLastError().empty());
    
    // Test DisplayManager
    DisplayManager displayManager;
    auto displays = displayManager.getDisplays();
    std::cout << "Found " << displays.size() << " displays" << std::endl;
    
    // Test FileUtils
    auto formats = FileUtils::getSupportedImageFormats();
    assert(!formats.empty());
    std::cout << "Supported formats: ";
    for (const auto& format : formats) {
        std::cout << format << " ";
    }
    std::cout << std::endl;
    
    // Test image file validation
    assert(FileUtils::isImageFile("test.png"));
    assert(!FileUtils::isImageFile("test.txt"));
    
    std::cout << "✓ Basic functionality test passed" << std::endl;
}

void testDisplayDetection() {
    std::cout << "Testing display detection..." << std::endl;
    
    DisplayManager displayManager;
    
    // Test that we can get displays
    auto displays = displayManager.getDisplays();
    std::cout << "Display count: " << displays.size() << std::endl;
    
    if (!displays.empty()) {
        const auto& primary = displayManager.getPrimaryDisplay();
        std::cout << "Primary display: " << primary.name << " (" 
                  << primary.width << "x" << primary.height << ")" << std::endl;
    }
    
    std::cout << "✓ Display detection test passed" << std::endl;
}

void testWallpaperModes() {
    std::cout << "Testing wallpaper modes..." << std::endl;
    
    WallpaperManager manager;
    
    // Test all wallpaper modes
    WallpaperMode modes[] = {
        WallpaperMode::Stretch,
        WallpaperMode::Center,
        WallpaperMode::Tile,
        WallpaperMode::Scale
    };
    
    for (auto mode : modes) {
        assert(manager.setWallpaperMode(0, mode));
        assert(manager.getWallpaperMode(0) == mode);
    }
    
    std::cout << "✓ Wallpaper modes test passed" << std::endl;
}

int main() {
    std::cout << "Running basic tests for Caithe Wallpaper Manager..." << std::endl;
    std::cout << "==================================================" << std::endl;
    
    try {
        testBasicFunctionality();
        testDisplayDetection();
        testWallpaperModes();
        
        std::cout << "==================================================" << std::endl;
        std::cout << "All basic tests passed! ✨" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
} 