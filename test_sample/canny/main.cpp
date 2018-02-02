#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat img = imread("heg.jpg");
    Mat dst, gray;
    cvtColor(img, gray, COLOR_RGB2GRAY);
    Canny(gray, dst, 40, 150);
    imshow("image",dst);
    waitKey(0);
}