#include "UserInterface.h"
#include "../filters/GaussianBlurFilter.h"
#include "../filters/EdgeDetectionFilter.h"
#include <iostream>
#include <sstream>

// Constants for UI
const int DISPLAY_WIDTH = 1280;
const int DISPLAY_HEIGHT = 720;

UserInterface::UserInterface(std::shared_ptr<VideoProcessor> processor)
    : processor(processor), windowName("Video Filter App"), running(false) {
    // Initialize the UI
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    // Initialize available filters
    initializeFilters();
}

int UserInterface::run() {
    running = true;
    
    // Create a blank initial frame
    cv::Mat blankFrame(DISPLAY_HEIGHT, DISPLAY_WIDTH, CV_8UC3, cv::Scalar(40, 40, 40));
    putText(blankFrame, "Open a video file to start (press 'O')", 
            cv::Point(DISPLAY_WIDTH/2 - 200, DISPLAY_HEIGHT/2),
            cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    
    drawControls(blankFrame);
    cv::imshow(windowName, blankFrame);
    
    // Main event loop
    while (running) {
        updateDisplay();
        
        // Handle keyboard input (30ms wait for key)
        int key = cv::waitKey(30);
        if (key > 0) {
            handleKeyPress(key);
        }
    }
    
    cv::destroyAllWindows();
    return 0;
}

void UserInterface::onOpenFile() {
    // In a real application, we would show a file dialog
    // For simplicity, we'll use a hardcoded path
    std::string filename = "sample_video.mp4";
    std::cout << "Enter video path: ";
    std::getline(std::cin, filename);
    
    if (processor->openVideo(filename)) {
        processor->startProcessing();
    }
}

void UserInterface::onTogglePlayPause() {
    if (processor->isProcessing()) {
        if (processor->getCurrentFramePosition() == 0) {
            // Start processing if not started
            processor->startProcessing();
        } else if (processor->getFrameRate() == 0) {
            // Resume if paused
            processor->resumeProcessing();
        } else {
            // Pause if playing
            processor->pauseProcessing();
        }
    }
}

void UserInterface::onAddFilter(int filterIndex) {
    if (filterIndex >= 0 && filterIndex < availableFilters.size()) {
        processor->addFilter(availableFilters[filterIndex]);
        std::cout << "Added filter: " << availableFilters[filterIndex]->getName() << std::endl;
    }
}

void UserInterface::onSaveVideo() {
    // In a real application, we would show a save dialog
    // For simplicity, we'll use a hardcoded path
    std::string filename = "output_video.mp4";
    std::cout << "Enter output path: ";
    std::getline(std::cin, filename);
    
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    if (processor->setOutputFile(filename, fourcc, 30.0)) {
        std::cout << "Output video will be saved to: " << filename << std::endl;
    }
}

void UserInterface::handleKeyPress(int key) {
    switch (key) {
        case 27:  // ESC key
            running = false;
            break;
        case 'o':
        case 'O':
            onOpenFile();
            break;
        case ' ':  // Space bar
            onTogglePlayPause();
            break;
        case 's':
        case 'S':
            onSaveVideo();
            break;
        case '1':
            onAddFilter(0);  // Gaussian blur
            break;
        case '2':
            onAddFilter(1);  // Edge detection
            break;
        case 'q':
        case 'Q':
            running = false;
        case 'r':
        case 'R':
            if (processor->hasVideoEnded()) {
                processor->restartVideo();
                std::cout << "Video restarted" << std::endl;
            }
        break;
    }
}

void UserInterface::updateDisplay() {
    if (processor->isProcessing()) {
        cv::Mat frame = processor->getLatestFrame();
        
        if (!frame.empty()) {
            // Resize for display if needed
            if (frame.cols != DISPLAY_WIDTH || frame.rows != DISPLAY_HEIGHT) {
                cv::resize(frame, frame, cv::Size(DISPLAY_WIDTH, DISPLAY_HEIGHT));
            }
            
            // Add UI elements
            drawControls(frame);
            drawPerformanceInfo(frame);

            // Check for video ended state
            if (processor->hasVideoEnded()) {
                // Add a "Video Ended" message
                cv::Mat overlay;
                frame.copyTo(overlay);
                cv::rectangle(overlay, cv::Rect(frame.cols/2 - 150, frame.rows/2 - 40, 300, 80),
                           cv::Scalar(0, 0, 0), -1);
                cv::addWeighted(overlay, 0.7, frame, 0.3, 0, frame);

                cv::putText(frame, "Video Ended",
                         cv::Point(frame.cols/2 - 100, frame.rows/2),
                         cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);

                cv::putText(frame, "Press 'R' to restart",
                         cv::Point(frame.cols/2 - 130, frame.rows/2 + 30),
                         cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 1);
            }
            
            // Display the frame
            cv::imshow(windowName, frame);
        }
    }
}

void UserInterface::drawControls(cv::Mat& frame) {
    int y = 30;
    int lineHeight = 30;
    cv::Scalar textColor(255, 255, 255);
    cv::Scalar bgColor(0, 0, 0, 0.7);
    
    // Create a semi-transparent overlay for controls
    cv::Mat overlay;
    frame.copyTo(overlay);
    cv::rectangle(overlay, cv::Rect(10, 10, 350, 200), cv::Scalar(0, 0, 0), -1);
    cv::addWeighted(overlay, 0.5, frame, 0.5, 0, frame);
    
    // Add control instructions
    cv::putText(frame, "Controls:", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "O - Open video file", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "Space - Play/Pause", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "S - Save output video", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "1 - Add Gaussian Blur filter", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "2 - Add Edge Detection filter", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    cv::putText(frame, "ESC/Q - Quit", cv::Point(20, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
}

void UserInterface::drawPerformanceInfo(cv::Mat& frame) {
    int y = 30;
    int lineHeight = 30;
    cv::Scalar textColor(255, 255, 255);
    
    // Create a semi-transparent overlay for performance info
    cv::Mat overlay;
    frame.copyTo(overlay);
    cv::rectangle(overlay, cv::Rect(frame.cols - 350, 10, 340, 120), cv::Scalar(0, 0, 0), -1);
    cv::addWeighted(overlay, 0.5, frame, 0.5, 0, frame);
    
    // Add performance information
    cv::putText(frame, "Performance:", cv::Point(frame.cols - 340, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1) << processor->getFrameRate();
    cv::putText(frame, ss.str(), cv::Point(frame.cols - 340, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    ss.str("");
    ss << "Frame: " << processor->getCurrentFramePosition() 
       << " / " << processor->getTotalFrames();
    cv::putText(frame, ss.str(), cv::Point(frame.cols - 340, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
    y += lineHeight;
    
    ss.str("");
    ss << "Active Filters: " << processor->getFilters().size();
    cv::putText(frame, ss.str(), cv::Point(frame.cols - 340, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, textColor, 1);
}

void UserInterface::initializeFilters() {
    // Initialize available filters
    availableFilters.push_back(std::make_shared<GaussianBlurFilter>());
    availableFilters.push_back(std::make_shared<EdgeDetectionFilter>());
}