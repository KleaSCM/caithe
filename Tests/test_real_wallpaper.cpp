/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_real_wallpaper.cpp
 * Description: Real wallpaper testing with actual image files and Hyprland integration
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include "core/WallpaperManager.h"
#include "core/DisplayManager.h"

int main() {
    std::cout << "🖼️  Testing Caithe Wallpaper Manager with Real Images" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    try {
        WallpaperManager wallpaperManager;
        
        // Test with a real image file
        std::string testImage = "/usr/share/pixmaps/archlinux-logo.png";
        
        std::cout << "\n📋 Testing wallpaper validation..." << std::endl;
        std::cout << "  Testing image: " << testImage << std::endl;
        
        if (wallpaperManager.isValidImageFile(testImage)) {
            std::cout << "  ✓ Image is valid!" << std::endl;
            
            // Test setting wallpaper
            std::cout << "\n🎨 Testing wallpaper setting..." << std::endl;
            if (wallpaperManager.setWallpaper(testImage, 0)) {
                std::cout << "  ✓ Wallpaper set successfully!" << std::endl;
                
                // Test getting wallpaper info
                const auto& info = wallpaperManager.getWallpaperInfo(0);
                std::cout << "  Wallpaper info:" << std::endl;
                std::cout << "    Path: " << info.path << std::endl;
                std::cout << "    Dimensions: " << info.width << "x" << info.height << std::endl;
                std::cout << "    Format: " << info.format << std::endl;
                std::cout << "    Display ID: " << info.displayId << std::endl;
                
                // Test mathematical calculations with real dimensions
                std::cout << "\n🧮 Testing mathematical calculations with real image..." << std::endl;
                double scale = wallpaperManager.calculateAspectRatioScale(1920, 1080, info.width, info.height);
                std::cout << "  Aspect ratio scale: " << scale << std::endl;
                
                auto [offsetX, offsetY] = wallpaperManager.calculateCenteringOffsets(1920, 1080, info.width, info.height);
                std::cout << "  Centering offsets: (" << offsetX << ", " << offsetY << ")" << std::endl;
                
                auto [tilesX, tilesY] = wallpaperManager.calculateTilingCount(1920, 1080, info.width, info.height);
                std::cout << "  Tiling count: (" << tilesX << ", " << tilesY << ")" << std::endl;
                
            } else {
                std::cout << "  ✗ Failed to set wallpaper: " << wallpaperManager.getLastError() << std::endl;
            }
        } else {
            std::cout << "  ✗ Image is not valid: " << wallpaperManager.getLastError() << std::endl;
        }
        
        // Test Hyprland integration
        std::cout << "\n🖥️  Testing Hyprland integration..." << std::endl;
        if (wallpaperManager.applyToHyprland(0)) {
            std::cout << "  ✓ Successfully applied to Hyprland!" << std::endl;
        } else {
            std::cout << "  ✗ Failed to apply to Hyprland: " << wallpaperManager.getLastError() << std::endl;
        }
        
        std::cout << "\n🎉 Wallpaper manager test completed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
} 