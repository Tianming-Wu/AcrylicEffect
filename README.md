<h1 align="center">WindowsVisualEffect</h1>

Project page: <a href="https://github.com/Tianming-Wu/WindowsVisualEffect" color="#15ff00" target="_blank">GitHub Repository</a>

A C++ library for applying modern Windows visual effects including Acrylic, Mica, and MicaAlt effects to your Windows applications.

>[!NOTE]
> You **should** check the notes at the bottom of this README first, or you might encounter some common issues when using the library.

## Features

- **Acrylic Effect**: Semi-transparent acrylic background with blur
- **Mica Effect**: Mica backdrop that blends with system theme
- **MicaAlt Effect**: Alternative Mica style variant
- **Window Corner Preferences**: Customize window corner styles (default, no round, round, small round)
- **Legacy Blur Effect**: Support for blur effects on older Windows versions using Region
- **Power Mode Monitoring**: Monitor system power mode changes to adjust effects in power-saving mode

## Requirements

- Windows 10 or later (Mica effects require Windows 11)
- Visual Studio 2017 or later (or any compiler with C++17 support)
- CMake 3.18 or later

## CMake Installation Guide

### 1. Build and Install the Library

```bash
# Clone the repository
git clone https://github.com/Tianming-Wu/WindowsVisualEffect.git
cd WindowsVisualEffect

# Create build directory
mkdir build
cd build

# Configure CMake (set installation path)
cmake .. -DCMAKE_INSTALL_PREFIX="C:/Program Files/WindowsVisualEffect"

# Build
cmake --build . --config Release

# Install (requires administrator privileges)
cmake --install . --config Release
```

### 2. Using in Your Project

Add to your CMakeLists.txt:

```cmake
# Find the library
find_package(WindowsVisualEffect REQUIRED)

# Link to your target
target_link_libraries(your_target PRIVATE WindowsVisualEffect)

# (Optional) Automatically copy DLL to output directory
copy_windowsvisualeffect_dll(your_target)
```

**Important**: The `copy_windowsvisualeffect_dll()` function automatically copies `WindowsVisualEffect.dll` to your executable directory after build, ensuring the DLL is found at runtime.

### 3. Manual DLL Management

If you don't use the `copy_windowsvisualeffect_dll()` function, you need to manage the DLL manually:

- **Option 1**: Copy `WindowsVisualEffect.dll` to your executable directory
- **Option 2**: Add the DLL directory to your system PATH environment variable
- **Option 3**: Install the DLL to a system directory (not recommended)

DLL location: `<install_path>/bin/WindowsVisualEffect.dll`

## Usage Examples

### Basic Usage

```cpp
#include <WindowsVisualEffect/visualEffect.h>
#include <windows.h>

int main() {
    // Assuming you have a window handle
    HWND hwnd = /* your window handle */;
    
    // Apply Acrylic effect
    bool success = SetAcrylicEffect(hwnd, EffectType_Acrylic, Corner_Round);
    
    if (!success) {
        // Effect application failed (system may not support it)
    }
    
    return 0;
}
```

### Effect Types

```cpp
// Available effect types
EffectType_Default  // System decides
EffectType_None     // No effect
EffectType_Mica     // Mica effect (Windows 11)
EffectType_Acrylic  // Acrylic effect
EffectType_MicaAlt  // Alternative Mica effect (Windows 11)

// Window corner preferences
Corner_Default      // System default
Corner_NoRound      // No rounding
Corner_Round        // Rounded corners (8px)
Corner_RoundSmall   // Small rounded corners (4px)
```

>[!NOTE]
>It is not possible to have rounded corners that is not **8px** or **4px**, as these values are determined by the system and cannot be customized.

### Monitor Power Mode Changes

```cpp
#include <WindowsVisualEffect/visualEffect.h>

// Set callback function
SetBlurModeChangeCallback([](bool isPowerSaving) {
    if (isPowerSaving) {
        // Entered power saving mode, disable effects to save power
    } else {
        // Normal mode, enable effects
    }
});
```

### Using Legacy Blur Effect (Compatible with Older Windows)

```cpp
// Create rounded region
Region region = Region::createRounded(0, 0, 800, 600, 10); // (x, y, width, height, corner radius)

// Apply blur effect
bool success = SetBlurEffect(hwnd, std::move(region));
```

## Qt Integration Example

```cpp
#include <QWidget>
#include <WindowsVisualEffect/visualEffect.h>

class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // Set window attributes
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Apply Acrylic effect
        SetAcrylicEffect((HWND)winId(), EffectType_Acrylic, Corner_Round);
    }
};
```

## Notes

1. **DLL Dependency**: Applications using this library need runtime access to `WindowsVisualEffect.dll`. It's recommended to use the `copy_windowsvisualeffect_dll()` CMake function for automatic handling.
2. **System Compatibility**: Mica and MicaAlt effects are only available on Windows 11 and will gracefully degrade on older systems.
3. **Window Attributes**: When using visual effects, you typically need to set the window's transparent background attribute.
4. **Debug Build**: In Debug mode, the library file is named `WindowsVisualEffectd.dll` (with a 'd' suffix).

>[!IMPORTANT]
>**About Transparency**: For a good visual effect, your window should have a transparent background. In Qt that is usually achieved by setting the `Qt::WA_TranslucentBackground` attribute.
>
>**BUT** notice that the above Qt method, or "Layered Window" method will regard fully-transparent areas as **"click-through"**, and mouse operations will pass though those areas, and also NOT considering them as part of the window, so hover events will be broken. 
>
> For a good fix, check this answer on [Qt Forum](https://forum.qt.io/topic/162168/how-to-round-the-corners-of-a-frameless-qwidget-when-also-applying-acrylic-effects/10).

## Version History

Current version: **7.9.1**

## License

This project is released under the MIT License. See the LICENSE file for details.

