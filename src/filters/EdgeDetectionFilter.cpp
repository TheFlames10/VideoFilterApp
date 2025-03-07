#include "EdgeDetectionFilter.h"
#include <iostream>

EdgeDetectionFilter::EdgeDetectionFilter() 
    : threshold1(100.0), threshold2(200.0), apertureSize(3) {
}

EdgeDetectionFilter::EdgeDetectionFilter(double threshold1, double threshold2, int apertureSize)
    : threshold1(threshold1), threshold2(threshold2), apertureSize(apertureSize) {
    // Ensure aperture size is 3, 5, or 7
    if (apertureSize != 3 && apertureSize != 5 && apertureSize != 7) {
        this->apertureSize = 3;
        std::cout << "Warning: Aperture size must be 3, 5, or 7. Reset to 3." << std::endl;
    }
}

bool EdgeDetectionFilter::apply(const cv::Mat& inputFrame, cv::Mat& outputFrame) {
    if (!isEnabled() || inputFrame.empty()) {
        outputFrame = inputFrame.clone();
        return false;
    }
    
    try {
        // Convert to grayscale if needed
        cv::Mat grayFrame;
        if (inputFrame.channels() == 3) {
            cv::cvtColor(inputFrame, grayFrame, cv::COLOR_BGR2GRAY);
        } else {
            grayFrame = inputFrame.clone();
        }
        
        // Apply Canny edge detector
        cv::Canny(grayFrame, outputFrame, threshold1, threshold2, apertureSize);
        
        // Convert back to 3-channel if input was 3-channel
        if (inputFrame.channels() == 3) {
            cv::cvtColor(outputFrame, outputFrame, cv::COLOR_GRAY2BGR);
        }
        
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "Error in EdgeDetectionFilter: " << e.what() << std::endl;
        outputFrame = inputFrame.clone();
        return false;
    }
}

std::string EdgeDetectionFilter::getName() const {
    return "Edge Detection";
}

bool EdgeDetectionFilter::configure(const std::map<std::string, double>& params) {
    bool changed = false;
    
    if (params.count("threshold1")) {
        double newThreshold1 = params.at("threshold1");
        if (newThreshold1 >= 0) {
            threshold1 = newThreshold1;
            changed = true;
        }
    }
    
    if (params.count("threshold2")) {
        double newThreshold2 = params.at("threshold2");
        if (newThreshold2 >= 0) {
            threshold2 = newThreshold2;
            changed = true;
        }
    }
    
    if (params.count("apertureSize")) {
        int newApertureSize = static_cast<int>(params.at("apertureSize"));
        if (newApertureSize == 3 || newApertureSize == 5 || newApertureSize == 7) {
            apertureSize = newApertureSize;
            changed = true;
        }
    }
    
    return changed;
}