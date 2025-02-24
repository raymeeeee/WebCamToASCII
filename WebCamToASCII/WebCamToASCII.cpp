#include "ImageToASCIIconvertor.h"

const int fps = 60;

int main()
{
    Mat image; 

    VideoCapture vid(0);
	//vid >> image;
	
    if (!vid.isOpened()) return -1;


	Sleep(1000); // TIME TO RESCALE CONSOLE TO MAXIMUM // IF NOT DONE THE IMAGE WILL BE TEARED

    while (vid.read(image)) {
        //vid >> frame;
        imshow("Webcam", image);

		if (waitKey(1000 / fps) >= 0) {
			break;
		}

		//rescaling
		int rows = vid.get(cv::CAP_PROP_FRAME_WIDTH);
		int cols = vid.get(cv::CAP_PROP_FRAME_HEIGHT);

		//cout << rows << " " << cols << endl;

		float imasp = (float)rows / nwidth;
		int nheight = round(cols * aspect / imasp);
		resize(image, image, { nwidth, nheight }, 0, 0, INTER_NEAREST);

		Size sz = image.size();

		rows = sz.height;
		cols = sz.width;

		//cout << rows << " " << cols << endl;

		int scale = 1; //SCALExSCALE pixel square // reduction size

		int scaledRows = rows / scale;
		int scaledCols = cols / scale;

		//cout << scaledRows << " " << scaledCols << endl;

		//show image
		
		cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left
		printMatrix(convertToASCII(image, scaledRows, scaledCols, scale));

    }



    return 0;
}
