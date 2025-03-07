#pragma once

#include "Filter.h"

/**
 * @brief Applies Gaussian blur to video frames
 * 
 * This filter smooths the image using a Gaussian filter with configurable
 * kernel size and sigma values.
 */
class GaussianBlurFilter : public Filter {
public:
    /**
     * @brief Construct with default parameters
     */
    GaussianBlurFilter();
    
    /**
     * @brief Construct with specific blur parameters
     * 
     * @param kernelSize Size of the Gaussian kernel (must be odd)
     * @param sigmaX Gaussian kernel standard deviation in X direction
     * @param sigmaY Gaussian kernel standard deviation in Y direction
     */
    GaussianBlurFilter(int kernelSize, double sigmaX, double sigmaY);
    
    /**
     * @brief Apply the Gaussian blur to a frame
     * 
     * @param inputFrame The input frame to process
     * @param outputFrame The output frame after processing
     * @return true if processing was successful, false otherwise
     */
    bool apply(const cv::Mat& inputFrame, cv::Mat& outputFrame) override;
    
    /**
     * @brief Get the name of the filter
     * 
     * @return std::string The filter name
     */
    std::string getName() const override;
    
    /**
     * @brief Configure the filter with custom parameters
     * 
     * @param params A map of parameter name to value
     * @return true if configuration was successful, false otherwise
     */
    bool configure(const std::map<std::string, double>& params) override;

private:
    int kernelSize;     ///< Size of the Gaussian kernel
    double sigmaX;      ///< Sigma value for X direction
    double sigmaY;      ///< Sigma value for Y direction
};