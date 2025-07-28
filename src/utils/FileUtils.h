/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: FileUtils.h
 * Description: File utility functions for wallpaper management with path normalization algorithms
 * 
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <filesystem>

class FileUtils {
public:
    // File dialog operations
    static std::string openFileDialog(const std::string& title = "Select Wallpaper",
                                     const std::string& defaultPath = "",
                                     const std::vector<std::string>& filters = {});
    
    static std::string saveFileDialog(const std::string& title = "Save Settings",
                                     const std::string& defaultPath = "",
                                     const std::vector<std::string>& filters = {});
    
    // File operations
    static bool fileExists(const std::string& path);
    static bool isImageFile(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getDirectory(const std::string& path);
    
    // Directory operations
    static std::vector<std::string> getImageFilesInDirectory(const std::string& directory);
    static std::vector<std::string> getSubdirectories(const std::string& directory);
    static bool createDirectory(const std::string& path);
    
    // Path utilities
    static std::string expandPath(const std::string& path);
    static std::string normalizePath(const std::string& path);
    static std::string getHomeDirectory();
    static std::string getConfigDirectory();
    
    // Supported image formats
    static const std::vector<std::string>& getSupportedImageFormats();
    
private:
    // Internal helper methods
    static std::string executeFileDialog(const std::string& command);
    static std::vector<std::string> parseFileDialogOutput(const std::string& output);
    
    // Supported formats
    static const std::vector<std::string> SUPPORTED_IMAGE_FORMATS;
}; 