#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include "filters/Filter.h"

/**
 * @brief Main video processing class that manages the processing pipeline
 * 
 * This class loads video files, processes frames through a series of
 * filters, and provides methods to display and save the processed output.
 * It uses multiple threads for improved performance.
 */
class VideoProcessor {
public:
    /**
     * @brief Default constructor
     */
    VideoProcessor();
    
    /**
     * @brief Destructor that ensures all threads are properly terminated
     */
    ~VideoProcessor();

    /**
     * @brief Open a video file for processing
     * 
     * @param filename Path to the video file
     * @return true if the file was opened successfully, false otherwise
     */
    bool openVideo(const std::string& filename);
    
    /**
     * @brief Start processing the video
     * 
     * @return true if processing started successfully, false otherwise
     */
    bool startProcessing();
    
    /**
     * @brief Pause processing
     */
    void pauseProcessing();
    
    /**
     * @brief Resume processing after a pause
     */
    void resumeProcessing();
    
    /**
     * @brief Stop processing and release resources
     */
    void stopProcessing();
    
    /**
     * @brief Add a filter to the processing pipeline
     * 
     * @param filter Pointer to a Filter object
     */
    void addFilter(std::shared_ptr<Filter> filter);
    
    /**
     * @brief Remove a filter from the pipeline by index
     * 
     * @param index Index of the filter to remove
     * @return true if the filter was removed, false if index was invalid
     */
    bool removeFilter(size_t index);
    
    /**
     * @brief Get the currently active filters
     * 
     * @return Vector of filter pointers
     */
    std::vector<std::shared_ptr<Filter>> getFilters() const;
    
    /**
     * @brief Set the output file for saving processed video
     * 
     * @param filename Path to the output file
     * @param fourcc FourCC codec code (e.g., cv::VideoWriter::fourcc('M','J','P','G'))
     * @param fps Frames per second for the output video
     * @return true if the output file was set successfully, false otherwise
     */
    bool setOutputFile(const std::string& filename, int fourcc, double fps);
    
    /**
     * @brief Get the latest processed frame
     * 
     * @return The most recent processed frame
     */
    cv::Mat getLatestFrame();
    
    /**
     * @brief Check if processing is currently active
     * 
     * @return true if processing is active, false otherwise
     */
    bool isProcessing() const;
    
    /**
     * @brief Get the current processing framerate
     * 
     * @return Frames per second
     */
    double getFrameRate() const;
    
    /**
     * @brief Get the current frame position
     * 
     * @return Current frame index
     */
    int getCurrentFramePosition() const;
    
    /**
     * @brief Get the total number of frames in the video
     * 
     * @return Total frame count
     */
    int getTotalFrames() const;
    
    /**
     * @brief Seek to a specific frame in the video
     * 
     * @param framePos Frame index to seek to
     * @return true if seeking was successful, false otherwise
     */
    bool seekToFrame(int framePos);

    /**
     * @brief Check if the video has reached its end
     *
     * @return true if the video has ended, false otherwise
     */
    bool hasVideoEnded() const;

    /**
     * @brief Restart the video from the beginning
     *
     * @return true if restart was successful, false otherwise
     */
    bool restartVideo();

private:
    // Video capture and properties
    cv::VideoCapture videoCapture;
    cv::VideoWriter videoWriter;
    std::string inputFilename;
    std::string outputFilename;
    int frameWidth;
    int frameHeight;
    int totalFrames;
    int currentFrame;
    double fps;
    bool videoEnded;
    
    // Processing state
    std::atomic<bool> processing;
    std::atomic<bool> paused;
    std::atomic<bool> stopRequested;
    
    // Filter pipeline
    std::vector<std::shared_ptr<Filter>> filters;
    mutable std::mutex filtersMutex;

    // Frame queue for thread communication
    const size_t MAX_QUEUE_SIZE = 10;
    std::queue<cv::Mat> frameQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;

    // Processing threads
    std::thread captureThread;
    std::thread processingThread;

    // Latest processed frame for display
    cv::Mat latestFrame;
    std::mutex frameMutex;

    // Performance monitoring
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;
    double currentFps;
    mutable std::mutex fpsMutex;
    
    // Thread functions
    void captureThreadFunc();
    void processingThreadFunc();

    
    // Apply all filters to a frame
    void applyFilters(const cv::Mat& input, cv::Mat& output);
};