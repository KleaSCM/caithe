# Caithe Wallpaper Manager - Architecture Documentation

## Overview

Caithe is a modern wallpaper manager designed specifically for Hyprland on Arch Linux. The application uses a modular architecture with clear separation of concerns.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                       │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │   Application   │  │   ImGui UI      │  │   Main      │ │
│  │   (src/ui/)     │  │   Components    │  │   Entry     │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    Core Business Logic                     │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │ WallpaperManager│  │ DisplayManager  │  │ Settings    │ │
│  │ (src/core/)     │  │ (src/core/)     │  │ Manager     │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    Utility Layer                           │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │   FileUtils     │  │   ImageUtils    │  │   Config    │ │
│  │ (src/utils/)    │  │ (src/utils/)    │  │   Utils     │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    External Dependencies                   │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │    Hyprland     │  │     ImGui       │  │    GLFW     │ │
│  │   (hyprctl)     │  │   (UI Library)  │  │ (Windowing) │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Application Layer (`src/ui/`)

**Application Class**
- **Purpose**: Main application controller and window management
- **Responsibilities**:
  - Initialize GLFW window and OpenGL context
  - Setup ImGui integration
  - Handle main application loop
  - Coordinate between UI and business logic
  - Manage application lifecycle

**Key Methods**:
```cpp
class Application {
    int run();                    // Main application loop
    bool initializeWindow();       // GLFW window setup
    bool initializeImGui();        // ImGui context setup
    void renderFrame();           // Main rendering loop
    void renderMainWindow();      // Main UI rendering
};
```

### 2. Core Business Logic (`src/core/`)

**WallpaperManager Class**
- **Purpose**: Core wallpaper management functionality
- **Responsibilities**:
  - Set/remove wallpapers on displays
  - Manage wallpaper modes (Stretch, Center, Tile, Scale)
  - Integrate with Hyprland's wallpaper system
  - Validate image files
  - Handle wallpaper information storage

**Key Methods**:
```cpp
class WallpaperManager {
    bool setWallpaper(const std::string& path, int displayId);
    bool setWallpaperAllDisplays(const std::string& path);
    bool setWallpaperMode(int displayId, WallpaperMode mode);
    bool applyToHyprland(int displayId);
    bool isValidImageFile(const std::string& path) const;
};
```

**DisplayManager Class**
- **Purpose**: Display detection and management
- **Responsibilities**:
  - Detect connected displays
  - Parse Hyprland display information
  - Provide display information to UI
  - Handle multi-monitor setups

**Key Methods**:
```cpp
class DisplayManager {
    bool refreshDisplays();
    std::vector<Display> getDisplays() const;
    Display getPrimaryDisplay() const;
    bool queryHyprlandDisplays();
};
```

### 3. Utility Layer (`src/utils/`)

**FileUtils Class**
- **Purpose**: File system operations and dialogs
- **Responsibilities**:
  - File dialog operations (open/save)
  - File validation and type checking
  - Directory operations
  - Path utilities and expansion

**Key Methods**:
```cpp
class FileUtils {
    static std::string openFileDialog(...);
    static bool isImageFile(const std::string& path);
    static std::vector<std::string> getImageFilesInDirectory(...);
    static std::string expandPath(const std::string& path);
};
```

## Data Flow

### 1. Application Startup
```
main() → Application::Application() → initializeWindow() → initializeImGui()
```

### 2. Wallpaper Setting Process
```
UI Button Click → FileUtils::openFileDialog() → WallpaperManager::setWallpaper() 
→ WallpaperManager::applyToHyprland() → hyprctl command execution
```

### 3. Display Detection Process
```
Application::Application() → DisplayManager::DisplayManager() 
→ DisplayManager::refreshDisplays() → DisplayManager::queryHyprlandDisplays()
→ parseHyprlandOutput() → UI display list population
```

## Design Patterns

### 1. Singleton Pattern
- **Application**: Single application instance
- **DisplayManager**: Single display manager instance
- **WallpaperManager**: Single wallpaper manager instance

### 2. Observer Pattern
- **UI Components**: Observe wallpaper/display changes
- **Settings**: Observe configuration changes

### 3. Strategy Pattern
- **Wallpaper Modes**: Different scaling strategies
- **File Dialogs**: Different dialog implementations

## Error Handling

### 1. Exception Safety
- All classes provide strong exception guarantees
- RAII principles for resource management
- Smart pointers for memory management

### 2. Error Reporting
- `getLastError()` methods for detailed error information
- `clearError()` for error state management
- Comprehensive error messages for debugging

### 3. Graceful Degradation
- Fallback to default displays if detection fails
- Continue operation with partial functionality
- User-friendly error messages

## Performance Considerations

### 1. Memory Management
- Smart pointers for automatic memory management
- RAII for resource cleanup
- Efficient string handling with move semantics

### 2. UI Responsiveness
- Non-blocking file operations
- Asynchronous display detection
- Efficient ImGui rendering

### 3. Hyprland Integration
- Minimal command execution overhead
- Efficient display information parsing
- Cached display information

## Security Considerations

### 1. File Operations
- Path validation and sanitization
- File type verification
- Safe file dialog operations

### 2. Command Execution
- Sanitized command construction
- Safe system call execution
- Input validation

## Testing Strategy

### 1. Unit Tests
- Individual component testing
- Mock objects for external dependencies
- Comprehensive error condition testing

### 2. Integration Tests
- End-to-end workflow testing
- Hyprland integration testing
- UI interaction testing

### 3. Performance Tests
- Memory usage monitoring
- UI responsiveness testing
- Large file handling testing

## Future Enhancements

### 1. Planned Features
- Wallpaper slideshow functionality
- Advanced image processing
- Plugin system for custom effects
- Network wallpaper sources

### 2. Architecture Improvements
- Plugin architecture for extensibility
- Configuration management system
- Logging and debugging framework
- Performance monitoring

---

**Author**: KleaSCM  
**Email**: KleaSCM@gmail.com  
**Last Updated**: July 2024 