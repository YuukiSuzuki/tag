//openCVによるUSBカメラ画像の取得
//URL;https://qiita.com/vs4sh/items/4a9ce178f1b2fd26ea30
//公式；URL；https://docs.opencv.org/3.0-beta/modules/videoio/doc/reading_and_writing_video.html
#include <sstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

int main(void)
{   
    cv::VideoCapture cap(2);//デバイスのオープン→
    //cap.open(0);//こっちでも良い．
    //上記の引数は、（０）はパソコンの内蔵カメラで、WEBカメラの場合、（１）で宣言
    if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
    {
        //読み込みに失敗したときの処理
        return -1;
    }
    int i=0;
    cv::Mat frame; //取得したフレーム
    while(cap.read(frame))//無限ループ
    {
        //
        //取得したフレーム画像に対して，クレースケール変換や2値化などの処理を書き込む．
        //
	const int key = cv::waitKey(1);
        cv::imshow("win", frame);//画像を表 示．
        if(key == 's'/*113*/)//qボタンが押されたとき
        {
        //保存
        std::ostringstream oss;
        oss << std::setfill( '0' ) << std::setw( 2 ) << i++;
        cv::imwrite("calib_img\\"+ oss.str() +  ".jpg",frame);
        }
	//cv::imwrite("/home/ryukoku/tag/img.jpg", frame);
        //cv::waitKey(33);//33ms
        if(key == 'q'/*113*/)//qボタンが押されたとき
        {
            break;//whileループから抜ける．
        }

    }
    cv::destroyAllWindows();
    return 0;
}
