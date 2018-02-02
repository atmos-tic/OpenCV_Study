#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat img = imread("heg.jpg");
    Mat dst;
    Point2f srcPoint[] =
    {
        Point(0,        0),
        Point(0,        img.cols),
        Point(img.rows, img.cols),
        Point(img.rows, 0),
    };
    Point2f dstPoint[] =
    {
        Point(100, 100),
        Point(0,   300),
        Point(400, 450),
        Point(500, 50),
    };

    Mat H = getPerspectiveTransform(srcPoint, dstPoint);
    warpPerspective( img, dst, H, Size(512, 512));
    imshow("image", dst);
    waitKey(0);
}