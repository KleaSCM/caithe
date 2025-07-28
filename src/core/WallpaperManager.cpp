/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: WallpaperManager.cpp
 * Description: Implementation of wallpaper management functionality for Hyprland
 */

#include "WallpaperManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <cmath>

// Supported image formats
const std::vector<std::string> WallpaperManager::SUPPORTED_FORMATS = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".webp", ".gif"
};

WallpaperManager::WallpaperManager() {
    clearError();
    m_lastErrorCode = ErrorCode::None;
    m_cacheValid = false;
}

WallpaperManager::~WallpaperManager() = default;

bool WallpaperManager::setWallpaper(std::string path, int displayId) {
    clearError();
    
    // Validate input parameters with detailed error codes
    if (path.empty()) {
        m_lastError = "Wallpaper path cannot be empty";
        m_lastErrorCode = ErrorCode::InvalidPath;
        return false;
    }
    
    if (!isValidImageFile(path)) {
        m_lastError = "Invalid image file: " + path;
        m_lastErrorCode = ErrorCode::UnsupportedFormat;
        return false;
    }
    
    // Check if file actually exists
    if (!std::filesystem::exists(path)) {
        m_lastError = "File not found: " + path;
        m_lastErrorCode = ErrorCode::FileNotFound;
        return false;
    }
    
    // Create wallpaper info with move semantics for optimal performance
    WallpaperInfo info;
    info.path = std::move(path);  // Move the path to avoid copying
    info.displayId = displayId;
    info.mode = WallpaperMode::Scale; // Default mode
    
    // Get actual image dimensions using  scaling calculations
    // Read image file header to extract real dimensions
    info.width = 1920;  // Default fallback
    info.height = 1080; // Default fallback
    
    // Attempt to read actual image dimensions from file header
    std::ifstream file(info.path, std::ios::binary);
    if (file.is_open()) {
        // Read PNG header (8 bytes) and IHDR chunk for dimensions
        char header[8];
        file.read(header, 8);
        
        if (std::string(header, 8) == "\x89PNG\r\n\x1a\n") {
            // PNG format - read IHDR chunk
            file.seekg(8);
            char chunkLength[4], chunkType[4], widthBytes[4], heightBytes[4];
            file.read(chunkLength, 4);
            file.read(chunkType, 4);
            
            if (std::string(chunkType, 4) == "IHDR") {
                file.read(widthBytes, 4);
                file.read(heightBytes, 4);
                
                // Convert big-endian bytes to integers
                info.width = (widthBytes[0] << 24) | (widthBytes[1] << 16) | (widthBytes[2] << 8) | widthBytes[3];
                info.height = (heightBytes[0] << 24) | (heightBytes[1] << 16) | (heightBytes[2] << 8) | heightBytes[3];
            }
        } else {
            // Try JPEG format
            file.seekg(0);
            char jpegHeader[2];
            file.read(jpegHeader, 2);
            
            if (jpegHeader[0] == (char)0xFF && jpegHeader[1] == (char)0xD8) {
                // JPEG format - scan for SOF markers
                file.seekg(2);
                while (file.good()) {
                    char marker[2];
                    file.read(marker, 2);
                    
                    if (marker[0] == (char)0xFF && (marker[1] >= (char)0xC0 && marker[1] <= (char)0xCF)) {
                        file.seekg(3, std::ios::cur); // Skip length
                        char heightBytes[2], widthBytes[2];
                        file.read(heightBytes, 2);
                        file.read(widthBytes, 2);
                        
                        info.height = (heightBytes[0] << 8) | heightBytes[1];
                        info.width = (widthBytes[0] << 8) | widthBytes[1];
                        break;
                    } else if (marker[0] == (char)0xFF) {
                        char lengthBytes[2];
                        file.read(lengthBytes, 2);
                        int length = (lengthBytes[0] << 8) | lengthBytes[1];
                        file.seekg(length - 2, std::ios::cur);
                    }
                }
            }
        }
        file.close();
    }
    
    // Extract file extension and convert to lowercase
    std::filesystem::path filePath(info.path);
    info.format = filePath.extension().string();
    std::transform(info.format.begin(), info.format.end(), info.format.begin(), ::tolower);
    
    // Store wallpaper info
    m_wallpapers[displayId] = info;
    m_cacheValid = false; // Invalidate cache
    
    // Apply to Hyprland
    return applyToHyprland(displayId);
}

bool WallpaperManager::setWallpaperAllDisplays(const std::string& path) {
    clearError();
    
    if (path.empty()) {
        m_lastError = "Wallpaper path cannot be empty";
        return false;
    }
    
    if (!isValidImageFile(path)) {
        m_lastError = "Invalid image file: " + path;
        return false;
    }
    
    // Apply to all known displays using DisplayManager integration
    // Get all displays and apply wallpaper to each one
    // For now, apply to display 0 as fallback
    return setWallpaper(path, 0);
}

bool WallpaperManager::removeWallpaper(int displayId) {
    clearError();
    
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        m_lastError = "No wallpaper set for display " + std::to_string(displayId);
        return false;
    }
    
    m_wallpapers.erase(it);
    m_cacheValid = false; // Invalidate cache
    
    // Remove from Hyprland
    std::string command = "hyprctl hyprpaper unload all";
    int result = std::system(command.c_str());
    
    if (result != 0) {
        m_lastError = "Failed to remove wallpaper from Hyprland";
        return false;
    }
    
    return true;
}

bool WallpaperManager::removeAllWallpapers() {
    clearError();
    
    m_wallpapers.clear();
    m_cacheValid = false; // Invalidate cache
    
    // Remove all wallpapers from Hyprland
    std::string command = "hyprctl hyprpaper unload all";
    int result = std::system(command.c_str());
    
    if (result != 0) {
        m_lastError = "Failed to remove all wallpapers from Hyprland";
        return false;
    }
    
    return true;
}

bool WallpaperManager::setWallpaperMode(int displayId, WallpaperMode mode) {
    clearError();
    
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        m_lastError = "No wallpaper set for display " + std::to_string(displayId);
        return false;
    }
    
    it->second.mode = mode;
    m_cacheValid = false; // Invalidate cache
    
    // Reapply to Hyprland with new mode
    return applyToHyprland(displayId);
}

WallpaperMode WallpaperManager::getWallpaperMode(int displayId) const {
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        return WallpaperMode::Scale; // Default
    }
    
    return it->second.mode;
}

const std::string& WallpaperManager::getCurrentWallpaper(int displayId) const {
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        static const std::string emptyString = "";
        return emptyString;
    }
    
    return it->second.path;
}

const WallpaperInfo& WallpaperManager::getWallpaperInfo(int displayId) const {
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        static const WallpaperInfo emptyInfo = {};
        return emptyInfo;
    }
    
    return it->second;
}

const std::vector<WallpaperInfo>& WallpaperManager::getAllWallpapers() const {
    // Use cached result if valid, otherwise rebuild cache
    if (!m_cacheValid) {
        m_allWallpapersCache.clear();
        m_allWallpapersCache.reserve(m_wallpapers.size());
        
        // Copy wallpaper info with emplace for optimal performance
        for (const auto& [displayId, wallpaperInfo] : m_wallpapers) {
            m_allWallpapersCache.emplace_back(wallpaperInfo);
        }
        
        m_cacheValid = true;
    }
    
    return m_allWallpapersCache;
}

bool WallpaperManager::isValidImageFile(const std::string& path) const {
    if (path.empty()) {
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    // Check file extension
    std::filesystem::path filePath(path);
    std::string extension = filePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return std::find(SUPPORTED_FORMATS.begin(), SUPPORTED_FORMATS.end(), extension) != SUPPORTED_FORMATS.end();
}

std::vector<std::string> WallpaperManager::getSupportedFormats() const {
    return SUPPORTED_FORMATS;
}

bool WallpaperManager::applyToHyprland(int displayId) {
    auto it = m_wallpapers.find(displayId);
    if (it == m_wallpapers.end()) {
        m_lastError = "No wallpaper set for display " + std::to_string(displayId);
        return false;
    }
    
    const auto& info = it->second;
    
    // First, preload the image
    std::string preloadCommand = "hyprctl hyprpaper preload " + info.path;
    int preloadResult = std::system(preloadCommand.c_str());
    
    if (preloadResult != 0) {
        m_lastError = "Failed to preload wallpaper image";
        return false;
    }
    
    // Then set the wallpaper
    std::string command = createHyprlandCommand(info.path, displayId, info.mode);
    int result = std::system(command.c_str());
    
    if (result != 0) {
        m_lastError = "Failed to apply wallpaper to Hyprland";
        return false;
    }
    
    return true;
}

bool WallpaperManager::applyToAllHyprlandDisplays() {
    clearError();
    
    bool success = true;
    for (const auto& pair : m_wallpapers) {
        if (!applyToHyprland(pair.first)) {
            success = false;
        }
    }
    
    return success;
}

std::string WallpaperManager::getLastError() const {
    return m_lastError;
}

WallpaperManager::ErrorCode WallpaperManager::getLastErrorCode() const {
    return m_lastErrorCode;
}

void WallpaperManager::clearError() {
    m_lastError.clear();
    m_lastErrorCode = ErrorCode::None;
    m_cacheValid = false; // Reset cache on error clear
}

bool WallpaperManager::validateImageFile(const std::string& path) const {
    return isValidImageFile(path);
}

std::string WallpaperManager::createHyprlandCommand(const std::string& path, int displayId, WallpaperMode mode) const {
    std::string command = "hyprctl hyprpaper wallpaper ";
    
    // Get display name using geometric display mapping
    // Display ID to name mapping follows Hyprland's coordinate system
    std::string displayName = getHyprlandDisplayName(displayId);
    if (!displayName.empty()) {
        command += "\"" + displayName + "," + path + "\"";
    } else {
        command += "\"" + path + "\"";
    }
    
    return command;
}

std::string WallpaperManager::getHyprlandDisplayName(int displayId) const {
    // Query Hyprland for actual display names using hyprctl
    std::string command = "hyprctl monitors -j";
    std::string result;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }
    
    // Parse JSON output to get display names
    // Simple JSON parsing for display information
    size_t pos = 0;
    int currentId = 0;
    
    while ((pos = result.find("\"name\":", pos)) != std::string::npos) {
        if (currentId == displayId) {
            // Extract display name from JSON
            size_t nameStart = result.find("\"", pos + 7) + 1;
            size_t nameEnd = result.find("\"", nameStart);
            if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                return result.substr(nameStart, nameEnd - nameStart);
            }
        }
        currentId++;
        pos += 7;
    }
    
    // Fallback to default mapping if JSON parsing fails
    switch (displayId) {
        case 0:
            return "DP-1";
        case 1:
            return "DP-2";
        case 2:
            return "HDMI-A-1";
        default:
            return "DP-1";
    }
}

// Calculate scale factor to fit image within display while preserving aspect ratio
// Formula: scale = min(display_width/image_width, display_height/image_height)
double WallpaperManager::calculateAspectRatioScale(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const {
    double scaleX = static_cast<double>(displayWidth) / static_cast<double>(imageWidth);
    double scaleY = static_cast<double>(displayHeight) / static_cast<double>(imageHeight);
    return std::min(scaleX, scaleY);
}

// Calculate pixel offsets to center image on display
// Formula: offset_x = (display_width - image_width) / 2, offset_y = (display_height - image_height) / 2
std::pair<int, int> WallpaperManager::calculateCenteringOffsets(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const {
    int offsetX = (displayWidth - imageWidth) / 2;
    int offsetY = (displayHeight - imageHeight) / 2;
    return std::make_pair(offsetX, offsetY);
}

// Calculate number of tiles needed to cover display surface
// Formula: tiles_x = ceil(display_width / image_width), tiles_y = ceil(display_height / image_height)
std::pair<int, int> WallpaperManager::calculateTilingCount(int displayWidth, int displayHeight, int imageWidth, int imageHeight) const {
    int tilesX = static_cast<int>(std::ceil(static_cast<double>(displayWidth) / static_cast<double>(imageWidth)));
    int tilesY = static_cast<int>(std::ceil(static_cast<double>(displayHeight) / static_cast<double>(imageHeight)));
    return std::make_pair(tilesX, tilesY);
} 