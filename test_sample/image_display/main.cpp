#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(void)
{
  int i;
  Mat img = imread("image.jpg");
  imshow("image",img);
  waitKey(0);
  return 0;
}