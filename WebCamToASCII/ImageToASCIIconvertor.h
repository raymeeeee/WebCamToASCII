#pragma once

//#include <Windows.h>

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


//using namespace cv;
//using namespace std;

constexpr int NWIDTH = 1000; // width // resolution
constexpr float ASPECT = 11.0f / 24.0f; // aspect pixel to ascii
constexpr const char* GRADIENT = " .:!/r(lZ4H9W8$@"; //shadowing
constexpr int GRADIENT_LEN = 16;
constexpr int SCALE = 3;

void convertToASCII(const cv::Mat& image, int scaledRows, int scaledCols, int scale,
    std::vector<char>& outBuffer)
{
    outBuffer.assign(scaledRows * scaledCols, ' ');

    for (int x = 0; x < scaledRows; ++x)
    {
        for (int y = 0; y < scaledCols; ++y)
        {
            int i = x * scale;
            int j = y * scale;

            int colorSum = 0;
            int sampleCount = 0;

            for (int m = 0; m < scale; ++m)
            {
                for (int n = 0; n < scale; ++n)
                {
                    int pi = i + m;
                    int pj = j + n;
                    if (pi >= image.rows || pj >= image.cols)
                        continue;

                    const cv::Vec3b& pixel = image.at<cv::Vec3b>(pi, pj);
                    colorSum += (pixel[0] + pixel[1] + pixel[2]) / 3;
                    ++sampleCount;
                }
            }

            int avgColor = (sampleCount > 0) ? (colorSum / sampleCount) : 0; // grayscale

            // Map 0-255 brightness to gradient index
            int gradientIndex = (avgColor * (GRADIENT_LEN - 1)) / 255;

            outBuffer[x * scaledCols + y] = GRADIENT[gradientIndex];
        }
    }
}

//void printASCII(const std::vector<char>& buffer, int rows, int cols)
//{
//    // +rows for the newlines
//    std::vector<char> printBuffer(rows * cols + rows);
//    size_t idx = 0;
//
//    for (int r = 0; r < rows; ++r)
//    {
//        for (int c = 0; c < cols; ++c)
//            printBuffer[idx++] = buffer[r * cols + c];
//        printBuffer[idx++] = '\n';
//    }
//
//    std::cout.write(printBuffer.data(), idx);
//}

constexpr float FONT_SCALE = 0.4f;
static const int FONT_FACE = cv::FONT_HERSHEY_PLAIN;

struct CharMetrics
{
    int width;
    int height;
    int baseline;
};
inline CharMetrics measureChar()
{
    int baseline = 0;
    // Measure the widest character to use as the fixed cell width
    cv::Size sz = cv::getTextSize("W", FONT_FACE, FONT_SCALE, 1, &baseline);
    int cellW = sz.width + 2;  // +2 horizontal gap between characters
    int cellH = sz.height + 3;  // +3 vertical gap between rows
    return { cellW, cellH, baseline };
}

void renderASCII(const std::vector<char>& buffer,
    int rows, int cols,
    const CharMetrics& metrics,
    cv::Mat& canvas,
    const std::string& windowName = "ASCII")
{
    // Wipe canvas black — fast memset internally
    canvas.setTo(cv::Scalar(0, 0, 0));

    std::string rowStr(cols, ' ');
    char str[2] = { 0, 0 }; // reusable single-char string
    for (int r = 0; r < rows; ++r)
    {
        // Fixed y per row — same for every character in the row
        int y = (r + 1) * metrics.height - metrics.baseline;

        for (int c = 0; c < cols; ++c)
        {
            str[0] = buffer[r * cols + c];

            // Fixed x per column — every character occupies exactly metrics.width pixels
            int x = c * metrics.width;

            cv::putText(canvas, str,
                { x, y },
                FONT_FACE, FONT_SCALE,
                cv::Scalar(255, 255, 255),
                1, cv::LINE_AA);
        }
    }

    // Upscale to display width, preserving canvas aspect ratio exactly
    // so INTER_NEAREST distributes rows evenly (no stripe artifacts)
    constexpr int displayW = 900;
    int displayH = static_cast<int>(static_cast<float>(displayW) * canvas.rows / canvas.cols);
    cv::Mat display;
    cv::resize(canvas, display, { displayW, displayH }, 0, 0, cv::INTER_NEAREST);
    cv::imshow(windowName, display);
}