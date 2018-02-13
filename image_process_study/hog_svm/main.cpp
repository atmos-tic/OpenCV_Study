#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

#define Nangle (9)
#define Npixel (4)
#define Ncell  (3)
#define Data_Size (128)
cv::Rect selection;
int select_object;
void on_mouse (int event, int x, int y, int flags, void *param);
int main(int argc, const char* argv[])
{
  // 画像データを格納するための変数を宣言する
  cv::Mat frame;
  cv::Mat dst, img_diff[2], dif[3], img, clone;

  frame = cv::imread("seacow2.jpg");
    if(!frame.data)
    return -1;
  cv::cvtColor(frame, img, cv::COLOR_RGB2GRAY);
  cv::namedWindow("in", cv::WINDOW_AUTOSIZE);
  cv::imshow("in", img); 
  cvSetMouseCallback("in", (CvMouseCallback)(&on_mouse), &img);
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
  dst = img.clone();
  //選択範囲の表示
 // cv::rectangle(img, selection, 0xFF, 5);
 // cv::imshow("in", img); 
  //選択範囲への処理
  cv::Mat roi(img, selection);
  cv::resize(roi, dst, cv::Size(Data_Size, Data_Size));
  dst.convertTo(dst, CV_32F, 1);
  cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
  cv::Sobel(dst, img_diff[0], CV_32F, 1, 0);
  cv::Sobel(dst, img_diff[1], CV_32F, 0, 1);
  cv::Mat mag, angle; 
  cv::cartToPolar(img_diff[0], img_diff[1], mag, angle, true); 
  mag.convertTo(mag, CV_8U, 1);
  cv::imshow("mag", mag);
  // 勾配方向を[0, 180)にする
  cv::add(angle, cv::Scalar(180.0), angle, angle < 0.0);
  cv::add(angle, cv::Scalar(-360.0), angle, angle >= 360.0);
  cv::add(angle, cv::Scalar(-180), angle, angle >= 180);
  cv::add(angle, cv::Scalar(-180.0), angle, angle >= 180.0);
  angle.convertTo(angle, CV_8U, 1);
  cv::imshow("angle_", angle);
  angle /= (180/Nangle);
  cv::add(angle, cv::Scalar(-1), angle, angle == Nangle);//五入防止
  cv::imshow("angle", angle);
  
  for(int x_block_start = 0; x_block_start < Data_Size/(Ncell*Npixel); x_block_start++){
    for(int y_block_start = 0; y_block_start < Data_Size/(Ncell*Npixel); y_block_start++){
      /*ブロック毎の処理*/
      cv::Scalar hist_sum;
      cv::Scalar hog_hist[Nangle];
      for(int x_cell_start = x_block_start; x_cell_start < x_block_start+Ncell; x_cell_start++){
        for(int y_cell_start = y_block_start; y_cell_start < y_block_start+Ncell; y_cell_start++){ 
          /*セル毎の処理*/
          //ROIでセルの切り出し
          cv::Mat mag_roi(mag, cv::Rect(x_cell_start*Npixel, y_cell_start*Npixel, Npixel, Npixel));
          cv::Mat angle_roi(angle, cv::Rect(x_cell_start*Npixel, y_cell_start*Npixel, Npixel, Npixel));
          //ヒストグラムの作成とその和
          for(int theta = 0; theta < Nangle; theta++){
            cv::bitwise_and(angle_roi, cv::Scalar(0x00), angle_roi, angle_roi!=theta);
            cv::add(angle_roi, cv::Scalar(1-theta), angle_roi, angle_roi!=theta);
            hog_hist[theta] = cv::sum(angle_roi.mul(mag_roi));
            hist_sum += hog_hist[theta]*hog_hist[theta];
          }
        }
      }
      cv::Mat mag_block_roi(mag, cv::Rect(x_block_start*Ncell, y_block_start*Ncell, Ncell*Npixel, Ncell*Npixel));
      cv::imshow("mag_block_roi", mag_block_roi);
    }
  }
 
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