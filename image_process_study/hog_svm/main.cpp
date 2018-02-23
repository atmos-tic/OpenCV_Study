#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

#define Nbin (9)
#define Np (4)
#define Nc  (4)
#define DSx (128)
#define DSy (128)
cv::Rect selection;
int select_object;
void on_mouse (int event, int x, int y, int flags, void *param);
int main(int argc, const char* argv[])
{
  // 画像データを格納するための変数を宣言する
  cv::Mat frame;
  cv::Mat imgdst, img_diff[2], dif[3], img, clone, dst;

  frame = cv::imread("seacow2.jpg");
  //frame = cv::imread("img.jpg");
  
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

  //選択範囲の表示
 // cv::rectangle(img, selection, 0xFF, 5);
 // cv::imshow("in", img); 

  //選択範囲への処理
  cv::Mat roi(img, selection);
  cv::resize(roi, roi, cv::Size(DSx, DSy));
  roi.convertTo(dst, CV_32F, 1);
  cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
  cv::Sobel(dst, img_diff[0], CV_32F, 1, 0);
  cv::Sobel(dst, img_diff[1], CV_32F, 0, 1);
  cv::Mat mag, ang; 
  cv::cartToPolar(img_diff[0], img_diff[1], mag, ang, true); 
  mag.convertTo(mag, CV_8U, 1);
  cv::imshow("mag", mag);
  // 勾配方向を[0, 180)にする
  cv::add(ang, cv::Scalar(180.0), ang, ang < 0.0);
  cv::add(ang, cv::Scalar(-180), ang, ang >= 180);
  cv::add(ang, cv::Scalar(-180), ang, ang == 180);
  ang.convertTo(ang, CV_8U, 1);
  ang /= (180/Nbin);
  cv::add(ang, cv::Scalar(-1), ang, ang == 9);
  cv::imshow("ang", ang);


  //積分画像作成
  std::vector<cv::Mat> bin;
  for(int theta = 0; theta < Nbin; theta++){
    cv::Mat tmp;
    cv::bitwise_and(ang, theta, tmp, ang==theta);
    cv::add(tmp, 1-theta, tmp, tmp==theta);
    tmp = tmp.mul(mag);
    bin.push_back(tmp);
  }
  cv::Mat Mat_bin(DSy/Np, DSx/Np, CV_8UC(Nbin)); 
  cv::merge(bin, Mat_bin);

  typedef cv::Matx<double, 9, 1> Matx9d;

  cv::Mat hog_hist[DSx/Np-Nc+1][DSy/Np-Nc+1];
  for(int Sbx = 0; Sbx < DSx/Np-Nc+1; Sbx++){
    for(int Sby = 0; Sby < DSy/Np-Nc+1; Sby++){
      //各ブロック内の処理
      hog_hist[Sbx][Sby] = cv::Mat::zeros(Nc, Nc, CV_8UC(Nbin));//ヒストグラム初期化
      for(int Scx = 0; Scx < Nc; Scx++){
        for(int Scy = 0; Scy < Nc; Scy++){ 
          //各セル内の処理
          cv::Mat hist_roi = Mat_bin(cv::Rect(Sby*Np+Scy*Np, Sbx*Np+Scx*Np, Np, Np));//全体からセルのデータ抽出
          cv::Mat tmp;
          cv::integral(hist_roi,tmp);
          //std::cout<<tmp.at<Matx9d>(Np, Np)<<std::endl;
          hog_hist[Sbx][Sby].at<Matx9d>(Scy, Scx) = tmp.at<Matx9d>(Np, Np);//各セルのヒストグラム作成
        }
      }
      
      double sum;
      sum = cv::sqrt(norm(hog_hist[Sbx][Sby])*norm(hog_hist[Sbx][Sby]) + 1);
      std::cout<<norm(hog_hist[Sbx][Sby])<<"x"<<Sbx<<"y"<<Sby<<std::endl;
      // //std::cout<<hog_hist[Sbx][Sby]<<std::endl;
      hog_hist[Sbx][Sby] /= sum;
      cv::Mat tmp(Nc, Nc, CV_8UC(9));
      cv::integral(hog_hist[Sbx][Sby],tmp);

      //角度ごとに線を描画
      cv::Point center(Sbx*Np+(Nc*Np)/2,Sby*Np+(Nc*Np)/2);
      for (int i = 0; i < Nbin; i++) {
        double theta = (i * 180 / Nbin + 90.0 ) * CV_PI / 180.0;
        cv::Point rd(Np*0.5*cos(theta), Np*0.5*sin(theta));
        cv::Point rp = center -   rd;
        cv::Point lp = center -  -rd;
        cv::line(roi, rp, lp, cv::Scalar(255*tmp.at<cv::Scalar>(Nc, Nc)[i], 255, 255)); 
      }     
    }
  }
  cv::imshow("img", roi);
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