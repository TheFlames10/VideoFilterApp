# Video Filter Application

A C++ application for demonstrating video processing capabilities using OpenCV. This project showcases skills in multimedia programming, multithreaded design, and software architecture that are relevant to video software development positions.

## Features

- **Video Loading**: Open and process video files in various formats
- **Real-time Filtering**: Apply video filters in real-time using a pipeline architecture
- **Multithreaded Design**: Utilizes separate threads for video capture and processing
- **Filter Framework**: Extensible design for adding new video filters
- **Performance Monitoring**: Track processing frame rate and performance metrics
- **Simple UI**: Interactive controls for manipulating video playback and filters

## Architecture

The application is designed using several object-oriented design patterns:

1. **Strategy Pattern**: Filter implementations follow the strategy pattern, allowing runtime selection of algorithms
2. **Producer-Consumer Pattern**: Multithreaded architecture with frame queue for efficient processing
3. **Facade Pattern**: VideoProcessor provides a simplified interface to the complex processing pipeline

## Implemented Filters

1. **Gaussian Blur**: Smooths the video using configurable kernel sizes
2. **Edge Detection**: Highlights edges in the video using the Canny algorithm

## Technical Details

- **Language**: C++17
- **Build System**: CMake
- **Dependencies**:
    - OpenCV 4.x for video processing
    - Standard C++ libraries (STL)
- **Multithreading**: Uses std::thread and synchronization primitives
- **Memory Management**: Modern C++ with smart pointers (std::shared_ptr, etc.)

## Future Enhancements

- GPU acceleration using CUDA or OpenCL
- Additional filter types (color manipulation, tracking, etc.)
- Support for camera input
- Advanced user interface with parameter controls
- Integration with video streaming protocols (RTSP, HLS)

## Relevant Skills Demonstrated

- C++ development for multimedia applications
- Video processing fundamentals
- Understanding of multithreaded programming
- Software architecture and design patterns
- Error handling and robust application design
- Performance optimization techniques