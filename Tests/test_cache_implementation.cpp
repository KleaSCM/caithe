/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: test_cache_implementation.cpp
 * Description: Test for fully implemented getAllWallpapers method with proper caching
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../src/core/WallpaperManager.h"

void testCacheImplementation() {
    std::cout << "Testing fully implemented getAllWallpapers with caching..." << std::endl;
    
    WallpaperManager manager;
    
    // Test initial state
    const auto& initialWallpapers = manager.getAllWallpapers();
    assert(initialWallpapers.empty());
    std::cout << "  ✓ Initial state: empty wallpapers list" << std::endl;
    
    // Test adding wallpapers
    assert(manager.setWallpaper("test1.png", 0));
    const auto& wallpapers1 = manager.getAllWallpapers();
    assert(wallpapers1.size() == 1);
    assert(wallpapers1[0].path == "test1.png");
    assert(wallpapers1[0].displayId == 0);
    std::cout << "  ✓ Added wallpaper to display 0" << std::endl;
    
    // Test adding second wallpaper
    assert(manager.setWallpaper("test2.png", 1));
    const auto& wallpapers2 = manager.getAllWallpapers();
    assert(wallpapers2.size() == 2);
    assert(wallpapers2[0].path == "test1.png");
    assert(wallpapers2[1].path == "test2.png");
    std::cout << "  ✓ Added wallpaper to display 1" << std::endl;
    
    // Test cache consistency - same reference should be returned
    const auto& wallpapers2_again = manager.getAllWallpapers();
    assert(&wallpapers2 == &wallpapers2_again); // Should return same reference
    std::cout << "  ✓ Cache consistency verified" << std::endl;
    
    // Test removing wallpaper
    assert(manager.removeWallpaper(0));
    const auto& wallpapers3 = manager.getAllWallpapers();
    assert(wallpapers3.size() == 1);
    assert(wallpapers3[0].path == "test2.png");
    assert(wallpapers3[0].displayId == 1);
    std::cout << "  ✓ Removed wallpaper from display 0" << std::endl;
    
    // Test changing wallpaper mode
    assert(manager.setWallpaperMode(1, WallpaperMode::Stretch));
    const auto& wallpapers4 = manager.getAllWallpapers();
    assert(wallpapers4.size() == 1);
    assert(wallpapers4[0].mode == WallpaperMode::Stretch);
    std::cout << "  ✓ Changed wallpaper mode" << std::endl;
    
    // Test removing all wallpapers
    assert(manager.removeAllWallpapers());
    const auto& wallpapers5 = manager.getAllWallpapers();
    assert(wallpapers5.empty());
    std::cout << "  ✓ Removed all wallpapers" << std::endl;
    
    // Test performance - multiple rapid calls
    assert(manager.setWallpaper("performance_test.png", 0));
    for (int i = 0; i < 1000; ++i) {
        const auto& perfTest = manager.getAllWallpapers();
        assert(perfTest.size() == 1);
        assert(perfTest[0].path == "performance_test.png");
    }
    std::cout << "  ✓ Performance test: 1000 rapid calls completed" << std::endl;
    
    std::cout << "✓ Cache implementation test passed" << std::endl;
}

int main() {
    std::cout << "Testing fully implemented getAllWallpapers method..." << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        testCacheImplementation();
        
        std::cout << "=================================================" << std::endl;
        std::cout << "All cache implementation tests passed! ✨" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Cache implementation test failed with exception: " << e.what() << std::endl;
        return 1;
    }
} 