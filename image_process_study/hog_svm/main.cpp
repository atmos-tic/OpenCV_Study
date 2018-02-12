#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

cv::Rect selection;
int select_object;
void on_mouse (int event, int x, int y, int flags, void *param);
#define USE_VIDEO_CAPTURE
int main(int argc, const char* argv[])
{
  // 画像データを格納するための変数を宣言する
  cv::Mat frame;
  cv::Mat dst, img_diff[2], dif[3], img, clone;

  frame = cv::imread("seacow.jpg");
    if(!frame.data)
    return -1;
  cv::namedWindow("in", cv::WINDOW_AUTOSIZE);
  cv::imshow("in", frame); 
  cvSetMouseCallback("in", (CvMouseCallback)(&on_mouse), &frame);
  img = frame.clone();
  while(!select_object){
    if(cv::waitKey(27)>=0)//escをおされるまで　
    {
      cv::destroyAllWindows();
      break;
    }
  }
  while(select_object){
    if(cv::waitKey(27)>=0)//escをおされるまで　
    {
      cv::destroyAllWindows();
      break;
    }
  }
  //選択範囲の表示
  cv::rectangle(frame, selection, CV_RGB(0x00, 0xff, 0xff));
  cv::imshow("in", frame); 
  //選択範囲への処理
  cv::Mat roi(img, selection);;
  cv::resize(roi, dst, cv::Size(128, 128));
  std::cout<<"ccc"<<std::endl;
  dst.convertTo(dst, CV_32F, 1/255.0);
  cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
  cv::Sobel(dst, img_diff[0], CV_32F, 1, 0);
  cv::Sobel(dst, img_diff[1], CV_32F, 0, 1);
  cv::Mat mag, angle; 
  cv::cartToPolar(img_diff[0], img_diff[1], mag, angle, 1); 
  cv::imshow("out", mag);
  cv::waitKey(0);

  return 0;
}

/* コールバック関数 */
void on_mouse (int event, int x, int y, int flags, void *param = NULL)
{
  // process a start and a finish selecting events (i.e. button-up and -down)
  switch(event) {
  case CV_EVENT_MOUSEMOVE:
    if (select_object){
      selection.width = x - selection.x;
      selection.height = y - selection.y;
    }
    break;
  case CV_EVENT_LBUTTONDOWN:
    selection = cv::Rect(x, y, 0, 0);
    select_object = true;
    break;
  case CV_EVENT_LBUTTONUP:
    select_object = false;
    if (selection.width < 0){
      selection.x += selection.width;
      selection.width *= -1;
    }
    if (selection.height < 0){
      selection.y += selection.height;
      selection.height *= -1;
    }
    break;
  }
  
}