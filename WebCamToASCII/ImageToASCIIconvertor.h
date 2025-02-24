#pragma once
#pragma once
#include <Windows.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>

using namespace cv;
using namespace std;


const int nwidth = 400; // width // resolution
const float aspect = 11.0 / 24.0; // aspect pixel to ascii
const char* gradient = " .:!/r(lZ4H9W8$@"; //shadowing

vector<vector<char>> convertToASCII(Mat image, int scaledRows, int scaledCols, int scale = 1) {

	vector<vector<char>> ascii(scaledRows, vector<char>(scaledCols, ' '));

	if (scale == 1) {
		for (int i = 0, x = 0; x < scaledRows; i += scale, x++) {
			for (int j = 0, y = 0; y < scaledCols; j += scale, y++) {

				//if scale == 1
				Vec3b pixel = image.at<Vec3b>(i, j);
				int color = round((pixel[0] + pixel[1] + pixel[2]) / 3.0);
				ascii[x][y] = gradient[color / 16];
			}
		}

		return ascii;
	}

	//else
	for (int i = 0, x = 0; x < scaledRows; i += scale, x++) {
		for (int j = 0, y = 0; y < scaledCols; j += scale, y++) {

			//taking pixels
			//if scale > 1
			int colorSum = 0;
			for (int m = 0; m < scale; m++) {
				int color = 0;
				for (int n = 0; n < scale; n++) {
					if (i + m >= image.size().height - 1 || j + n >= image.size().width - 1) {
						color = 0;
					}
					else {
						Vec3b pixel = image.at<Vec3b>(i + m, j + n);
						color = round((pixel[0] + pixel[1] + pixel[2]) / 3.0);
					}

				}
				colorSum += color;
			}

			//if scale == 1
			/*Vec3b pixel = image.at<Vec3b>(i, j);
			int color = round((pixel[0] + pixel[1] + pixel[2]) / 3.0);*/

			//avg color if scale > 1
			colorSum /= (scale * scale);

			//cout << i << " " << j << " " << x << " " << y << " " << endl;

			//if scale > 1
			ascii[x][y] = gradient[colorSum / 16];
			//ascii[x][y] = gradient[color / 16];
		}
	}

	return ascii;
}

void printMatrix(vector<vector<char>> matrix) {

	vector<char> buffer(matrix.size() * matrix[0].size() + matrix.size()); //area + (\n)

	size_t bufferIndex = 0;
	for (const auto& row : matrix) {
		for (char c : row) {
			buffer[bufferIndex++] = c;
		}
		buffer[bufferIndex++] = '\n'; // Add newline at the end of each row
	}

	cout.write(buffer.data(), buffer.size());
}