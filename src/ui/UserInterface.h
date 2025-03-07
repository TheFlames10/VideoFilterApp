#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>
#include <functional>
#include "../VideoProcessor.h"

/**
 * @brief Simple OpenCV-based user interface for the video processor
 * 
 * This class handles user interaction and displays video frames.
 */
class UserInterface {
public:
    /**
     * @brief Constructor
     * 
     * @param processor Shared pointer to a VideoProcessor object
     */
    UserInterface(std::shared_ptr<VideoProcessor> processor);
    
    /**
     * @brief Start the UI event loop
     * 
     * @return Exit code (0 for normal exit)
     */
    int run();

private:
    // Video processor
    std::shared_ptr<VideoProcessor> processor;
    
    // UI state
    std::string windowName;
    bool running;
    
    // Available filters
    std::vector<std::shared_ptr<Filter>> availableFilters;
    
    // UI callbacks
    void onOpenFile();
    void onTogglePlayPause();
    void onAddFilter(int filterIndex);
    void onSaveVideo();
    
    // Keyboard event handler
    void handleKeyPress(int key);
    
    // Display functions
    void updateDisplay();
    void drawControls(cv::Mat& frame);
    void drawPerformanceInfo(cv::Mat& frame);
    
    // Initialize available filters
    void initializeFilters();
};