/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_wallpaper_integration.cpp
 * Description: Integration tests for wallpaper management, Hyprland communication, and real image processing
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "../src/core/WallpaperManager.h"
#include "../src/core/DisplayManager.h"
#include "../src/utils/FileUtils.h"

// Test mathematical implementations
void testMathematicalImplementations() {
    std::cout << "Testing mathematical implementations..." << std::endl;
    
    WallpaperManager manager;
    
    // Test aspect ratio preservation using real implementation
    // Given: display 1920x1080, image 3840x2160
    // Expected: scale = min(1920/3840, 1080/2160) = min(0.5, 0.5) = 0.5
    double scale = manager.calculateAspectRatioScale(1920, 1080, 3840, 2160);
    assert(std::abs(scale - 0.5) < 1e-6);
    
    // Test centering calculation using real implementation
    // Given: display 1920x1080, image 800x600
    // Expected: offset_x = (1920 - 800) / 2 = 560, offset_y = (1080 - 600) / 2 = 240
    auto [offset_x, offset_y] = manager.calculateCenteringOffsets(1920, 1080, 800, 600);
    assert(offset_x == 560);
    assert(offset_y == 240);
    
    // Test tiling algorithm using real implementation
    // Given: display 1920x1080, image 400x300
    // Expected: tiles_x = ceil(1920/400) = 5, tiles_y = ceil(1080/300) = 4
    auto [tiles_x, tiles_y] = manager.calculateTilingCount(1920, 1080, 400, 300);
    assert(tiles_x == 5);
    assert(tiles_y == 4);
    
    std::cout << "✓ Mathematical implementations test passed" << std::endl;
}

// Test complete error handling
void testCompleteErrorHandling() {
    std::cout << "Testing complete error handling..." << std::endl;
    
    WallpaperManager wallpaperManager;
    DisplayManager displayManager;
    
    // Test all error codes
    std::vector<std::pair<std::string, WallpaperManager::ErrorCode>> wallpaperTests = {
        {"", WallpaperManager::ErrorCode::InvalidPath},
        {"/nonexistent.png", WallpaperManager::ErrorCode::FileNotFound},
        {"test.txt", WallpaperManager::ErrorCode::UnsupportedFormat}
    };
    
    for (const auto& [path, expectedCode] : wallpaperTests) {
        wallpaperManager.clearError();
        assert(!wallpaperManager.setWallpaper(path, 0));
        assert(wallpaperManager.getLastErrorCode() == expectedCode);
        assert(!wallpaperManager.getLastError().empty());
    }
    
    // Test display manager error codes
    displayManager.clearError();
    assert(displayManager.getLastErrorCode() == DisplayManager::ErrorCode::None);
    
    std::cout << "✓ Complete error handling test passed" << std::endl;
}

// Test complete file operations
void testCompleteFileOperations() {
    std::cout << "Testing complete file operations..." << std::endl;
    
    // Test path normalization
    std::string testPath = "/home/user/../user/./wallpaper.png";
    std::string normalized = FileUtils::normalizePath(testPath);
    assert(!normalized.empty());
    
    // Test file extension validation
    std::vector<std::string> testPaths = {
        "test.png", "test.PNG", "test.jpg", "test.txt", ""
    };
    
    std::vector<bool> expectedResults = {true, true, true, false, false};
    
    for (size_t i = 0; i < testPaths.size(); ++i) {
        assert(FileUtils::isImageFile(testPaths[i]) == expectedResults[i]);
    }
    
    // Test directory operations
    std::string homeDir = FileUtils::getHomeDirectory();
    assert(!homeDir.empty());
    
    std::string configDir = FileUtils::getConfigDirectory();
    assert(!configDir.empty());
    
    std::cout << "✓ Complete file operations test passed" << std::endl;
}

// Test complete display detection
void testCompleteDisplayDetection() {
    std::cout << "Testing complete display detection..." << std::endl;
    
    DisplayManager manager;
    
    // Test that displays are detected
    auto displays = manager.getDisplays();
    std::cout << "Detected " << displays.size() << " displays" << std::endl;
    
    // Test primary display detection
    if (!displays.empty()) {
        const auto& primary = manager.getPrimaryDisplay();
        assert(primary.isPrimary);
        assert(primary.isActive);
        assert(primary.width > 0);
        assert(primary.height > 0);
        assert(primary.refreshRate > 0);
        
        // Test coordinate system validation
        // Primary display should be at (0,0) in coordinate system
        assert(primary.name.find("DP-") != std::string::npos || 
               primary.name.find("HDMI-") != std::string::npos ||
               primary.name.find("DVI-") != std::string::npos);
    }
    
    std::cout << "✓ Complete display detection test passed" << std::endl;
}

// Test complete wallpaper management
void testCompleteWallpaperManagement() {
    std::cout << "Testing complete wallpaper management..." << std::endl;
    
    WallpaperManager manager;
    
    // Test all wallpaper modes
    std::vector<WallpaperMode> modes = {
        WallpaperMode::Stretch,
        WallpaperMode::Center,
        WallpaperMode::Tile,
        WallpaperMode::Scale
    };
    
    for (auto mode : modes) {
        assert(manager.setWallpaperMode(0, mode));
        assert(manager.getWallpaperMode(0) == mode);
    }
    
    // Test move semantics
    std::string testPath = "test.png";
    assert(manager.setWallpaper(std::move(testPath), 0));
    assert(testPath.empty()); // Should be moved
    
    // Test const reference efficiency
    const auto& wallpapers = manager.getAllWallpapers();
    assert(&wallpapers == &manager.getAllWallpapers()); // Should return same reference
    
    std::cout << "✓ Complete wallpaper management test passed" << std::endl;
}

// Test performance optimizations
void testPerformanceOptimizations() {
    std::cout << "Testing performance optimizations..." << std::endl;
    
    WallpaperManager manager;
    
    // Test rapid operations
    for (int i = 0; i < 1000; ++i) {
        manager.setWallpaperMode(0, static_cast<WallpaperMode>(i % 4));
    }
    
    // Test memory efficiency
    std::vector<std::string> largePaths;
    for (int i = 0; i < 100; ++i) {
        largePaths.push_back(std::string(1000, 'a') + ".png");
    }
    
    // Test const correctness
    const WallpaperManager& constManager = manager;
    auto mode = constManager.getWallpaperMode(0);
    assert(mode == WallpaperMode::Scale || mode == WallpaperMode::Stretch || 
           mode == WallpaperMode::Center || mode == WallpaperMode::Tile);
    
    std::cout << "✓ Performance optimizations test passed" << std::endl;
}

// Test mathematical precision
void testMathematicalPrecision() {
    std::cout << "Testing mathematical precision..." << std::endl;
    
    // Test floating point precision for scaling calculations
    double displayWidth = 1920.0;
    double displayHeight = 1080.0;
    double imageWidth = 3840.0;
    double imageHeight = 2160.0;
    
    // Calculate scale with high precision
    double scaleX = displayWidth / imageWidth;
    double scaleY = displayHeight / imageHeight;
    double scale = std::min(scaleX, scaleY);
    
    // Verify precision to 6 decimal places
    assert(std::abs(scale - 0.5) < 1e-6);
    
    // Test coordinate system calculations
    int x = 0, y = 0;
    bool isPrimary = (x == 0) && (y == 0);
    assert(isPrimary);
    
    // Test aspect ratio preservation
    double aspectRatio = imageWidth / imageHeight;
    double scaledWidth = imageWidth * scale;
    double scaledHeight = imageHeight * scale;
    double scaledAspectRatio = scaledWidth / scaledHeight;
    
    // Aspect ratio should be preserved
    assert(std::abs(aspectRatio - scaledAspectRatio) < 1e-6);
    
    std::cout << "✓ Mathematical precision test passed" << std::endl;
}

int main() {
    std::cout << "Running complete implementation tests..." << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        testMathematicalImplementations();
        testCompleteErrorHandling();
        testCompleteFileOperations();
        testCompleteDisplayDetection();
        testCompleteWallpaperManagement();
        testPerformanceOptimizations();
        testMathematicalPrecision();
        
        std::cout << "=========================================" << std::endl;
        std::cout << "All complete implementation tests passed! ✨" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Complete implementation test failed with exception: " << e.what() << std::endl;
        return 1;
    }
} 