#pragma once

#include <string>
#include <vector>
#include <../cmake-build-debug/vcpkg_installed/x64-windows/include/opencv4/opencv2/opencv.hpp>

/**
 * @brief Cross-platform file dialog utility
 * 
 * Provides platform-specific implementations for open and save file dialogs.
 */
class FileDialog {
public:
    /**
     * @brief Open a file selection dialog
     * 
     * @param title Dialog window title
     * @param defaultPath Default directory to open
     * @param filterPatterns File extensions to filter (e.g., "*.mp4")
     * @return std::string Selected file path or empty string if canceled
     */
    static std::string openFile(const std::string& title = "Open File", 
                              const std::string& defaultPath = "", 
                              const std::vector<std::string>& filterPatterns = {"*.mp4"});
    
    /**
     * @brief Open a file save dialog
     * 
     * @param title Dialog window title
     * @param defaultPath Default directory and filename
     * @param filterPatterns File extensions to filter (e.g., "*.mp4")
     * @return std::string Selected save path or empty string if canceled
     */
    static std::string saveFile(const std::string& title = "Save File", 
                              const std::string& defaultPath = "output.mp4", 
                              const std::vector<std::string>& filterPatterns = {"*.mp4"});
};