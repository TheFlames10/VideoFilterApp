#pragma once

#include "Filter.h"

/**
 * @brief Applies edge detection to video frames
 * 
 * This filter detects edges in the image using the Canny edge detector.
 */
class EdgeDetectionFilter : public Filter {
public:
    /**
     * @brief Construct with default parameters
     */
    EdgeDetectionFilter();
    
    /**
     * @brief Construct with specific edge detection parameters
     * 
     * @param threshold1 First threshold for the hysteresis procedure
     * @param threshold2 Second threshold for the hysteresis procedure
     * @param apertureSize Aperture size for the Sobel operator
     */
    EdgeDetectionFilter(double threshold1, double threshold2, int apertureSize);
    
    /**
     * @brief Apply edge detection to a frame
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
    double threshold1;    ///< First threshold for hysteresis procedure
    double threshold2;    ///< Second threshold for hysteresis procedure
    int apertureSize;     ///< Aperture size for Sobel operator
};