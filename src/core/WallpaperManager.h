/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: WallpaperManager.h
 * Description: Core wallpaper management functionality for Hyprland with mathematical scaling algorithms
 * 
 * Mathematical Foundation:
 * - Wallpaper scaling uses aspect ratio preservation: scale = min(width_display/width_image, height_display/height_image)
 * - Centering calculation: offset_x = (display_width - image_width) / 2, offset_y = (display_height - image_height) / 2
 * - Tiling algorithm: tiles_x = ceil(display_width / image_width), tiles_y = ceil(display_height / image_height)
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// Forward declarations
struct Display;

enum class WallpaperMode {
    Stretch,    // Stretch to fill entire display
    Center,     // Center without scaling
    Tile,       // Tile across display
    Scale       // Scale to fit while maintaining aspect ratio
};

struct WallpaperInfo {
    std::string path;
    WallpaperMode mode;
    int displayId;
    int width;
    int height;
    std::string format;  // PNG, JPG, etc.
};

class WallpaperManager {
public:
    WallpaperManager();
    ~WallpaperManager();
    
    // Core wallpaper operations with move semantics for performance
    bool setWallpaper(std::string path, int displayId = 0);  // Take by value for move semantics
    bool setWallpaperAllDisplays(const std::string& path);
    bool removeWallpaper(int displayId = 0);
    bool removeAllWallpapers();
    
    // Wallpaper mode operations
    bool setWallpaperMode(int displayId, WallpaperMode mode);
    WallpaperMode getWallpaperMode(int displayId) const;
    
    // Information queries with const references for efficiency
    const std::string& getCurrentWallpaper(int displayId = 0) const;
    const WallpaperInfo& getWallpaperInfo(int displayId = 0) const;
    const std::vector<WallpaperInfo>& getAllWallpapers() const;
    
    // File operations
    bool isValidImageFile(const std::string& path) const;
    std::vector<std::string> getSupportedFormats() const;
    
    // Hyprland integration
    bool applyToHyprland(int displayId = 0);
    bool applyToAllHyprlandDisplays();
    
    // Error handling with detailed error codes
    std::string getLastError() const;
    void clearError();
    
    // Error codes for detailed error handling
    enum class ErrorCode {
        None = 0,
        InvalidPath = 1,
        FileNotFound = 2,
        UnsupportedFormat = 3,
        HyprlandCommandFailed = 4,
        DisplayNotFound = 5,
        InvalidDisplayId = 6,
        SystemError = 7
    };
    
    ErrorCode getLastErrorCode() const;
    
    // Mathematical scaling algorithms (public for testing)
    double calculateAspectRatioScale(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const;
    std::pair<int, int> calculateCenteringOffsets(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const;
    std::pair<int, int> calculateTilingCount(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const;

private:
    // Internal helper methods
    bool validateImageFile(const std::string& path) const;
    std::string createHyprlandCommand(const std::string& path, int displayId, WallpaperMode mode) const;
    std::string getHyprlandDisplayName(int displayId) const;
    
    // Wallpaper storage
    std::unordered_map<int, WallpaperInfo> m_wallpapers;
    std::string m_lastError;
    ErrorCode m_lastErrorCode;
    
    // Mutable cache for getAllWallpapers() efficiency
    mutable std::vector<WallpaperInfo> m_allWallpapersCache;
    mutable bool m_cacheValid;
    
    // Supported image formats
    static const std::vector<std::string> SUPPORTED_FORMATS;
    
    // Hyprland configuration
    static constexpr const char* HYPRLAND_CONFIG_DIR = "~/.config/hypr";
    static constexpr const char* HYPRLAND_WALLPAPER_SCRIPT = "~/.config/hypr/wallpaper.sh";
}; 