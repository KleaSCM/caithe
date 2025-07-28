# Caithe Wallpaper Manager

A modern, elegant wallpaper manager designed specifically for Hyprland on Arch Linux. Built with C++ and ImGui for a beautiful, responsive interface.

## Features

- **Hyprland Integration**: Native support for Hyprland's wallpaper system
- **Multi-Display Support**: Manage wallpapers across multiple monitors
- **Modern UI**: Clean, intuitive interface built with ImGui
- **Multiple Wallpaper Modes**: Stretch, Center, Tile, and Scale options
- **File Format Support**: PNG, JPG, JPEG, BMP, TIFF, WebP, GIF
- **Display Detection**: Automatic detection of connected displays
- **Settings Persistence**: Remember your preferences and last used wallpapers

## Requirements

- **OS**: Arch Linux (or other Linux distributions)
- **Window Manager**: Hyprland
- **Dependencies**:
  - xmake (build system)
  - OpenGL 3.3+
  - GLFW
  - ImGui
  - zenity (for file dialogs)

## Installation

### Prerequisites

```bash
# Install required packages
sudo pacman -S xmake glfw imgui zenity

# For development
sudo pacman -S base-devel
```

### Building

```bash
# Clone the repository
git clone https://github.com/KleaSCM/caithe.git
cd caithe

# Build the project
xmake

# Run the application
xmake run
```

## Usage

### Basic Usage

1. Launch Caithe Wallpaper Manager
2. Click "Load Wallpaper" to select an image file
3. Choose your target display from the display panel
4. Select your preferred wallpaper mode (Stretch, Center, Tile, Scale)
5. Click "Apply to Selected Display" or "Apply to All Displays"

### Wallpaper Modes

- **Stretch**: Stretches the image to fill the entire display
- **Center**: Centers the image without scaling
- **Tile**: Tiles the image across the display
- **Scale**: Scales the image to fit while maintaining aspect ratio

### Display Management

The application automatically detects your connected displays and shows them in the display panel. You can:

- Select individual displays for wallpaper application
- View display information (resolution, refresh rate, connector type)
- Apply different wallpapers to different displays

## Configuration

### Settings

- **Auto-start with system**: Automatically launch Caithe on system startup
- **Remember last wallpaper**: Restore the last used wallpaper on startup
- **Wallpaper mode**: Set the default wallpaper scaling mode

### Hyprland Integration

Caithe integrates directly with Hyprland's wallpaper system using `hyprctl hyprpaper`. The application:

- Automatically detects Hyprland displays
- Uses proper Hyprland display names (DP-1, HDMI-A-1, etc.)
- Supports all Hyprland wallpaper modes

## Development

### Project Structure

```
caithe/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── ui/
│   │   ├── Application.h     # Main application class
│   │   └── Application.cpp   # Application implementation
│   ├── core/
│   │   ├── WallpaperManager.h    # Wallpaper management
│   │   ├── WallpaperManager.cpp  # Wallpaper implementation
│   │   ├── DisplayManager.h      # Display detection
│   │   └── DisplayManager.cpp    # Display implementation
│   └── utils/
│       ├── FileUtils.h       # File operations
│       └── FileUtils.cpp     # File utilities
├── Tests/                    # Unit tests
├── Docs/                     # Documentation
├── xmake.lua                 # Build configuration
└── README.md                 # This file
```

### Building for Development

```bash
# Debug build
xmake -m debug

# Release build
xmake -m release

# Clean build
xmake clean
```

### Adding Features

1. **New Wallpaper Modes**: Add to `WallpaperMode` enum in `WallpaperManager.h`
2. **File Format Support**: Add to `SUPPORTED_FORMATS` in `WallpaperManager.cpp`
3. **UI Components**: Add to `Application.cpp` render methods
4. **Display Detection**: Extend `DisplayManager.cpp` parsing methods

## Troubleshooting

### Common Issues

**Application won't start**
- Ensure Hyprland is running
- Check that all dependencies are installed
- Verify OpenGL support

**Wallpapers not applying**
- Check Hyprland is running: `hyprctl monitors`
- Verify hyprpaper is installed: `pacman -Q hyprpaper`
- Check file permissions on wallpaper images

**Display detection issues**
- Run `hyprctl monitors` to verify display detection
- Check X11 fallback: `xrandr --listmonitors`

### Debug Mode

```bash
# Build with debug information
xmake -m debug

# Run with verbose output
xmake run -v
```

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes
4. Add tests in the `Tests/` directory
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- **Hyprland**: For the excellent Wayland compositor
- **ImGui**: For the beautiful immediate mode GUI library
- **xmake**: For the modern build system

---

**Author**: KleaSCM  
**Email**: KleaSCM@gmail.com
