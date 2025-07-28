/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * File: main.cpp
 * Description: Main entry point for Caithe Wallpaper Manager - A modern wallpaper manager for Hyprland
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include "ui/Application.h"

int main(int argc, char** argv) {
    try {
        // Create and run the wallpaper manager application
        auto app = std::make_unique<Application>();
        return app->run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
