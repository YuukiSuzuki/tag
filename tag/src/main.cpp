#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <vector>
#include<stdio.h>

#include <ros/ros.h>
#include <ros/package.h>
#include <cmath>
#include <std_msgs/Float64MultiArray.h>

struct getDate
{
	int id=0;
	int markerDirection=0;
	cv::Point2f id_dstPint[4];
	//std::vector<cv::Point> id_approxcurve;
	cv::Point3d id_rotation[1];
	cv::Point3d id_translation[1];
	double a[4][4];
	double b[4];
	double c[3];
};
int count=0;
int count_1=0;

struct getDate id_nunber[10];

int decodeMarker(cv::Mat src);



int main(int argc, char *argv[]){
         
	////////////////////////////////////////////////////////////////////
	//画像に表示させる軸の準備
	#define MARKER_SIZE  (135) 
        /*マーカーの１辺のサイズ[mm]*/

        //カメラの内部パラメータ、歪み係数を読み込み
	cv::FileStorage fs2("/home/ryukoku/catkin_ws/src/tag/camera.xml",cv::FileStorage::READ);
	if(!fs2.isOpened()){
		std::cout << "File can not be opened" << std::endl;

		return -1;
	}
	
	cv::Mat intrinsic;
	cv::Mat distortion;

	fs2["intrinsic"] >> intrinsic;
	fs2["distortion"] >> distortion;
        //読み込みデータを表示
	//std::cout << "camera matrix:" << intrinsic << std::endl;
	//std::cout << "distortion coeffs:" << distortion << std::endl;

	cv::Mat object_points(4,3,CV_32FC1);
	cv::Mat image_points(4,1,CV_32FC2);

	cv::Mat rotation(1 ,3,CV_32FC1);
	cv::Mat translation(1 , 3, CV_32FC1);

	//軸の座標生成用　
	cv::Mat srcPoints3D(4 , 1,CV_32FC3);//元の３次元座標
	cv::Mat dstPoints2D(4 , 1,CV_32FC3);//画面に投影したときの２次元座標
	
	//MPSの長辺のサイズ
	cv::Mat AsrcPoints3D(4 , 1,CV_32FC3);//元の３次元座標(inputの場合)
	cv::Mat BsrcPoints3D(4 , 1,CV_32FC3);//元の３次元座標(outputの場合)
        cv::Mat BdstPoints2D(4 , 1,CV_32FC2);//画面に投影したときの２次元座標

	cv::Point3f baseMarkerPoints[4];
        //四角が物理空間上ではどの座標になるかを指定する。
	baseMarkerPoints[0].x=(float)0*MARKER_SIZE;
	baseMarkerPoints[0].y=(float)0*MARKER_SIZE;
	baseMarkerPoints[0].z=0.0;
        
	//baseMarkerPoints[1].x=(float)0*MARKER_SIZE;
        //baseMarkerPoints[1].y=(float)1*MARKER_SIZE;
	baseMarkerPoints[1].x=(float)1*MARKER_SIZE;
        baseMarkerPoints[1].y=(float)0*MARKER_SIZE;
	baseMarkerPoints[1].z=0.0;

	baseMarkerPoints[2].x=(float)1*MARKER_SIZE;
        baseMarkerPoints[2].y=(float)1*MARKER_SIZE;
        baseMarkerPoints[2].z=0.0;

        //baseMarkerPoints[3].x=(float)1*MARKER_SIZE;
        //baseMarkerPoints[3].y=(float)0*MARKER_SIZE;
	baseMarkerPoints[3].x=(float)0*MARKER_SIZE;
        baseMarkerPoints[3].y=(float)1*MARKER_SIZE;
        baseMarkerPoints[3].z=0.0;

	//軸の基本座標を求める
	for(int i=0 ;i<4;i++)
	{
		switch(i)
		{
			case 0: srcPoints3D.at<cv::Vec3f>(0,0) = {0,0,0};
				break;
			case 1: srcPoints3D.at<cv::Vec3f>(1,0) = {2*(float)MARKER_SIZE,0,0};
				break;
			case 2: srcPoints3D.at<cv::Vec3f>(2,0) = {0,(float)MARKER_SIZE,0};
				break;
			case 3: srcPoints3D.at<cv::Vec3f>(3,0) = {0,0,(float)MARKER_SIZE};
				break;
		}
	}
        //std::cout << "srcPoints3D:" << srcPoints3D << std::endl;

	for(int i=0 ;i<4;i++)
        {
                switch(i)
                {
                        case 0: AsrcPoints3D.at<cv::Vec3f>(0,0) = {-25.5,-25.5,0};
                                break;
                        case 1: AsrcPoints3D.at<cv::Vec3f>(1,0) = {44,-25.5,0};
                                break;
                        case 2: AsrcPoints3D.at<cv::Vec3f>(2,0) = {44,45,0};
                                break;
                        case 3: AsrcPoints3D.at<cv::Vec3f>(3,0) = {-25.5,44,0};
                                break;
                }
        }

        
	for(int i=0 ;i<4;i++)
        {
                switch(i)
                {
                        case 0: BsrcPoints3D.at<cv::Vec3f>(0,0) = {-32,-26,0};
                                break;
                        case 1: BsrcPoints3D.at<cv::Vec3f>(1,0) = {39,-26,0};
                                break;
                        case 2: BsrcPoints3D.at<cv::Vec3f>(2,0) = {39,45,0};
                                break;
                        case 3: BsrcPoints3D.at<cv::Vec3f>(3,0) = {-32,45,0};
                                break;
                }
        }
        //std::cout << "BsrcPoints3D:" << BsrcPoints3D << std::endl;
	

        //軸の準備　ここまで
	///////////////////////////////////////////////////////////////////////////////////////

	//カメラ取得
        /////////////////////////////////////////////////////////////////////////////////////
        cv::VideoCapture cap(2);//デバイスのオープン→
        //cap.open(0);//こっちでも良い．
        //上記の引数は、（０）はパソコンの内蔵カメラで、WEBカメラの場合、（１）で宣>言
        if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
        {
             //読み込みに失敗したときの処理
             return -1;
        }

        cv::Mat frame; //取得したフレーム

        //ROSのノード設定
        ros::init(argc , argv, "robo_tag");
        ros::NodeHandle nh;
	ros::Publisher Robo_pub = nh.advertise<std_msgs::Float64MultiArray>("tag_1",13);
	//
	ros::Rate loop_rate(10); 

//for(int x = 0; x < 1; x++){
while(cap.read(frame) && ros::ok()){

        std_msgs::Float64MultiArray array; 
	array.data.resize(13);
	cv::Mat imag, gray, binary, binarycontour, contourbinary, imagA, mapImg, binary_1;
	//,dst = cv::Mat::zeros(imag.rows, imag.cols, CV_8UC3); //画像格納用のオブジェクト宣言  
	// imag  輪郭近似データ用
	// gray グレースケールデータ用
	// binary　二値化データ用
	// binary_1 二値化２データ用
	// binarycontour　前輪郭抽出データ用
	// contourbinary　後輪郭抽出データ用
	// imagA 輪郭近似データだけ用
	// srcMarker カラーの生データ
	//cv::Mat srcMarker(500,500,CV_32FC1);
	cv::Mat srcMarker(90,90,CV_8U);
	
	cv::Point2f dstPoint[4];
	dstPoint[0] = cv::Point2f(0,0);
	dstPoint[1] = cv::Point2f(srcMarker.cols, 0);
	dstPoint[2] = cv::Point2f(srcMarker.cols,srcMarker.rows);
	dstPoint[3] = cv::Point2f(0, srcMarker.rows);

	char value[255]="";
	static int i = 0; //輪郭の数
	
	//imag8.jpg の場合
        //imagA= cv::imread("img8.jpg");

	//カメラの場合
        cv::imwrite("./img8.jpg", frame);
        gray = cv::imread("img8.jpg",0); //画像データの読み込み グレースケール
	//gray = cv::imread(frame,0); 
        imagA= cv::imread("img8.jpg");
	contourbinary= cv::imread("img8.jpg");//カラー画像を読み込み

	//
	//グレースケール
	

	if (gray.empty()) { //エラー処理
		std::cout << "error" << std::endl;
		return 0;
	}

	////////////////////////////画像データ保存
	//cv::imwrite("gray.jpg", gray); //グレースケール画像の保存
	
	//
	//二値化

	//std::cout << "閾値を入力してください" << std::endl;
	
	//int tsd_v;
	//std::cin >> tsd_v;

	//cv::threshold(gray,binary,tsd_v,255,cv::THRESH_BINARY); //二値化

        
	cv::adaptiveThreshold(gray,binary,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY,7,0); 
	// //cv::ADAPTIVE_THRESH_MEAN_C  違う手法画像処理
	//cv::threshold(gray,binary, 0, 255,cv::THRESH_BINARY | cv::THRESH_OTSU);
	//
	

	cv::threshold(gray,binary_1, 0, 255,cv::THRESH_BINARY | cv::THRESH_OTSU);
        
	////////////////////////////画像データ保存
	//cv::imwrite("binary.jpg", binary);      //二値化画像の保存
	//
	//
	
        //cv::imwrite("imagaa.png",contourbinary);//カラー画像を保存して確認
        
	//
	//輪郭抽出

	binarycontour = binary; //コピー
	std::vector< std::vector< cv::Point > > contours ; //輪郭の座標リスト
	std::vector<cv::Vec4i> lines; //階層　オプションの出力ベクター
	cv::findContours(binarycontour,contours,lines,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);//輪郭抽出
        

        ////////////////////////////画像データ保存	
	//輪郭のデータをカラー画像に上書き
	//for (auto contour=contours.begin() ; contour!=contours.end() ; contour++){
	//	cv::polylines(contourbinary,*contour,true , cv::Scalar(0,200,0),1);
	//}
        //
	//cv::imwrite("contourbinary.jpg", contourbinary); //輪郭抽出画像を保存
        // 
	//imag=cv::imread("contourbinary.jpg");
	for ( auto contour=contours.begin() ;contour!=contours.end(); contour++){

                std::vector<cv::Point > approx;

                //輪郭を直線近似する

		cv::approxPolyDP(cv::Mat(*contour),approx,0.001*cv::arcLength(*contour,true),true);
             /////////////////////////////////////////////////////////////////////////////////////////
	     std::vector<cv::Point> approxcurve_1;//直線近似データ保存
                cv::approxPolyDP(cv::Mat(*contour),approxcurve_1,0.05*cv::arcLength(*contour,true),true);
	      	double area_1 = cv::contourArea(approxcurve_1);//面積を保存
	     	if(approxcurve_1.size()==4 && area_1 > 100 && cv::isContourConvex(approxcurve_1)){
             //  
             //cv::polylines(imag,approx,true , cv::Scalar(0,0,200),2);
	      cv::polylines(imagA,approx,true , cv::Scalar(0,0,200),1);
	     	}

        }
        
        ////////////////////////////画像データ保存	
        //cv::imwrite("contourapprox.jpg",imag); 
	//cv::imwrite("contourapprox_1.jpg",imagA);

	//
	//四角形の座標と透視変換
 
	for ( auto contour=contours.begin();contour!=contours.end(); contour++){
		//輪郭を直線近似する
		std::ostringstream oss;//正方形データ
		std::vector<cv::Point> approxcurve;//直線近似データ保存
		cv::approxPolyDP(cv::Mat(*contour),approxcurve,0.05*cv::arcLength(*contour,true),true);
		double area = cv::contourArea(approxcurve);//面積を保存
		if(approxcurve.size()==4 && area > 100 /*&& cv::isContourConvex(approxcurve)*/){ //頂点が４つの場合と面積が1000以上の場合　凸型の場合
		
			cv::Point2f srcPoint[]={approxcurve[0], approxcurve[1], approxcurve[2], approxcurve[3]};//４点データをコピー
			cv::Mat map = cv::getPerspectiveTransform(srcPoint,dstPoint);//原点に寄せる
			cv::warpPerspective(gray , mapImg , map, srcMarker.size());//透視変換
			//cv::Mat candidateImg = mapImg.clone();
			//cv::cvtColor(mapImg,mapImg,cv::COLOR_BGR2GRAY);
			//cv::threshold(mapImg,mapImg,250/*tsd_v*/,255,cv::THRESH_BINARY);//二値化を行う
                        
			//確認
			//oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
                        //cv::imwrite("mapImg"+ oss.str() +  ".jpg",mapImg);

			////decodeMarker(mapImg)でマーカのIDと向きを判別
		        decodeMarker(mapImg);
			//マーカーでないと判断したらタグIDを０とする
			if(id_nunber[count].id!=0){
				//端末にIDを表示
				//std::cout << "ID="<< id_nunber[count].id << std::endl;
				//取得したIDの角度を表示
                                //std::cout <<"角度="<< id_nunber[count].markerDirection << std::endl;
				//マーカの４つのポイントを変更向きを統一
				if(id_nunber[count].markerDirection==0){
					id_nunber[count].id_dstPint[0]=approxcurve[0];
					id_nunber[count].id_dstPint[1]=approxcurve[1];
					id_nunber[count].id_dstPint[2]=approxcurve[2];
					id_nunber[count].id_dstPint[3]=approxcurve[3];
				}
				if(id_nunber[count].markerDirection==90){
                                        id_nunber[count].id_dstPint[0]=approxcurve[3];
                                        id_nunber[count].id_dstPint[1]=approxcurve[0];
                                        id_nunber[count].id_dstPint[2]=approxcurve[1];
                                        id_nunber[count].id_dstPint[3]=approxcurve[2];
				}
				if(id_nunber[count].markerDirection==180){
                                        id_nunber[count].id_dstPint[0]=approxcurve[2];
                                        id_nunber[count].id_dstPint[1]=approxcurve[3];
                                        id_nunber[count].id_dstPint[2]=approxcurve[0];
                                        id_nunber[count].id_dstPint[3]=approxcurve[1];
				}
				if(id_nunber[count].markerDirection==270){
                                        id_nunber[count].id_dstPint[0]=approxcurve[1];
                                        id_nunber[count].id_dstPint[1]=approxcurve[2];
                                        id_nunber[count].id_dstPint[2]=approxcurve[3];
                                        id_nunber[count].id_dstPint[3]=approxcurve[0];
				}


			//原点ポイントを表示	
                        //std::cout <<"原点="<< id_nunber[count].id_dstPint[0] << std::endl;
			//std::cout <<"原点="<< approxcurve << std::endl;

			//3次元のときのデータを計算
                        //マーカの４つのデータをコピー
		        image_points.at<cv::Vec2f>(0,0) = id_nunber[count].id_dstPint[0];
		        image_points.at<cv::Vec2f>(1,0) = id_nunber[count].id_dstPint[1];
		        image_points.at<cv::Vec2f>(2,0) = id_nunber[count].id_dstPint[2];
		        image_points.at<cv::Vec2f>(3,0) = id_nunber[count].id_dstPint[3];
		        //コピー
		        object_points.at<float>(0,0) = baseMarkerPoints[0].x;
			object_points.at<float>(0,1) = baseMarkerPoints[0].y;
			object_points.at<float>(0,2) = baseMarkerPoints[0].z;
			object_points.at<float>(1,0) = baseMarkerPoints[1].x;
			object_points.at<float>(1,1) = baseMarkerPoints[1].y;
			object_points.at<float>(1,2) = baseMarkerPoints[1].z;
			object_points.at<float>(2,0) = baseMarkerPoints[2].x;
			object_points.at<float>(2,1) = baseMarkerPoints[2].y;
			object_points.at<float>(2,2) = baseMarkerPoints[2].z;
			object_points.at<float>(3,0) = baseMarkerPoints[3].x;
			object_points.at<float>(3,1) = baseMarkerPoints[3].y;
			object_points.at<float>(3,2) = baseMarkerPoints[3].z;

			//確認
                        //std::cout << "imag=" << image_points << std::endl;
			//std::cout << "object=" << object_points << std::endl;
			//std::cout << "ID=" << id_nunber[count].id << std::endl;
                        //std::cout << "markerDirection=" << id_nunber[count].markerDirection << std::endl;
                        //std::cout << "id_dstPint=" << id_nunber[count].id_dstPint[0] << std::endl;
                        //std::cout << "id_dstPint=" << id_nunber[count].id_dstPint[1] << std::endl;
                        //std::cout << "id_dstPint=" << id_nunber[count].id_dstPint[2] << std::endl;
                        //std::cout << "id_dstPint=" << id_nunber[count].id_dstPint[3] << std::endl;

		        //std::cout << "id_rotation[1]" << id_nunber[i].id_rotation[0] << std::endl;
                        //std::cout << "id_translation[1]" << id_nunber[i].id_translation[0] << std::endl;
                        //std::cout << "a[0][0]=" << id_nunber[i].a[0][0] << std::endl;
                        //std::cout << "b[0]=" << id_nunber[i].b[0] << std::endl;
                        //std::cout << "c[0]=" << id_nunber[i].c[0] << std::endl;

			//回転ベクトルと並進ベクトルを取得
			cv::solvePnP(object_points,image_points,intrinsic,distortion,rotation,translation);

			//マーカーのカメラからの回転ベクトルと並進ベクトルを保存
			id_nunber[count].id_rotation[0].x = rotation.at<float>(0,0);
                        id_nunber[count].id_rotation[0].y = rotation.at<float>(1,0);
                        id_nunber[count].id_rotation[0].z = rotation.at<float>(2,0);
			id_nunber[count].id_translation[0].x = translation.at<float>(0,0);
			id_nunber[count].id_translation[0].y = translation.at<float>(1,0);
			id_nunber[count].id_translation[0].z = translation.at<float>(2,0);


			//確認
			//std::cout << "translation=" << id_nunber[count].tvec << std::endl;
			//小数点第６まで７を四捨五入
			//std::cout << "rotation=" << rotation << std::endl;
                        //std::cout << "translation=" << translation << std::endl;
			//std::cout << "回転ベクトル=" << id_nunber[count].id_rotation[0] << std::endl;
                        //std::cout << "並進ベクトル=" << id_nunber[count].id_translation[0] << std::endl;


			cv::projectPoints(srcPoints3D,rotation,translation,intrinsic,distortion,dstPoints2D);

			//std::cout <<"2次元座標="<< dstPoints2D << std::endl;
			cv::Point2f dstPoints2DD[4];
 			dstPoints2DD[0] = dstPoints2D.at<cv::Vec2f>(0,0);
                        dstPoints2DD[1] = dstPoints2D.at<cv::Vec2f>(1,0);
                        dstPoints2DD[2] = dstPoints2D.at<cv::Vec2f>(2,0);
                        dstPoints2DD[3] = dstPoints2D.at<cv::Vec2f>(3,0);
                        
		  
			////////////////////////////////画像にタグ名前と３次元を出力////////////////

			////valueでID＝認識したIDで保存
                        sprintf(value,"ID=%d",id_nunber[count].id);
			////
			//cv::polylines(imagA, approxcurve, true, cv::Scalar(0 ,255, 0), 2);
			//cv::circle(imagA, id_nunber[count].id_dstPint[0], 10, cv::Scalar(255, 0, 0));
			//cv::circle(imagA, id_nunber[count].id_dstPint[1], 10, cv::Scalar(0, 255, 0));
			//cv::circle(imagA, id_nunber[count].id_dstPint[2], 10, cv::Scalar(0, 0, 255));
			//cv::circle(imagA, id_nunber[count].id_dstPint[3], 10, cv::Scalar(255,255,0));
			cv::line(imagA,dstPoints2DD[0],dstPoints2DD[1],cv::Scalar(255, 0, 0),3);
			cv::line(imagA,dstPoints2DD[0],dstPoints2DD[2],cv::Scalar(0,255, 0),3);
			cv::line(imagA,dstPoints2DD[0],dstPoints2DD[3],cv::Scalar(0, 0, 255),3);
			cv::putText(imagA,value,cv::Point((dstPoints2DD[0].x+dstPoints2DD[2].x)/2,(dstPoints2DD[0].y+dstPoints2DD[1].y)/2),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,255),1.5);
                        ////////////////////////////////////////////////////////////////////////////

			//保存　タグ
                        //oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
			//cv::imwrite("mapImg"+ oss.str() +  ".jpg",mapImg);
                       
			count++;
			}
			//oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
                        //cv::imwrite("mapImg"+ oss.str() +  ".jpg",mapImg);


		}
			


	}
	//カメラの原点に移動
        for(int i=0; i<10 ; i++){
                if(id_nunber[i].id!=0){
		   //確認
	           //std::cout << "ID=" << id_nunber[i].id << std::endl;
                   //std::cout << "markerDirection=" << id_nunber[i].markerDirection << std::endl;
                   //std::cout << "id_dstPint=" << id_nunber[i].id_dstPint[0] << std::endl;
		   //std::cout << "id_dstPint=" << id_nunber[i].id_dstPint[1] << std::endl;
		   //std::cout << "id_dstPint=" << id_nunber[i].id_dstPint[2] << std::endl;
		   //std::cout << "id_dstPint=" << id_nunber[i].id_dstPint[3] << std::endl;
		   //std::cout << "id_rotation[1]" << id_nunber[i].id_rotation[0] << std::endl;
                   //std::cout << "id_translation[1]" << id_nunber[i].id_translation[0] << std::endl;
		   //std::cout << "a[0][0]=" << id_nunber[i].a[0][0] << std::endl;
		   //std::cout << "b[0]=" << id_nunber[i].b[0] << std::endl;
		   //std::cout << "c[0]=" << id_nunber[i].c[0] << std::endl;

                   cv::Mat rotation_points(3,3,CV_64F);
                   cv::Mat id_rotation(3,1,CV_64F);
                   cv::Mat id_translation(3,1,CV_64F);

                   id_rotation.at<double>(0,0) = id_nunber[i].id_rotation[0].x;
                   id_rotation.at<double>(1,0) = id_nunber[i].id_rotation[0].y;
                   id_rotation.at<double>(2,0) = id_nunber[i].id_rotation[0].z;

		   id_translation.at<double>(0,0) = id_nunber[i].id_translation[0].x;
		   id_translation.at<double>(1,0) = id_nunber[i].id_translation[0].y;
		   id_translation.at<double>(2,0) = id_nunber[i].id_translation[0].z;

                   cv::Rodrigues(id_rotation,rotation_points);

                   //確認
		   //std::cout << "並進行列=" << id_translation << std::endl;
                   //std::cout << "回転行列=" << id_rotation << std::endl;
                   //std::cout << "回転行列=" << rotation_points << std::endl;

                   cv::Mat rt_points(4,4 ,CV_64F);
                      
                   rt_points.at<double>(0,0) = rotation_points.at<double>(0,0);
                   id_nunber[i].a[0][0]=rotation_points.at<double>(0,0);
                   rt_points.at<double>(1,0) = rotation_points.at<double>(1,0);
                   id_nunber[i].a[1][0]=rotation_points.at<double>(1,0);
                   rt_points.at<double>(2,0) = rotation_points.at<double>(2,0);
                   id_nunber[i].a[2][0]=rotation_points.at<double>(2,0);
                   rt_points.at<double>(3,0) = 0.0;
                   id_nunber[i].a[3][0]=0.0;
                   rt_points.at<double>(0,1) = rotation_points.at<double>(0,1);
                   id_nunber[i].a[0][1]=rotation_points.at<double>(0,1);
                   rt_points.at<double>(1,1) = rotation_points.at<double>(1,1);
                   id_nunber[i].a[1][1]=rotation_points.at<double>(1,1);
                   rt_points.at<double>(2,1) = rotation_points.at<double>(2,1);
                   id_nunber[i].a[2][1]=rotation_points.at<double>(2,1);
                   rt_points.at<double>(3,1) = 0.0;
                   id_nunber[i].a[3][1]=0.0;
                   rt_points.at<double>(0,2) = rotation_points.at<double>(0,2);
                   id_nunber[i].a[0][2]=rotation_points.at<double>(0,2);
                   rt_points.at<double>(1,2) = rotation_points.at<double>(1,2);
                   id_nunber[i].a[1][2]=rotation_points.at<double>(1,2);
                   rt_points.at<double>(2,2) = rotation_points.at<double>(2,2);
                   id_nunber[i].a[2][2]=rotation_points.at<double>(2,2);
                   rt_points.at<double>(3,2) = 0.0;
                   id_nunber[i].a[3][2]=0.0;
                   rt_points.at<double>(0,3) = id_nunber[i].id_translation[0].x;
                   id_nunber[i].a[0][3]=id_nunber[i].id_translation[0].x;
                   rt_points.at<double>(1,3) = id_nunber[i].id_translation[0].y;
                   id_nunber[i].a[1][3]=id_nunber[i].id_translation[0].y;
                   rt_points.at<double>(2,3) = id_nunber[i].id_translation[0].z;
                   id_nunber[i].a[2][3]=id_nunber[i].id_translation[0].z;
                   rt_points.at<double>(3,3) = 1.0;
                   id_nunber[i].a[3][3]=1.0;

                   cv::Mat matA(4,4,CV_64F,id_nunber[i].a);
                   
		   rotation_points = rotation_points.t(); //rotation of inverse
		   id_translation = -rotation_points  * id_translation;

		   cv::Mat T = cv::Mat::eye(4, 4, rotation_points.type()); // T is 4x4
                   T( cv::Range(0,3), cv::Range(0,3) ) = rotation_points * 1; // copies R into T
                   T( cv::Range(0,3), cv::Range(3,4) ) = id_translation * 1; // copies tvec into T

		   //std::cout << "逆=" << T << std::endl;


		   //確認
		   //std::cout << "=" << rt_points <<std::endl;
                   //std::cout << "回転ー並進行列=" << matA <<std::endl;

		   //４点のデータがほしいため
		   double ac[4]={0.0,0.0,0.0,1.0};//タグ原点座標
		   double ad[4]={67.5,0.0,82.0,1.0};//タグ座標系でのロボットの中心座標
                  
                   cv::Mat matB(4,1,CV_64F,ac);//タグ原点の行列式
		   cv::Mat matC(4,1,CV_64F,ad);//タグ座標系でのロボットの中心座標の行列式
                   cv::Mat matX(4,1,CV_64F);//カメラ原点座標系でのタグ原点の座標
		   cv::Mat matY(4,1,CV_64F);//タグ原点座標系でのカメラ原点の座標
		   
                   matX=matA*matC;
		   std::cout << "ID=" << id_nunber[i].id << std::endl;
                   std::cout << "カメラ原点座標でのタグ原点の座標=" << matX << std::endl;
                   matY =T*matB;
                   std::cout << "タグ原点座標系でのカメラ原点の座標=" << matY << std::endl;
		   //確認 タグ原点座標系でのカメラ原点の座標をカメラ座標系に戻すと０となる　
                   //matY = matA*matY;
		   //std::cout << "タグ原点座標系でのカメラ原点の座標をカメラ座標系に戻す=" << matY << std::endl;

		   //タグ原点座標系でのカメラ原点の座標をロボットの中心座標系でのカメラ原点の座標に変換
                     //タグとロボットの中心の関係
		     //数字はタグのID
		     //　　robotino1  robotino2    robotino3　　タグ　　　ロボット
		     //      ↑　前       ↑　前       ↑　前　　　　→　X　　前Y
		     //     _1__       _33_         _65_　　　　↓          ↑
		     //    |    |     |    |       |    |　　　　Y　　　　　→　ｘ
		     //  2 |    |18 34|    |178  66|    |82
		     //     ____       ____         ____
                     //      17        177           81
                   
                     if(id_nunber[i].id==1 || id_nunber[i].id==33 || id_nunber[i].id==65){
			     id_nunber[i].c[0] = 67.5-matY.at<double>(0,0);
			     id_nunber[i].c[1] = 82.0-matY.at<double>(2,0);
			     id_nunber[i].c[2] = -matY.at<double>(1,0);
		     }
	             if(id_nunber[i].id==2 || id_nunber[i].id==34 || id_nunber[i].id==66){   
                             id_nunber[i].c[0] = matY.at<double>(2,0)-82.0;
			     id_nunber[i].c[1] = 67.5-matY.at<double>(0,0);
			     id_nunber[i].c[2] = -matY.at<double>(1,0);

                     }
                     if(id_nunber[i].id==17 || id_nunber[i].id==177 || id_nunber[i].id==81){
			     id_nunber[i].c[0] = matY.at<double>(0,0)-67.5;
			     id_nunber[i].c[1] = matY.at<double>(2,0)-82.0;
			     id_nunber[i].c[2] = -matY.at<double>(1,0);

                     }
                     if(id_nunber[i].id==18 || id_nunber[i].id==178 || id_nunber[i].id==82){ 
			     id_nunber[i].c[0] = 82.0-matY.at<double>(2,0);
			     id_nunber[i].c[1] = matY.at<double>(0,0)-67.5;
			     id_nunber[i].c[2] = -matY.at<double>(1,0); 

                     }
                   std::cout << "ID=" << id_nunber[i].id << std::endl;
		   std::cout << "ロボット中心からカメラ座標ｘ=" << id_nunber[i].c[0] << std::endl;
		   std::cout << "ロボット中心からカメラ座標y=" << id_nunber[i].c[1] << std::endl;
		   std::cout << "ロボット中心からカメラ座標ｚ=" << id_nunber[i].c[2] << std::endl;
	     
                   id_nunber[i].b[0] = matX.at<double>(0,0);
                   id_nunber[i].b[1] = matX.at<double>(1,0);
                   id_nunber[i].b[2] = matX.at<double>(2,0);
                   id_nunber[i].b[3] = matX.at<double>(3,0);

		   std::cout << "カメラ座標からロボットの中心ｘ=" << id_nunber[i].b[0] << std::endl;
                   std::cout << "カメラ座標からロボットの中心y=" << id_nunber[i].b[1] << std::endl;
                   std::cout << "カメラ座標からロボットの中心ｚ=" << id_nunber[i].b[2] << std::endl;
                   double s;
                   s = sqrt((id_nunber[i].b[0]*id_nunber[i].b[0]) + (id_nunber[i].b[1]*id_nunber[i].b[1]) + (id_nunber[i].b[2]*id_nunber[i].b[2]));
		   std::cout << "カメラ座標からロボットの中心距離=" << s << std::endl;

		    s = sqrt((id_nunber[i].c[0]*id_nunber[i].c[0]) + (id_nunber[i].c[1]*id_nunber[i].c[1]) + (id_nunber[i].c[2]*id_nunber[i].c[2]));

		   std::cout << "ロボット中心からカメラ座標距離=" << s << std::endl;
		   //開放
		   rotation_points.release();
		   id_rotation.release();
		   id_translation.release();
		   rt_points.release();
		   matA.release();
		   T.release();
		   matB.release();
		   matC.release();
		   matX.release();
		   matY.release();
		}
	}
	double tx1=0,ty1=0,tz1=0,tx2=0,ty2=0,tz2=0,tx3=0,ty3=0,tz3=0;
	double cx1=0,cy1=0,cz1=0,cx2=0,cy2=0,cz2=0,cx3=0,cy3=0,cz3=0;
	int count1=0,count2=0,count3=0,count4=0;

	for(int i=0; i<10 ; i++){
		if(id_nunber[i].id!=0){
			//robotino1
			if(id_nunber[i].id==1 || id_nunber[i].id==2 || id_nunber[i].id==17 || id_nunber[i].id==18){
                             tx1=tx1+id_nunber[i].c[0];
			     ty1=ty1+id_nunber[i].c[1];
			     tz1=tz1+id_nunber[i].c[2];
			     cx1=cx1+id_nunber[i].b[0];
			     cy1=cy1+id_nunber[i].b[1];
			     cz1=cz1+id_nunber[i].b[2];
			     count1++;
			}
			//robotino2
		        if(id_nunber[i].id==33 || id_nunber[i].id==34 || id_nunber[i].id==177 || id_nunber[i].id==178){
		             tx2=tx2+id_nunber[i].c[0];
                             ty2=ty2+id_nunber[i].c[1];
                             tz2=tz2+id_nunber[i].c[2];
                             cx2=cx2+id_nunber[i].b[0];
                             cy2=cy2+id_nunber[i].b[1];
                             cz2=cz2+id_nunber[i].b[2];
                             count2++;
                        }
			//robotino3
                        if(id_nunber[i].id==65 || id_nunber[i].id==66 || id_nunber[i].id==81 || id_nunber[i].id==82){
			     tx3=tx3+id_nunber[i].c[0];
                             ty3=ty3+id_nunber[i].c[1];
                             tz3=tz3+id_nunber[i].c[2];
                             cx3=cx3+id_nunber[i].b[0];
                             cy3=cy3+id_nunber[i].b[1];
                             cz3=cz3+id_nunber[i].b[2];
                             count3++;
                        }
		}
	}


                         
        if(count1!=0){ 
        tx1=tx1/count1;
	ty1=ty1/count1;
	tz1=tz1/count1;
	cx1=cx1/count1;
	cy1=cy1/count1;
	cz1=cz1/count1;
        array.data[count4*7] = 1.0;
	array.data[count4*7+1] = tx1;
	array.data[count4*7+2] = ty1;
	array.data[count4*7+3] = tz1;
	array.data[count4*7+4] = cx1;
	array.data[count4*7+5] = cy1;
	array.data[count4*7+6] = cz1;
	count4++;
	}
        if(count2!=0){       
        tx2=tx2/count2;
        ty2=ty2/count2;
        tz2=tz2/count2;
        cx2=cx2/count2;
        cy2=cy2/count2;
        cz2=cz2/count2;
	array.data[count4*7] = 2.0;
        array.data[count4*7+1] = tx2;
        array.data[count4*7+2] = ty2;
        array.data[count4*7+3] = tz2;
        array.data[count4*7+4] = cx2;
        array.data[count4*7+5] = cy2;
        array.data[count4*7+6] = cz2;
	count4++;
        }
	if(count3!=0){
        tx3=tx3/count3;
        ty3=ty3/count3;
        tz3=tz3/count3;
        cx3=cx3/count3;
        cy3=cy3/count3;
        cz3=cz3/count3;
	array.data[count4*7] = 3.0;
        array.data[count4*7+1] = tx3;
        array.data[count4*7+2] = ty3;
        array.data[count4*7+3] = tz3;
        array.data[count4*7+4] = cx3;
        array.data[count4*7+5] = cy3;
        array.data[count4*7+6] = cz3;
        count4++;
        }
        
	Robo_pub.publish(array);
	ROS_INFO("I published array!");
        ros::spinOnce();
        loop_rate.sleep();







//        std::cout<<"m1="<< mapImg  << std::endl;
//	for(int y = 0; y < mapImg.rows; y++){
//	for(int x = 0; x < mapImg.cols; x++){
//			std::cout << (int)mapImg.at<unsigned char>(x,y) << std::endl;
//	}
//}

	//cv::namedWindow("Components",1);
        cv::imshow("win",imagA);
	const int key = cv::waitKey(1);
        if(key == 'q'/*113*/)//qボタンが押されたとき
        { 
	    //開放
        
        gray.release();
        binary.release();
	binary_1.release();
        binarycontour.release();
        contourbinary.release();
        imag.release();
        imagA.release();
        mapImg.release();
        srcMarker.release();
	
            break;//whileループから抜ける．
        }
        else if(key == 's'/*115*/)//sが押されたとき
        {
            //フレーム画像を保存する．
            cv::imwrite("imagA.jpg",imagA);
        }
	
	i = 0 ;
        count = 0;
	count_1 =0;
        //printf("ループの回数；%d\n", x);



	//開放
	
        gray.release();
	binary.release();
	binary_1.release();
        binarycontour.release();
	contourbinary.release();
	imag.release();
	imagA.release();
	mapImg.release();
	srcMarker.release();
}
        cv::destroyAllWindows();

        intrinsic.release();
	distortion.release();
	object_points.release();
	image_points.release();
	rotation.release();
	translation.release();
	srcPoints3D.release();
	dstPoints2D.release();
	AsrcPoints3D.release();
	BsrcPoints3D.release();
        BdstPoints2D.release();

	return 0;
	}


int decodeMarker(cv::Mat src)
{
        
        id_nunber[count].id=0;	
	if(src.channels()!=1)
	{ //チャンネルが１つであること
		id_nunber[count].id=0;
		return -1;
	}

	if(src.cols!=src.rows)
	{//縦と横が同じサイズであること
		id_nunber[count].id=0;
		return -1;
	}
       
	//マーカードットを解析する
	//周りの２ドットが黒（●　）あるか判断
	//Xで表示したところが白か黒かを判断して、数字に変換して戻す。
	//●　●　●　●　●　●　●　●　●
	//●　●　●　●　●　●　●　●　●
	//●　●　X  X  X  X  X  ●　●
	//●　●　X  X  X  X  X  ●　●
	//●　●  X  X  X  X  X  ●　●
	//●　●　X  X  X  X  X  ●　●
	//●　●　X  X  X  X  X  ●　●
	//●　●　●　●　●　●　●　●　●
	//●　●　●　●　●　●　●　●　●
	//
	
	cv::Mat src_1 = src; 
	int dot[90][90];
        int judgment;
	int markerDirection;
	int src_result[5][5];
	//int *src_return[2];

	int cellRow=9; //矢で横
	int cellCol=9; //柱で縦
	int result[9][9];
	int cellsize=10;  //pixcel size of cell

	//外周の１ドットの黒の平均の値を二値化のしきい値し二値化する
	//生データを行列式に変換
	for(int y = 0; y < src_1.rows; y++)
        {
                for(int x = 0; x < src_1.cols; x++)
                {
                         dot[x][y]=(int)src_1.at<unsigned char>(x,y);
                }
        }
        
        //RとCに分けて以下の白黒判定のように平均の数値を求めてしきい値とする
	//R,Cは黒  Fは白
	//R　R　R　R　R　R　R　R　R
        //C　●　●　●　●　●　●　●　C
        //C　●　●  ●  ●  ●  ●  ●　C 
        //C　●　●  ●  ●  ●  ●  ●　C 
        //C　●  ●  ●  F  ●  ●  ●　C 
        //C　●　●  ●  ●  ●  ●  ●　C 
        //C　●　●  ●  ●  ●  ●  ●　C 
        //C　●　●　●　●　●　●　●　C 
        //R　R　R　R　R　R　R　R　R
        //
        int sumR=0;
	int sumC=0;
	int sumRC=0;
	int sumF=0;
	for(int y= 0; y < cellRow ; y++)
	{
		for(int x = 0 ; x < 2 ; x++)
		{
			for(int celly=2 ; celly <8 ; celly++)
			{
				for(int cellx=2 ; cellx <8 ; cellx++)
				{
					sumR= sumR + dot[cellx + ((x*8)*10)][celly + (y*10)];
				}
			}
                       
		}
	}
	sumR = sumR/(((6*6)*2)*9);

	for(int y= 0; y < 2 ; y++)
        {
                for(int x = 1 ; x < 8 ; x++)
                {
                        for(int celly=2 ; celly <8 ; celly++)
                        {
                                for(int cellx=2 ; cellx <8 ; cellx++)
                                {
                                        sumC= sumC + dot[cellx + (x*10)][celly + ((y*8)*10)];
                                }
                        }

                }
        }
        sumC = sumC/(((6*6)*2)*7);
        
	sumRC = (((((sumR + sumC )*10000)/(2*255))/*+215*/)*255)/10000;
	//sumRC = 255 - sumRC ;
	//sumRC = (sumR + sumC )/2;
        //printf("しきい値黒：%d\n", sumRC );

                        for(int celly=2 ; celly <8 ; celly++)
                        {
                                for(int cellx=2 ; cellx <8 ; cellx++)
                                {
                                        sumF= sumF + dot[cellx + (4*10)][celly + (4*10)];
                                }
                        }

        sumF = sumF/(6*6);
	//printf("しきい値白：%d\n", sumF );

	sumRC = (sumRC + sumF)/2;
        //printf("しきい値：%d\n", sumRC );

	//二値化
	cv::threshold(src_1,src,sumRC,255,cv::THRESH_BINARY);
        //確認
        std::ostringstream oss_1;//正方形データ
        oss_1 << std::setfill( '0' ) << std::setw( 3 ) << count_1++;
        //cv::imwrite("mapImg_1"+ oss_1.str() +  ".jpg",src);

        src_1.release();

        
	//結果の初期化
        for (int i=0;i<9;i++)
	{
		for (int j=0;j<9;j++)
		{
			result[i][j]=0;
		}
	}

	//生データを行列式に変換
	for(int y = 0; y < src.rows; y++)
	{
		for(int x = 0; x < src.cols; x++)
		{
                         dot[x][y]=(int)src.at<unsigned char>(x,y);
		}
	}

	//cellsizeでの白黒の判定
	//    cellsize=10
	//  0 1 2 3 4 5 6 7 8 9 
	// 0- - - - - - - - - -
	// 1|                 |
	// 2|   - - - - - -   |
	// 3|   |         |   |
	// 4|   |  中枠   |   |
	// 5|   |  平均   |   |
	// 6|   |         |   |
	// 7|   - - - - - -   |
	// 8|                 |
	// 9- - - - - - - - - -
	//
	//　　220<白<=255   0<=黒<25  
	//    result 白　1  黒　0
	for(int y= 0; y < cellRow ; y++)
	{
		for(int x=0; x< cellCol; x++)
		{
			int sum=0;
			for(int celly=2;celly < 8; celly++)
			{
			   for(int cellx=2;cellx < 8; cellx++)
			   {
				sum=sum+dot[cellx + (x*10)][celly + (y*10)];
			   }
			}
			sum=sum/(6*6);

		if( 0<=  sum && sum <= 127){
			result[x][y] = 0;
		}
		else if( 127 <  sum && sum <= 255 ){
			result[x][y] = 1;
		}
                else{
			result[x][y] = 3;
		}
		}
	}
			
        judgment=0;
	judgment=result[0][0]+result[1][0]+result[2][0]+result[3][0]+result[4][0]+result[5][0]+result[6][0]+result[7][0]+result[8][0];        
	judgment=judgment+result[0][1]+result[1][1]+result[2][1]+result[3][1]+result[4][1]+result[5][1]+result[6][1]+result[7][1]+result[8][1];
	judgment=judgment+result[0][2]+result[1][2]+result[7][2]+result[8][2];
	judgment=judgment+result[0][3]+result[1][3]+result[7][3]+result[8][3];
	judgment=judgment+result[0][4]+result[1][4]+result[7][4]+result[8][4];
	judgment=judgment+result[0][5]+result[1][5]+result[7][5]+result[8][5];
	judgment=judgment+result[0][6]+result[1][6]+result[7][6]+result[8][6];
	judgment=judgment+result[0][7]+result[1][7]+result[2][7]+result[3][7]+result[4][7]+result[5][7]+result[6][7]+result[7][7]+result[8][7];
	judgment=judgment+result[0][8]+result[1][8]+result[2][8]+result[3][8]+result[4][8]+result[5][8]+result[6][8]+result[7][8]+result[8][8];




        //外枠が２つ黒の場合正常なタグとみなす
	if(judgment!=0){
		id_nunber[count].id=0;
		return -1;
	}
//        for(int y = 0; y < 9; y++){
//        for(int x = 0; x < 9; x++){
//                        std::cout << result[x][y] << std::endl;
//        }
//}

	//タグの検出　表示の向きも考慮　０度 Direction=0　９０度 Direction=1　１８０度 Direction=2　２７０度 Direction=3
	
	for(int Direction=0; Direction < 4 ; Direction++)
	{
		if(Direction==0)
		{
			//0度回転　真ん中を抜き出す
			for(int y=0;y < 5;y++)
			{
				for(int x=0;x<5;x++)
				{
			                src_result[x][y]=result[x+2][y+2];
				}
			}
		}
		else if(Direction==1)
		{
			//９０度回転　真ん中を抜き出す
			for(int y=0; y<5 ;y++)
			{
				for(int x=0;x<5;x++)
				{
					src_result[x][y]=result[5-1-y+2][x+2];
				}
			}
		}
		else if(Direction==2)
		{
			//１８０度回転　真ん中を抜き出す
			for(int y=0; y<5;y++)
			{
				for(int x=0;x<5;x++)
				{
					src_result[x][y]=result[5-1-x+2][5-1-y+2];
				}
			}
		}
		else if(Direction==3)
		{
			//２７０度回転　真ん中を抜き出す
			for(int y=0; y<5;y++)
			{
				for(int x=0;x<5;x++)
				{
					src_result[x][y]=result[y][5-1-x+2];
				}
			}
		}

		       //for(int y = 0; y < 5; y++){
                       //for(int x = 0; x < 5; x++){
                       // std::cout << src_result[x][y] << std::endl;
                       //  }
                       //  }


		//タグIDを検索   黒　0 白　1 
		
		if( src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
	            src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
		    src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
		    src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
		    src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
		{
			//ID 177 C-RS2 Input
			id_nunber[count].id=177;
			id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 178 C-RS2 Output
                        id_nunber[count].id=178;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
		{
			//ID 65 C-BS Input
			id_nunber[count].id=65;
			id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
		{
			//ID 66 C-BS Output
                        id_nunber[count].id=66;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 81 C-DS Input 
                        id_nunber[count].id=81;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 82 C-DS Output
                        id_nunber[count].id=82;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 1 C-CS1 Input
                        id_nunber[count].id=1;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 2 C-CS1 Output
                        id_nunber[count].id=2;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 17 C-CS2 Input
                        id_nunber[count].id=17;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 18 C-CS2 Output
                        id_nunber[count].id=18;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 33 C-RS1 Input 
                        id_nunber[count].id=33;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 34 C-RS1 Output 
                        id_nunber[count].id=34;
                        id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 193 C-SS Input 
                        id_nunber[count].id=193;
                        id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 194 C-SS Output 
                        id_nunber[count].id=194;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
		{
                        //ID 97 M-CS1 Input 
                        id_nunber[count].id=97;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 98 M-CS1 Output 
                        id_nunber[count].id=98;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==1 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 113 M-CS1 Input 
                        id_nunber[count].id=113;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==1 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 114 M-CS1 Output 
                        id_nunber[count].id=114;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 129 M-RS1 Input 
                        id_nunber[count].id=129;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 130 M-RS1 Output 
                        id_nunber[count].id=130;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 145 M-RS2 Input 
                        id_nunber[count].id=145;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 146 M-RS2 Output 
                        id_nunber[count].id=146;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 161 M-BS Input 
                        id_nunber[count].id=161;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 162 M-BS Output 
                        id_nunber[count].id=162;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 49 M-DS Input 
                        id_nunber[count].id=49;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 50 M-DS Output 
                        id_nunber[count].id=50;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 209 M-SS Input 
                        id_nunber[count].id=209;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 210 M-SS Output 
                        id_nunber[count].id=210;
                        id_nunber[count].markerDirection=Direction*90;
                }










                
                if(id_nunber[count].id!=0){
			break;
		}

		
	}


	return 0;

}
