#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat dst, gray;
    Mat img = imread("heg.jpg");
    cvtColor(img, gray, COLOR_RGB2GRAY);
    Sobel(gray, dst, -1, 1, 0);
    imshow("image",dst);
    waitKey(0);
}