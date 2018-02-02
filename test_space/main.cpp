#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
  Mat img = imread("heg.jpg");
  Mat dst;
  cvtColor(img, dst, COLOR_RGB2GRAY);
  imshow("image",dst);
  waitKey(0);
  return 0;
}