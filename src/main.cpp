#include <iostream>
#include <memory>
#include "VideoProcessor.h"
#include "ui/UserInterface.h"

/**
 * @brief Entry point for the Video Filter Application
 *
 * This application demonstrates video processing capabilities using OpenCV
 * and C++ multithreading. It allows loading video files, applying various
 * filters, and saving the processed output.
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return int Exit code
 */
int main(int argc, char* argv[]) {
    std::cout << "Video Filter Application" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "A demonstration of video processing using C++ and OpenCV." << std::endl;
    std::cout << std::endl;

    try {
        // Create the video processor
        auto processor = std::make_shared<VideoProcessor>();

        // Create the user interface
        UserInterface ui(processor);

        // Process command line arguments
        if (argc > 1) {
            std::string filename = argv[1];
            std::cout << "Opening video from command line: " << filename << std::endl;

            if (processor->openVideo(filename)) {
                processor->startProcessing();
            }
        }

        // Start the UI event loop
        return ui.run();
    }
    catch (const cv::Exception& e) {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception!" << std::endl;
        return 1;
    }
}