#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat img = imread("image.jpg");
    Mat dst(img, Rect(100, 100, 600, 600));;
    imshow("result", dst);
    waitKey(0);
    return 0;
}