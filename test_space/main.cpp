#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>
/*https://www.buildinsider.net/small/opencv/006*/

int main(int argc, const char* argv[])
{
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

  // 画像表示用のウィンドウを生成する
  cv::namedWindow("image", cv::WINDOW_AUTOSIZE);

  // 画像データを格納するための変数を宣言する
  cv::Mat frame;

  cv::Mat dst, gray, img1, img2;

  /*トラックバー用window作成*/
   cv::namedWindow("track", cv::WINDOW_AUTOSIZE);
   int thredshold_value = 80;
   cv::createTrackbar("threshold", "track", &thredshold_value, 255);
   //cv::imshow("track", dst);
  for (;;)
  {
    // （4）カメラから1フレーム分の画像データを取得して、変数frameに格納する
    cap >> frame;
    cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
    //cv::equalizeHist(gray, img1);
    threshold(gray, dst, thredshold_value, 200, 0);
    // cv::GaussianBlur(gray, img2, cv::Size(7,7), 10, 10);
    // cv::Canny(img2, dst, 40, 150);
    // 画像データ取得に失敗したらループを抜ける
    if (frame.empty()) break;

    // 取得した画像データをウィンドウ表示する
    cv::imshow("image", dst);

    if (cv::waitKey(30) >= 0) break;
  }
  return 0;
}