/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_wallpaper_manager.cpp
 * Description: Unit tests for WallpaperManager class
 */

#include <iostream>
#include <cassert>
#include <string>
#include "../src/core/WallpaperManager.h"

void testWallpaperManagerCreation() {
    std::cout << "Testing WallpaperManager creation..." << std::endl;
    
    WallpaperManager manager;
    
    // Test that manager is created without errors
    assert(manager.getLastError().empty());
    
    std::cout << "✓ WallpaperManager creation test passed" << std::endl;
}

void testSupportedFormats() {
    std::cout << "Testing supported formats..." << std::endl;
    
    WallpaperManager manager;
    auto formats = manager.getSupportedFormats();
    
    // Test that we have supported formats
    assert(!formats.empty());
    
    // Test that common formats are supported
    bool hasPng = false, hasJpg = false;
    for (const auto& format : formats) {
        if (format == ".png") hasPng = true;
        if (format == ".jpg") hasJpg = true;
    }
    
    assert(hasPng);
    assert(hasJpg);
    
    std::cout << "✓ Supported formats test passed" << std::endl;
}

void testImageFileValidation() {
    std::cout << "Testing image file validation..." << std::endl;
    
    WallpaperManager manager;
    
    // Test valid image files
    assert(manager.isValidImageFile("test.png"));
    assert(manager.isValidImageFile("test.jpg"));
    assert(manager.isValidImageFile("test.jpeg"));
    
    // Test invalid files
    assert(!manager.isValidImageFile("test.txt"));
    assert(!manager.isValidImageFile("test"));
    assert(!manager.isValidImageFile(""));
    
    std::cout << "✓ Image file validation test passed" << std::endl;
}

void testWallpaperMode() {
    std::cout << "Testing wallpaper modes..." << std::endl;
    
    WallpaperManager manager;
    
    // Test default mode
    WallpaperMode defaultMode = manager.getWallpaperMode(0);
    assert(defaultMode == WallpaperMode::Scale);
    
    // Test setting mode
    assert(manager.setWallpaperMode(0, WallpaperMode::Stretch));
    assert(manager.getWallpaperMode(0) == WallpaperMode::Stretch);
    
    std::cout << "✓ Wallpaper mode test passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    WallpaperManager manager;
    
    // Test setting invalid wallpaper
    assert(!manager.setWallpaper("nonexistent.png"));
    assert(!manager.getLastError().empty());
    
    // Test clearing error
    manager.clearError();
    assert(manager.getLastError().empty());
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

int main() {
    std::cout << "Running WallpaperManager tests..." << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        testWallpaperManagerCreation();
        testSupportedFormats();
        testImageFileValidation();
        testWallpaperMode();
        testErrorHandling();
        
        std::cout << "=================================" << std::endl;
        std::cout << "All tests passed! ✨" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
} 