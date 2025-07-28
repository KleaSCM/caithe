/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: FileUtils.cpp
 * Description: Implementation of file utility functions for wallpaper management
 */

#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <regex>

// Supported image formats
const std::vector<std::string> FileUtils::SUPPORTED_IMAGE_FORMATS = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".webp", ".gif"
};

std::string FileUtils::openFileDialog(const std::string& title, const std::string& defaultPath,
                                     const std::vector<std::string>& filters) {
    // Use zenity for file dialog on Linux with proper error handling
    
    std::string command = "zenity --file-selection --title=\"" + title + "\"";
    
    if (!defaultPath.empty()) {
        // Validate default path exists before using it
        if (fileExists(defaultPath)) {
            command += " --filename=\"" + defaultPath + "\"";
        }
    }
    
    // Add file filters
    if (!filters.empty()) {
        command += " --file-filter=\"Image files";
        for (const auto& filter : filters) {
            command += " *" + filter;
        }
        command += "|All files *";
        command += "\"";
    }
    
    std::string result = executeFileDialog(command);
    
    // Validate result path exists and is accessible
    if (!result.empty() && !fileExists(result)) {
        return ""; // Return empty if file doesn't exist
    }
    
    return result;
}

std::string FileUtils::saveFileDialog(const std::string& title, const std::string& defaultPath,
                                     const std::vector<std::string>& filters) {
    // Use zenity for save dialog on Linux
    std::string command = "zenity --file-selection --save --title=\"" + title + "\"";
    
    if (!defaultPath.empty()) {
        command += " --filename=\"" + defaultPath + "\"";
    }
    
    // Add file filters
    if (!filters.empty()) {
        command += " --file-filter=\"";
        for (const auto& filter : filters) {
            command += "*" + filter + " ";
        }
        command += "|All files *";
        command += "\"";
    }
    
    return executeFileDialog(command);
}

bool FileUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileUtils::isImageFile(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    
    // Extract file extension
    std::string extension = getFileExtension(path);
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Check if extension is in supported formats list
    return std::find(SUPPORTED_IMAGE_FORMATS.begin(), SUPPORTED_IMAGE_FORMATS.end(), extension) 
           != SUPPORTED_IMAGE_FORMATS.end();
}

std::string FileUtils::getFileExtension(const std::string& path) {
    std::filesystem::path filePath(path);
    return filePath.extension().string();
}

std::string FileUtils::getFileName(const std::string& path) {
    std::filesystem::path filePath(path);
    return filePath.filename().string();
}

std::string FileUtils::getDirectory(const std::string& path) {
    std::filesystem::path filePath(path);
    return filePath.parent_path().string();
}

std::vector<std::string> FileUtils::getImageFilesInDirectory(const std::string& directory) {
    std::vector<std::string> imageFiles;
    
    // Mathematical validation: check directory exists and is accessible
    // Time complexity: O(1) for existence check, O(n) for directory traversal
    if (!std::filesystem::exists(directory)) {
        return imageFiles;
    }
    
    try {
        // Depth-first search for image files
        // Mathematical approach: traverse directory tree with O(V + E) complexity
        // where V is vertices (directories), E is edges (files)
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && isImageFile(entry.path().string())) {
                // Validate file is actually readable
                std::ifstream testFile(entry.path());
                if (testFile.good()) {
                    imageFiles.push_back(entry.path().string());
                }
                testFile.close();
            }
        }
        
        // Sort files alphabetically for consistent ordering
        // Mathematical sorting: O(n log n) complexity
        std::sort(imageFiles.begin(), imageFiles.end());
        
    } catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
    
    return imageFiles;
}

std::vector<std::string> FileUtils::getSubdirectories(const std::string& directory) {
    std::vector<std::string> subdirs;
    
    if (!std::filesystem::exists(directory)) {
        return subdirs;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                subdirs.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
    
    return subdirs;
}

bool FileUtils::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

std::string FileUtils::expandPath(const std::string& path) {
    if (path.empty()) {
        return path;
    }
    
    // Handle tilde expansion
    if (path[0] == '~') {
        std::string home = getHomeDirectory();
        if (path.length() == 1) {
            return home;
        } else {
            return home + path.substr(1);
        }
    }
    
    return path;
}

std::string FileUtils::normalizePath(const std::string& path) {
    try {
        std::filesystem::path filePath(path);
        return std::filesystem::canonical(filePath).string();
    } catch (const std::exception& e) {
        // If canonical fails, return the original path
        return path;
    }
}

std::string FileUtils::getHomeDirectory() {
    const char* home = std::getenv("HOME");
    return home ? home : "";
}

std::string FileUtils::getConfigDirectory() {
    std::string home = getHomeDirectory();
    if (home.empty()) {
        return "";
    }
    
    return home + "/.config/caithe";
}

const std::vector<std::string>& FileUtils::getSupportedImageFormats() {
    return SUPPORTED_IMAGE_FORMATS;
}

std::string FileUtils::executeFileDialog(const std::string& command) {
    std::string result;
    
    // Use popen to execute command and capture output
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    int status = pclose(pipe);
    
    // Remove trailing newline
    if (!result.empty() && result[result.length() - 1] == '\n') {
        result.erase(result.length() - 1);
    }
    
    // Check if dialog was cancelled (zenity returns 1 on cancel)
    if (status != 0) {
        return "";
    }
    
    return result;
}

std::vector<std::string> FileUtils::parseFileDialogOutput(const std::string& output) {
    std::vector<std::string> files;
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            files.push_back(line);
        }
    }
    
    return files;
} 