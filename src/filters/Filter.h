#pragma once

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief Abstract base class for all video filters
 * 
 * This class defines the interface that all filters must implement.
 * It follows the Strategy design pattern, allowing for runtime
 * selection of different filtering algorithms.
 */
class Filter {
public:
    /**
     * @brief Default constructor
     */
    Filter() = default;
    
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~Filter() = default;
    
    /**
     * @brief Apply the filter to a frame
     * 
     * @param inputFrame The input frame to process
     * @param outputFrame The output frame after processing
     * @return true if processing was successful, false otherwise
     */
    virtual bool apply(const cv::Mat& inputFrame, cv::Mat& outputFrame) = 0;
    
    /**
     * @brief Get the name of the filter
     * 
     * @return std::string The filter name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Configure the filter with custom parameters
     * 
     * @param params A map of parameter name to value
     * @return true if configuration was successful, false otherwise
     */
    virtual bool configure(const std::map<std::string, double>& params) {
        return false; // Default implementation does nothing
    }
    
    /**
     * @brief Check if the filter is enabled
     * 
     * @return true if the filter is enabled, false otherwise
     */
    bool isEnabled() const {
        return enabled;
    }
    
    /**
     * @brief Enable or disable the filter
     * 
     * @param state True to enable, false to disable
     */
    void setEnabled(bool state) {
        enabled = state;
    }

protected:
    bool enabled = true;
};