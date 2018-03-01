#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

#define Nbin (9)
#define Np (16)
#define Nc  (2)
#define DSx (512)
#define DSy (512)
cv::Rect selection;
int select_object;
void on_mouse (int event, int x, int y, int flags, void *param);
int main(int argc, const char* argv[])
{
  // 画像データを格納するための変数を宣言する
  cv::Mat frame;
  cv::Mat imgdst, img_diff[2], dif[3], img, clone, dst;

// （1）DirectShowを使ってカメラキャプチャを行う。
  // また、カメラIDが0番のカメラについてキャプチャの準備を行う
  int camera_id = 0;
  //cv::VideoCapture cap(cv::CAP_DSHOW + camera_id);
   cv::VideoCapture cap(camera_id);

  // （2）カメラIDが0番のカメラが正しくオープンできているかをチェックする（正しくオープンできていなければエラー終了する）
  if (!cap.isOpened())
    return -1;

  // （3）カメラキャプチャのフレームレートを30.0に指定する
  cap.set(cv::CAP_PROP_FPS, 30.0);


  while(true) {
		cap >> img;

    //img = cv::imread("l_hires.jpg");
    //frame = cv::imread("img.jpg");
    
    if(!img.data)
      return -1;
    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
    //cv::namedWindow("in", cv::WINDOW_AUTOSIZE);
    // cv::imshow("in", img); 
    // cvSetMouseCallback("in", (CvMouseCallback)(&on_mouse), &img);
    // while(!select_object){
    //   if(cv::waitKey(27)>=0)//escをおされるまで　
    //   {
    //     cv::destroyAllWindows();
    //     break;
    //   }
    // }
    // while(select_object){
    //   if(cv::waitKey(27)>=0)//escをおされるまで　
    //   {
    //     cv::destroyAllWindows();
    //     break;
    //   }
    // }

    //選択範囲への処理
    //cv::Mat roi(img, selection);
    cv::Mat roi;
    cv::resize(img, roi, cv::Size(DSx, DSy));
    roi.convertTo(dst, CV_32F, 1);
    cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
    cv::Sobel(dst, img_diff[0], CV_64F, 1, 0);
    cv::Sobel(dst, img_diff[1], CV_64F, 0, 1);
    cv::Mat mag, ang; 
    cv::cartToPolar(img_diff[0], img_diff[1], mag, ang, true); 
    // 勾配方向を[0, 180)にする
    cv::add(ang, cv::Scalar(180.0), ang, ang < 0.0);
    cv::add(ang, cv::Scalar(-180), ang, ang >= 180);
    ang.convertTo(ang, CV_8U, 1);
    ang /= (180/Nbin);
    ang.convertTo(ang, CV_8U, 1);
    cv::add(ang, cv::Scalar(-1), ang, ang == 9);
    ang.convertTo(ang, CV_64FC1, 1);
    //cv::imshow("ang", ang);    

    //積分画像作成
    std::vector<cv::Mat> bin;
    cv::add(ang, 1, ang);
    for(int theta = 0; theta < Nbin; theta++){
      cv::Mat tmp;
      cv::bitwise_and(ang, theta+1, tmp, ang==theta+1);
      cv::add(tmp, -theta, tmp, tmp==theta+1);
      tmp = tmp.mul(mag);
      std::cout<<cv::sum(tmp)<<"|"<<cv::norm(tmp)<<std::endl;      
      bin.push_back(tmp);
    }
    cv::Mat Mat_bin(DSy/Np, DSx/Np, CV_64FC(Nbin)); 
    cv::merge(bin, Mat_bin);

    typedef cv::Vec<double, 9> Vec9d;

    cv::Mat hog_hist[DSy/Np-Nc+1][DSx/Np-Nc+1];
    for(int Sbx = 0; Sbx < DSx/Np-Nc+1; Sbx++){
      for(int Sby = 0; Sby < DSy/Np-Nc+1; Sby++){
        //各ブロック内の処理
        hog_hist[Sby][Sbx] = cv::Mat(Nc, Nc, CV_64FC(Nbin));//ヒストグラム初期化
        for(int Scx = 0; Scx < Nc; Scx++){
          for(int Scy = 0; Scy < Nc; Scy++){ 
            //各セル内の処理
            cv::Mat hist_roi = Mat_bin(cv::Rect(Sby*Np+Scy*Np, Sbx*Np+Scx*Np, Np, Np));//全体からセルのデータ抽出
            cv::Mat tmp;
            cv::integral(hist_roi,tmp);         
            for(int theta = 0; theta < Nbin; theta++){    
              hog_hist[Sby][Sbx].at<double>(Scy, Scx*Nbin + theta) = tmp.at<double>(Np, Np*Nbin + theta);//各セルのヒストグラム作成
            }    
          }
        }
        
        double sum;
        sum = cv::sqrt(norm(hog_hist[Sby][Sbx])*norm(hog_hist[Sby][Sbx]) + 1);
        hog_hist[Sby][Sbx] /= sum;
        cv::Mat tmp;
        cv::integral(hog_hist[Sby][Sbx],tmp);

        //角度ごとに線を描画
        cv::Point center(Sby*Np+(Nc*Np)/2, Sbx*Np+(Nc*Np)/2);
        for (int i = 0; i < Nbin; i++) {
          double theta = (i * 180 / Nbin + 90) * CV_PI / 180.0;
          cv::Point rd(Np*0.5*cos(theta), Np*0.5*sin(theta));
          cv::Point rp = center + rd;
          cv::Point lp = center - rd;
          cv::line(roi, rp, lp, cv::Scalar(255*tmp.at<double>(Nc, Nc*Nbin + i), 255*tmp.at<double>(Nc, Nc*Nbin + i), 255*tmp.at<double>(Nc, Nc*Nbin + i))); 
        }     
      }
    }
    cv::imshow("img", roi);
    int c = cv::waitKey(1);
		if( c == 27) break;
  }
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