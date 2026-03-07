#include "ImageToASCIIconvertor.h"

//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <Windows.h>
#include <opencv2/highgui.hpp>
#include <chrono> 

constexpr int FPS = 60;

int main()
{
    cv::Mat image;
    cv::VideoCapture vid(0);

    if (!vid.isOpened()) return -1;

    // Compute output dimensions once up front
    int camWidth  = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_WIDTH));
    int camHeight = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_HEIGHT));

    float imasp = static_cast<float>(camWidth) / NWIDTH;
    //int nheight = static_cast<int>(std::round(camHeight * ASPECT / imasp));

   
    int scaledCols = NWIDTH / SCALE;
    //int scaledRows = nheight / SCALE;

    //// Reuse the same buffer each frame to avoid repeated allocation
    //std::vector<char> asciiBuffer;
    //asciiBuffer.reserve(scaledRows * scaledCols);

    //while (vid.read(image))
    //{
    //    cv::imshow("Webcam", image);

    //    if (cv::waitKey(1000 / FPS) >= 0)
    //        break;

    //    cv::resize(image, image, { NWIDTH, nheight }, 0, 0, cv::INTER_NEAREST);

    //    convertToASCII(image, scaledRows, scaledCols, SCALE, asciiBuffer);

    //    std::cout << "\033[H";
    //    printASCII(asciiBuffer, scaledRows, scaledCols);
    //}

    //return 0;

    // Measure how large each character renders so we can size the canvas exactly
    CharMetrics metrics = measureChar();

    int scaledRows = static_cast<int>(std::round(
        static_cast<float>(camHeight) / camWidth * scaledCols
        * metrics.width / metrics.height   // correct for cell aspect ratio (width != height)
    ));
    int nheight = scaledRows * SCALE;

    int canvasW = scaledCols * metrics.width;
    int canvasH = scaledRows * metrics.height;

    // Preallocate both buffers once — reused every frame
    std::vector<char> asciiBuffer;
    asciiBuffer.reserve(scaledRows * scaledCols);

    cv::Mat canvas(canvasH, canvasW, CV_8UC3);

    // imshow will auto-create the window on first call — namedWindow not needed

    while (vid.read(image))
    {
        auto frameStart = std::chrono::steady_clock::now();

        cv::resize(image, image, { NWIDTH, nheight }, 0, 0, cv::INTER_NEAREST);

        convertToASCII(image, scaledRows, scaledCols, SCALE, asciiBuffer);
        renderASCII(asciiBuffer, scaledRows, scaledCols, metrics, canvas, "ASCII");

        // Wait only the time remaining in the frame budget
        // If conversion + render already took longer than 1/FPS, just pump events
        auto elapsed = std::chrono::steady_clock::now() - frameStart;
        int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
        int remainingMs = (1000 / FPS) - elapsedMs;

        if (cv::waitKey(remainingMs > 0 ? remainingMs : 1) >= 0)
            break;
    }

    return 0;
}

