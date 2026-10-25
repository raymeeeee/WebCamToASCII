#pragma once

//#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>

#include <opencv2/opencv.hpp>



//using namespace cv;
//using namespace std;

constexpr int NWIDTH = 1000; // width // resolution
constexpr float ASPECT = 11.0f / 24.0f; // aspect pixel to ascii
constexpr const char* GRADIENT = " .:!/r(lZ4H9W8$@"; //shadowing
constexpr int GRADIENT_LEN = 16;
constexpr int SCALE = 10;

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

void printASCII(const std::vector<char>& buffer, int rows, int cols)
{
    // +rows for the newlines
    std::vector<char> printBuffer(rows * cols + rows);
    size_t idx = 0;

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
            printBuffer[idx++] = buffer[r * cols + c];
        printBuffer[idx++] = '\n';
    }

    std::cout.write(printBuffer.data(), idx);
}
