#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat img = imread("heg.jpg");
     Mat dst;
     GaussianBlur(img, dst, Size(7,7), 10, 10);
     imshow("image",dst);
     waitKey(0);
    return 0;
}