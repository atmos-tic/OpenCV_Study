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
  int camera_id = 1;
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

  for (;;)
  {
    // （4）カメラから1フレーム分の画像データを取得して、変数frameに格納する
    cap >> frame;

    // 画像データ取得に失敗したらループを抜ける
    if (frame.empty()) break;

    // 取得した画像データをウィンドウ表示する
    cv::imshow("image", frame);

    if (cv::waitKey(30) >= 0) break;
  }
  return 0;
}