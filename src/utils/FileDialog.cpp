#include "FileDialog.h"
#include <iostream>

#ifdef _WIN32
// Windows implementation
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

std::string FileDialog::openFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    // Prepare filter string (format: "Description\0*.ext\0")
    std::string filterStr = "Video Files\0*.mp4;*.avi;*.mkv\0All Files\0*.*\0";
    
    char filename[MAX_PATH] = {0};
    if (!defaultPath.empty()) {
        strncpy(filename, defaultPath.c_str(), sizeof(filename) - 1);
    }
    
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filterStr.c_str();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    
    return "";
}

std::string FileDialog::saveFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    // Prepare filter string
    std::string filterStr = "Video Files\0*.mp4;*.avi;*.mkv\0All Files\0*.*\0";
    
    char filename[MAX_PATH] = {0};
    if (!defaultPath.empty()) {
        strncpy(filename, defaultPath.c_str(), sizeof(filename) - 1);
    }
    
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filterStr.c_str();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    if (GetSaveFileNameA(&ofn)) {
        std::string result(filename);
        // Ensure mp4 extension if not specified
        if (result.find(".") == std::string::npos) {
            result += ".mp4";
        }
        return result;
    }
    
    return "";
}

#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
// macOS/Linux implementation using zenity or native dialogs

#include <cstdlib>
#include <array>
#include <memory>
#include <stdexcept>

// Helper function to execute a command and get its output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // Remove trailing newline
    if (!result.empty() && result[result.length()-1] == '\n') {
        result.erase(result.length()-1);
    }
    return result;
}

std::string FileDialog::openFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    try {
        // Try with zenity (common on many Linux distros)
        std::string cmd = "zenity --file-selection --title=\"" + title + "\" --file-filter=\"Video files (mp4 avi mkv) | *.mp4 *.avi *.mkv\"";
        return exec(cmd.c_str());
    } catch (const std::exception& e) {
        std::cerr << "Error using file dialog: " << e.what() << std::endl;
        
        // Fallback to console input
        std::string path;
        std::cout << "Enter file path to open: ";
        std::getline(std::cin, path);
        return path;
    }
}

std::string FileDialog::saveFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    try {
        // Try with zenity
        std::string cmd = "zenity --file-selection --save --title=\"" + title + "\" --file-filter=\"Video files (mp4) | *.mp4\" --filename=\"" + defaultPath + "\"";
        std::string result = exec(cmd.c_str());
        
        // Ensure mp4 extension if not specified
        if (!result.empty() && result.find(".") == std::string::npos) {
            result += ".mp4";
        }
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error using file dialog: " << e.what() << std::endl;
        
        // Fallback to console input
        std::string path = defaultPath;
        std::cout << "Enter file path to save: ";
        std::getline(std::cin, path);
        return path;
    }
}

#else
// Fallback implementation for unsupported platforms
std::string FileDialog::openFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    std::string path;
    std::cout << title << " (Enter file path): ";
    std::getline(std::cin, path);
    return path;
}

std::string FileDialog::saveFile(const std::string& title, 
                              const std::string& defaultPath, 
                              const std::vector<std::string>& filterPatterns) {
    std::string path = defaultPath;
    std::cout << title << " (Enter file path, default: " << defaultPath << "): ";
    std::getline(std::cin, path);
    return path;
}
#endif