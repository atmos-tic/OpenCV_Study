#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat img(512,512, CV_8UC3, Scalar(100, 100, 100));
    circle(img, Point(256, 256), 100, Scalar(200, 100, 100), 5);
    line(img, Point(0, 0), Point(512, 512), Scalar(0, 200, 0), 3, 4);
    rectangle(img, Point(100, 100), Point(300, 200), Scalar(0, 0, 200), 2, 4);
    imshow("image",img);
    waitKey(0);
    return 0;
}