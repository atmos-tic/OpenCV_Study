#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

#define Nbin (9)
#define Np (4)
#define Nc  (4)
#define DSx (64)
#define DSy (64)
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
  cv::resize(roi, dst, cv::Size(DSx, DSy));
  dst.convertTo(dst, CV_32F, 1);
  cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
  cv::Sobel(dst, img_diff[0], CV_32F, 1, 0);
  cv::Sobel(dst, img_diff[1], CV_32F, 0, 1);
  cv::Mat mag, ang; 
  cv::cartToPolar(img_diff[0], img_diff[1], mag, ang, true); 
  mag.convertTo(mag, CV_8U, 1);
  cv::imshow("mag", mag);
  // 勾配方向を[0, 180)にする
  cv::add(ang, cv::Scalar(180.0), ang, ang < 0.0);
  cv::add(ang, cv::Scalar(-360.0), ang, ang >= 360.0);
  cv::add(ang, cv::Scalar(-180), ang, ang >= 180);
  cv::add(ang, cv::Scalar(-180.0), ang, ang >= 180.0);
  ang.convertTo(ang, CV_8U, 1);
  ang /= (180/Nbin);
  cv::add(ang, cv::Scalar(-1), ang, ang == Nbin);//五入防止
  cv::imshow("angle", ang);

  //積分画像作成
  std::vector<cv::Mat> bin, integral;
  for(int theta = 0; theta < Nbin; theta++){
    cv::Mat tmp1, tmp2;
    cv::bitwise_and(ang, theta, tmp1, ang==theta);
    cv::add(tmp1, 1-theta, tmp1, tmp1==theta);
    tmp1 = tmp1.mul(mag);
    cv::integral(tmp1,tmp2);

    bin.push_back(tmp1);
    integral.push_back(tmp2);
  }
  cv::Mat Mat_bin(DSx/Np, DSy/Np, CV_8UC(9)); 
  cv::Mat Mat_integral(DSx/Np, DSy/Np, CV_8UC(9));
  cv::merge(bin, Mat_bin);
  cv::merge(integral, Mat_integral);

  // cv::Mat hist;
  // int channels[] = {0};
  // float range[] = {0, 180};
  // const float* ranges[] = {range};
  // int histSize[] = {Nbin};
  // cv::calcHist(&ang,1,channels,cv::Mat(),hist,1,histSize,ranges,true,false);

  cv::Mat hog_hist[DSx/Np-Nc+1][DSy/Np-Nc+1];

  for(int Sbx = 0; Sbx < DSx/Np-Nc+1; Sbx++){
    for(int Sby = 0; Sby < DSy/Np-Nc+1; Sby++){
      hog_hist[Sbx][Sby] = cv::Mat(Nc, Nc, CV_8UC(9));//ヒストグラム初期化
      for(int Scx = Sbx; Scx < Sbx+Nc; Scx++){
        for(int Scy = Sby; Scy < Sby+Nc; Scy++){ 
          cv::Mat hist_roi(Mat_bin, cv::Rect(Sbx+(Scx-Sbx)*Np, Sby+(Scy-Sby)*Np, Np, Np));
          hog_hist[Sbx][Sby].at<cv::Mat>(Scy, Scx) = cv::sum(hist_roi);//各セルのヒストグラム作成

        }
      }
    }
  }

  // cv::Mat hog_descriptor;//(Data_Size-(Nc*Np)+1, Data_Size-(Nc*Np)+1);
  // cv::Mat cell_hist[Nbin];
  // for(int x_block_start = 0; x_block_start < Data_Size-(Nc*Np)+1; x_block_start++){
  //   for(int y_block_start = 0; y_block_start < Data_Size-(Nc*Np)+1; y_block_start++){
  //     /*ブロック毎の処理*/
  //     cv::Scalar hist_sum = 0;
  //     cv::Scalar hog_hist[Nbin];
  //     for(int x_cell_start = x_block_start; x_cell_start < x_block_start+Nc; x_cell_start++){
  //       for(int y_cell_start = y_block_start; y_cell_start < y_block_start+Nc; y_cell_start++){ 
  //         /*セル毎の処理*/
  //         //ROIでセルの切り出し
  //         cv::Mat mag_roi(mag, cv::Rect(x_block_start+(x_cell_start-x_block_start)*Np, y_block_start+(y_cell_start-y_block_start)*Np, Np, Np));
  //         cv::Mat angle_roi(angle, cv::Rect(x_block_start+(x_cell_start-x_block_start)*Np, y_block_start+(y_cell_start-y_block_start)*Np, Np, Np));
  //         //ヒストグラムの作成とその和
  //         for(int theta = 0; theta < Nbin; theta++){
  //           cv::Mat tmp;
  //           cv::bitwise_and(angle_roi, theta, tmp, angle_roi==theta);
  //           cv::add(tmp, 1-theta, tmp, tmp==theta);
  //           hog_hist[theta] = cv::sum(tmp.mul(mag_roi));
  //           hist_sum += hog_hist[theta];
  //         }
  //       }
  //     }
  //     cv::Mat mag_block_roi(mag, cv::Rect(x_block_start, y_block_start, Nc*Np, Nc*Np));
  //     hog_descriptor = mag_block_roi / std::sqrt(cv::norm(hist_sum)*cv::norm(hist_sum) + 1);
  //     std::cout<<std::sqrt(cv::norm(hist_sum)*cv::norm(hist_sum) + 1) <<std::endl;
  //     cv::imshow("hog_descriptor", hog_descriptor);
  //   }
  // }

  // cv::imshow("mag", mag);
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