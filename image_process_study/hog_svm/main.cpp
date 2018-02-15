#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // videoioのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiのヘッダーをインクルード
#include <iostream>

#define Nbin (9)
#define Npixel (4)
#define Ncell  (4)
#define Data_Size (64)
cv::Rect selection;
int select_object;
void on_mouse (int event, int x, int y, int flags, void *param);
int main(int argc, const char* argv[])
{
//   // 画像データを格納するための変数を宣言する
//   cv::Mat frame;
//   cv::Mat dst, img_diff[2], dif[3], img, clone;

//   frame = cv::imread("seacow2.jpg");
//     if(!frame.data)
//     return -1;
//   cv::cvtColor(frame, img, cv::COLOR_RGB2GRAY);
//   cv::namedWindow("in", cv::WINDOW_AUTOSIZE);
//   cv::imshow("in", img); 
//   cvSetMouseCallback("in", (CvMouseCallback)(&on_mouse), &img);
//   while(!select_object){
//     if(cv::waitKey(27)>=0)//escをおされるまで　
//     {
//       cv::destroyAllWindows();
//       break;
//     }
//   }
//   while(select_object){
//     if(cv::waitKey(27)>=0)//escをおされるまで　
//     {
//       cv::destroyAllWindows();
//       break;
//     }
//   }
//   dst = img.clone();
//   //選択範囲の表示
//  // cv::rectangle(img, selection, 0xFF, 5);
//  // cv::imshow("in", img); 
//   //選択範囲への処理
//   cv::Mat roi(img, selection);
//   cv::resize(roi, dst, cv::Size(Data_Size, Data_Size));
//   dst.convertTo(dst, CV_32F, 1);
//   cv::GaussianBlur(dst, dst, cv::Size(7,7), 10, 10);
//   cv::Sobel(dst, img_diff[0], CV_32F, 1, 0);
//   cv::Sobel(dst, img_diff[1], CV_32F, 0, 1);
//   cv::Mat mag, angle; 
//   cv::cartToPolar(img_diff[0], img_diff[1], mag, angle, true); 
//   mag.convertTo(mag, CV_8U, 1);
//   cv::imshow("mag", mag);
//   // 勾配方向を[0, 180)にする
//   cv::add(angle, cv::Scalar(180.0), angle, angle < 0.0);
//   cv::add(angle, cv::Scalar(-360.0), angle, angle >= 360.0);
//   cv::add(angle, cv::Scalar(-180), angle, angle >= 180);
//   cv::add(angle, cv::Scalar(-180.0), angle, angle >= 180.0);
//   angle.convertTo(angle, CV_8U, 1);
//   angle /= (180/Nbin);
//   cv::add(angle, cv::Scalar(-1), angle, angle == Nbin);//五入防止
//   cv::imshow("angle", angle);

  cv::Mat src_img;
  
  int camera_id = 0;
  //cv::VideoCapture capture(cv::CAP_DSHOW + camera_id);
  cv::VideoCapture capture(camera_id);

  // （2）カメラIDが0番のカメラが正しくオープンできているかをチェックする（正しくオープンできていなければエラー終了する）
  if (!capture.isOpened())
    return -1;

  // （3）カメラキャプチャのフレームレートを30.0に指定する
  capture.set(cv::CAP_PROP_FPS, 30.0);

	//cv::VideoCapture capture;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	capture.open(0);

	cv::HOGDescriptor hog;
	cv::Mat mono_img;
	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

	std::vector<cv::Rect> found;
	while(true) {
		capture >> src_img;

		cv::cvtColor(src_img, mono_img, CV_BGR2GRAY);
		hog.detectMultiScale(mono_img, found);

		for(unsigned i = 0; i < found.size(); i++) {
			cv::Rect r = found[i];
			rectangle(src_img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
		}
		cv::imshow("test", src_img);

		int c = cv::waitKey(1);
		if( c == 27) break;
	}
 // cv::waitKey(0);

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