#include "ImageToASCIIconvertor.h"

//#include <Windows.h>

constexpr int FPS = 60;

int main()
{
    cv::Mat image;
    cv::VideoCapture vid(0); // first web-camera output

    if (!vid.isOpened()) return -1;

    // Compute output dimensions once up front
    int camWidth  = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_WIDTH));
    int camHeight = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_HEIGHT));

    float imasp = static_cast<float>(camWidth) / NWIDTH;
    int nheight = static_cast<int>(std::round(camHeight * ASPECT / imasp));

   
    int scaledCols = NWIDTH / SCALE;
    int scaledRows = nheight / SCALE;

    //// Reuse the same buffer each frame to avoid repeated allocation
    std::vector<char> asciiBuffer;
    asciiBuffer.reserve(scaledRows * scaledCols);

    while (vid.read(image))
    {
        cv::imshow("Webcam", image);

        if (cv::waitKey(1000 / FPS) >= 0)
            break;

        cv::resize(image, image, { NWIDTH, nheight }, 0, 0, cv::INTER_NEAREST);

        convertToASCII(image, scaledRows, scaledCols, SCALE, asciiBuffer);

        std::cout << "\033[H";
        printASCII(asciiBuffer, scaledRows, scaledCols);
    }
    return 0;
}

