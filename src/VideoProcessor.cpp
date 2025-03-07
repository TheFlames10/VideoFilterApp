#include "VideoProcessor.h"
#include <iostream>

VideoProcessor::VideoProcessor()
    : frameWidth(0), frameHeight(0), totalFrames(0), currentFrame(0), fps(0.0),
      processing(false), paused(false), stopRequested(false), videoEnded(false), currentFps(0.0) {
}

VideoProcessor::~VideoProcessor() {
    stopProcessing();
}

bool VideoProcessor::openVideo(const std::string& filename) {
    // Close any previously opened video
    if (videoCapture.isOpened()) {
        videoCapture.release();
    }
    
    // Try to open the video file
    if (!videoCapture.open(filename)) {
        std::cerr << "Error: Could not open video file: " << filename << std::endl;
        return false;
    }
    
    // Get video properties
    frameWidth = static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
    frameHeight = static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));
    totalFrames = static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_COUNT));
    fps = videoCapture.get(cv::CAP_PROP_FPS);
    currentFrame = 0;
    
    // Store the filename
    inputFilename = filename;
    
    std::cout << "Opened video: " << filename << std::endl;
    std::cout << "  Resolution: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "  Total frames: " << totalFrames << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
    
    return true;
}

bool VideoProcessor::startProcessing() {
    if (!videoCapture.isOpened()) {
        std::cerr << "Error: No video file opened." << std::endl;
        return false;
    }
    
    if (processing) {
        std::cout << "Already processing video." << std::endl;
        return true;
    }
    
    // Reset state
    processing = true;
    paused = false;
    stopRequested = false;
    
    // Clear any existing frames in the queue
    std::queue<cv::Mat> empty;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::swap(frameQueue, empty);
    }
    
    // Start threads
    captureThread = std::thread(&VideoProcessor::captureThreadFunc, this);
    processingThread = std::thread(&VideoProcessor::processingThreadFunc, this);
    
    return true;
}

void VideoProcessor::pauseProcessing() {
    paused = true;
}

void VideoProcessor::resumeProcessing() {
    paused = false;
    queueCondition.notify_all();
}

void VideoProcessor::stopProcessing() {
    if (processing) {
        // Signal threads to stop
        stopRequested = true;
        paused = false;
        processing = false;
        
        // Notify all waiting threads
        queueCondition.notify_all();
        
        // Wait for threads to finish
        if (captureThread.joinable()) {
            captureThread.join();
        }
        
        if (processingThread.joinable()) {
            processingThread.join();
        }
        
        // Close video writer if open
        if (videoWriter.isOpened()) {
            videoWriter.release();
        }
    }
}

void VideoProcessor::addFilter(std::shared_ptr<Filter> filter) {
    if (filter) {
        std::lock_guard<std::mutex> lock(filtersMutex);
        filters.push_back(filter);
    }
}

bool VideoProcessor::removeFilter(size_t index) {
    std::lock_guard<std::mutex> lock(filtersMutex);
    if (index < filters.size()) {
        filters.erase(filters.begin() + index);
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<Filter>> VideoProcessor::getFilters() const {
    std::lock_guard<std::mutex> lock(filtersMutex);
    return filters;
}

bool VideoProcessor::setOutputFile(const std::string& filename, int fourcc, double fps) {
    if (fps <= 0) {
        fps = this->fps;  // Use input video fps if not specified
    }
    
    // Create video writer
    bool success = videoWriter.open(filename, fourcc, fps, 
                                   cv::Size(frameWidth, frameHeight));
    
    if (success) {
        outputFilename = filename;
        std::cout << "Output file set: " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
    }
    
    return success;
}

cv::Mat VideoProcessor::getLatestFrame() {
    std::lock_guard<std::mutex> lock(frameMutex);
    return latestFrame.clone();
}

bool VideoProcessor::isProcessing() const {
    return processing && !stopRequested;
}

double VideoProcessor::getFrameRate() const {
    std::lock_guard<std::mutex> lock(fpsMutex);
    return currentFps;
}

int VideoProcessor::getCurrentFramePosition() const {
    return currentFrame;
}

int VideoProcessor::getTotalFrames() const {
    return totalFrames;
}

bool VideoProcessor::seekToFrame(int framePos) {
    if (framePos < 0 || framePos >= totalFrames) {
        return false;
    }
    
    // Pause processing while seeking
    bool wasPaused = paused;
    pauseProcessing();
    
    // Clear the frame queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::queue<cv::Mat> empty;
        std::swap(frameQueue, empty);
    }
    
    // Seek to the desired frame
    bool success = videoCapture.set(cv::CAP_PROP_POS_FRAMES, framePos);
    
    if (success) {
        currentFrame = framePos;
    }
    
    // Resume if it wasn't paused before
    if (!wasPaused) {
        resumeProcessing();
    }
    
    return success;
}

void VideoProcessor::captureThreadFunc() {
    cv::Mat frame;
    
    while (!stopRequested) {
        if (paused) {
            // Wait while paused
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !paused || stopRequested; });
            continue;
        }
        
        // Limit the queue size
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (frameQueue.size() >= MAX_QUEUE_SIZE) {
                queueCondition.wait(lock, [this] { 
                    return frameQueue.size() < MAX_QUEUE_SIZE || stopRequested; 
                });
            }
        }
        
        // Read the next frame
        if (!videoCapture.read(frame)) {
            // End of video or error
            std::cout << "End of video reached." << std::endl;
            videoEnded = true;
            queueCondition.notify_all();
            break;
        }
        
        // Update current frame position
        currentFrame = static_cast<int>(videoCapture.get(cv::CAP_PROP_POS_FRAMES));
        
        // Add frame to queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame.clone());
        }
        
        // Notify processing thread
        queueCondition.notify_one();
    }
}

void VideoProcessor::processingThreadFunc() {
    cv::Mat inputFrame, outputFrame;
    
    while (!stopRequested) {
        // Get a frame from the queue
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (frameQueue.empty()) {
                // If video has ended and queue is empty, we're done processing
                if (videoEnded && !paused) {
                    queueCondition.wait_for(lock, std::chrono::milliseconds(100), [this] {
                        return !frameQueue.empty() || stopRequested || !videoEnded;
                    });

                    // If still empty and video still ended, check again
                    if (frameQueue.empty() && videoEnded && !stopRequested) {
                        continue;  // Keep the thread alive but don't process
                    }
                } else {
                    // Normal wait for more frames
                    queueCondition.wait(lock, [this] {
                        return !frameQueue.empty() || stopRequested;
                    });
                }

                if (stopRequested) break;
                if (frameQueue.empty()) continue;
            }
            
            inputFrame = frameQueue.front();
            frameQueue.pop();
        }
        
        // Notify capture thread that queue has space
        queueCondition.notify_one();
        
        // Process the frame
        applyFilters(inputFrame, outputFrame);
        
        // Save the processed frame if writer is open
        if (videoWriter.isOpened()) {
            videoWriter.write(outputFrame);
        }
        
        // Update the latest frame for display
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            latestFrame = outputFrame.clone();
        }
        
        // Update FPS calculation
        auto now = std::chrono::high_resolution_clock::now();
        if (lastFrameTime.time_since_epoch().count() > 0) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastFrameTime).count();
            if (duration > 0) {
                double instantFps = 1000.0 / duration;
                
                // Smooth FPS calculation
                {
                    std::lock_guard<std::mutex> lock(fpsMutex);
                    currentFps = currentFps * 0.9 + instantFps * 0.1;
                }
            }
        }
        lastFrameTime = now;
    }
}

bool VideoProcessor::hasVideoEnded() const {
    return videoEnded;
}

bool VideoProcessor::restartVideo() {
    if (inputFilename.empty() || !videoCapture.isOpened()) {
        return false;
    }

    // Reset state
    videoEnded = false;

    // Seek to the beginning
    bool success = videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
    if (success) {
        currentFrame = 0;

        // Restart processing if it was previously ended
        if (!processing) {
            startProcessing();
        } else if (paused) {
            // Resume if paused
            resumeProcessing();
        }
    }

    return success;
}

void VideoProcessor::applyFilters(const cv::Mat& input, cv::Mat& output) {
    cv::Mat temp = input.clone();
    cv::Mat result;
    
    // Apply each filter in sequence
    std::lock_guard<std::mutex> lock(filtersMutex);
    for (const auto& filter : filters) {
        if (filter->isEnabled()) {
            filter->apply(temp, result);
            temp = result.clone();
        }
    }
    
    output = temp;
}