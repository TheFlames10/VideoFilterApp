#include "GaussianBlurFilter.h"
#include <iostream>

GaussianBlurFilter::GaussianBlurFilter()
    : kernelSize(5), sigmaX(1.5), sigmaY(1.5) {
}

GaussianBlurFilter::GaussianBlurFilter(int kernelSize, double sigmaX, double sigmaY)
    : kernelSize(kernelSize), sigmaX(sigmaX), sigmaY(sigmaY) {
    // Ensure kernel size is odd
    if (kernelSize % 2 == 0) {
        this->kernelSize = kernelSize + 1;
        std::cout << "Warning: Kernel size must be odd. Adjusted to "
                  << this->kernelSize << std::endl;
    }
}

bool GaussianBlurFilter::apply(const cv::Mat& inputFrame, cv::Mat& outputFrame) {
    if (!isEnabled() || inputFrame.empty()) {
        outputFrame = inputFrame.clone();
        return false;
    }

    try {
        // Apply Gaussian blur
        cv::GaussianBlur(inputFrame, outputFrame,
                         cv::Size(kernelSize, kernelSize),
                         sigmaX, sigmaY);
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "Error in GaussianBlurFilter: " << e.what() << std::endl;
        outputFrame = inputFrame.clone();
        return false;
    }
}

std::string GaussianBlurFilter::getName() const {
    return "Gaussian Blur";
}

bool GaussianBlurFilter::configure(const std::map<std::string, double>& params) {
    bool changed = false;

    if (params.count("kernelSize")) {
        int newSize = static_cast<int>(params.at("kernelSize"));
        // Ensure kernel size is odd
        if (newSize % 2 == 0) newSize++;
        if (newSize > 0) {
            kernelSize = newSize;
            changed = true;
        }
    }

    if (params.count("sigmaX")) {
        double newSigmaX = params.at("sigmaX");
        if (newSigmaX > 0) {
            sigmaX = newSigmaX;
            changed = true;
        }
    }

    if (params.count("sigmaY")) {
        double newSigmaY = params.at("sigmaY");
        if (newSigmaY > 0) {
            sigmaY = newSigmaY;
            changed = true;
        }
    }

    return changed;
}