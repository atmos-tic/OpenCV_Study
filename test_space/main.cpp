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

  cv::Mat dst, pre_img[2], dif[3], gray, sub_img, img;
  int set_flag = 1;
  /*トラックバー用window作成*/
   cv::namedWindow("track", cv::WINDOW_AUTOSIZE);
   int thredshold_value = 80;
   cv::createTrackbar("threshold", "track", &thredshold_value, 255);
  
  for (;;)
  {
   if(set_flag){
      cap>>frame;
      cv::cvtColor(frame,img,CV_RGB2GRAY);
      //cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
      //threshold(gray, dst, thredshold_value, 200, 0);
      img.copyTo(pre_img[0],img)
      img.copyTo(pre_img[1],img)
   } 
    // （4）カメラから1フレーム分の画像データを取得して、変数frameに格納する
    cap >> frame;
    //cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
    //cv::equalizeHist(gray, img1);
    //threshold(gray, dst, thredshold_value, 200, 0);
    // cv::GaussianBlur(gray, img2, cv::Size(7,7), 10, 10);
    // cv::Canny(img2, dst, 40, 150);

    //差分1：フレーム1と2の差を求める
    cv::absdiff(pre_img[0], pre_img[1],dif[0]);
    //差分2：フレーム2と3の差を求める
    cv::absdiff(pre_img[0], img,dif[1]);
    //差分1と差分2の結果を比較(論理積)し、diffに出力
    cv::bitwise_and(dif[0],dif[1],dif[2]);
 
    //差分diffのうち、閾値thを超えている部分を1、それ以外を0としてmaskに出力
    cv::threshold(dif[2],gray,5,1,cv::THRESH_BINARY);
    //マスクmaskのうち、1(True)の部分を白(0)に、0(False)の部分を黒(255)にしてim_maskに出力
    cv::threshold(gray,dist,0,255,cv::THRESH_BINARY);
    //メディアンフィルタを使った平滑化によってゴマ塩ノイズを除去、アパーチャサイズ5
    cv::medianBlur(dist,dist,5);
    //ウィンドウ2枚にそれぞれ入力画像、差分画像を表示
    cv::imshow("in",frame);
    cv::imshow("out",dist);
 
    //新しいフレームをカメラから一つ取り出し、3つのフレームを全てずらす
    pre_img[1].copyTo(pre_img[0],pre_img[1]);
    img.copyTo(pre_img[1],img);
    cap>>frame;
    cv::cvtColor(frame,img,CV_RGB2GRAY);

    // 画像データ取得に失敗したらループを抜ける
    if (frame.empty()) break;

    // 取得した画像データをウィンドウ表示する
    cv::imshow("image", dst);

    //ウィンドウ上でEscキーが押されたらプログラム終了
    if(cv::waitKey(27)>=0)
    {
      cv::destroyAllWindows();
      break;
    }
  }
  return 0;
}