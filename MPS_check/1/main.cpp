#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <vector>

struct getDate
{
	int id=333;
	int markerDirection=0;
	cv::Point2f id_dstPint[4];//タグの頂点４点（2次元）
	cv::Point2f MPS_Pint[4]; //MPSの長辺の頂点４点　画像の２次元で取得
	cv::Point3d id_rotation[1]; //回転ベクトル
        cv::Point3d id_translation[1]; //並進（平行）ベクトル
	double a[4][4]; //タグ座標系からカメラ座標系に変換する行列

	double b[4];//カメラ座標系のMPSの中心座標

	double f[4][4]; //カメラ座標系からタグ座標系に変換する行例


};

struct refboxMPS
{ 
	//C Cyan M Magenta
	//C-CS1 10 C-CS2 11 C-RS1 12 C-RS2 13 C-BS 14 C-DS 15 C-SS 16
	//M-CS1 20 M-CS2 21 M-RS1 22 M-RS2 23 M-BS 24 M-DS 25 M-SS 26
	//int mps = 0; //MPSの名前上記の数字で表現
	double x = 0; //フィールドの絶対座標x     -> x
	double y = 0; //フィールドの絶対座標y     ↑　y
	double a = 0; //MPSの向き

};

int count=0;

struct getDate id_nunber[14];

struct refboxMPS RefboxMPS[2][7]; //[0:Cyan,1:Magenta][CS1 0 CS2 1 RS1 2 RS2 3 BS 4 DS 5 SS 6] 

int decodeMarker(cv::Mat src);

int main(){
        //RefBoxのデータを入れる
	///////////////////////////////////////////////////////////
	//1/10スケール使用 2020年度　鈴木勇貴　修士論文　図2.1　参考
        //C-CS1 x -450 mm y 450 mm a 45度
        //RefboxMPS[0].mps = 10;
	RefboxMPS[0][0].x = -450;
	RefboxMPS[0][0].y = 450;
	RefboxMPS[0][0].a = 45;
        //C-CS2 x  350 mm  y 650 mm a 0度
	//RefboxMPS[1].mps = 11;
        RefboxMPS[0][1].x = 350;
        RefboxMPS[0][1].y = 650;
        RefboxMPS[0][1].a = 0;
        //C-RS1 x  250 mm  y 250 mm a 45度
	//RefboxMPS[2].mps = 12;
        RefboxMPS[0][2].x = 250;
        RefboxMPS[0][2].y = 250;
        RefboxMPS[0][2].a = 45;
        //C-RS2 x  50 mm  y 350 mm a 90度
	//RefboxMPS[3].mps = 13;
        RefboxMPS[0][3].x = 50;
        RefboxMPS[0][3].y = 350;
        RefboxMPS[0][3].a = 90;
	//C-BS x  650 mm  y 550 mm a 90度
	//RefboxMPS[4].mps = 14;
        RefboxMPS[0][4].x = 650;
        RefboxMPS[0][4].y = 550;
        RefboxMPS[0][4].a = 90;
        //C-DS x 650 mm  y 150 mm a 135度
	//RefboxMPS[5].mps = 15;
        RefboxMPS[0][5].x = 650;
        RefboxMPS[0][5].y = 150;
        RefboxMPS[0][5].a = 135;
        //C-SS x -150 mm  y 750 mm a 180度
	//RefboxMPS[6].mps = 16;
        RefboxMPS[0][6].x = -150;
        RefboxMPS[0][6].y = 750;
        RefboxMPS[0][6].a = 180;

	//M-CS1 x 450 mm  y 450 mm a 135度
	//RefboxMPS[7].mps = 20;
        RefboxMPS[1][0].x = 450;
        RefboxMPS[1][0].y = 450;
        RefboxMPS[1][0].a = 135;
	//M-CS2 x -350 mm  y 650 mm a 180度
	//RefboxMPS[8].mps = 21;
        RefboxMPS[1][1].x = -350;
        RefboxMPS[1][1].y = 650;
        RefboxMPS[1][1].a = 180;
	//M-RS1 x -250 mm  y 250 mm a 135度
	//RefboxMPS[9].mps = 22;
        RefboxMPS[1][2].x = -250;
        RefboxMPS[1][2].y = 250;
        RefboxMPS[1][2].a = 135;
	//M-RS2 x -50 mm  y 350 mm a 90度
	//RefboxMPS[10].mps = 23;
        RefboxMPS[1][3].x = -50;
        RefboxMPS[1][3].y = 350;
        RefboxMPS[1][3].a = 90;
	//M-BS x -650 mm  y 550 mm a 90度
	//RefboxMPS[11].mps = 24;
        RefboxMPS[1][4].x = -650;
        RefboxMPS[1][4].y = 550;
        RefboxMPS[1][4].a = 90;
	//M-DS x  -650 mm  y 150 mm a 45度
	//RefboxMPS[12].mps = 25;
        RefboxMPS[1][5].x = -650;
        RefboxMPS[1][5].y = 150;
        RefboxMPS[1][5].a = 45;
	//M-SS  x  150 mm  y 750 mm a 0度
	//RefboxMPS[13].mps = 26;
        RefboxMPS[1][6].x = 150;
        RefboxMPS[1][6].y = 750;
        RefboxMPS[1][6].a = 0;


        ///////////////////////////////////////////////////////////	

	#define MARKER_SIZE  (13) 
        /*マーカーの１辺のサイズ[mm]*/

        //カメラの内部パラメータ、歪み係数を読み込み
	cv::FileStorage fs2("camera.xml",cv::FileStorage::READ);
	if(!fs2.isOpened()){
		std::cout << "File can not be opened" << std::endl;

		return -1;
	}
        
	//カメラ取得
        /////////////////////////////////////////////////////////////////////////////////////
        //cv::VideoCapture cap(2);//デバイスのオープン→
        //cap.open(0);//こっちでも良い．
        //上記の引数は、（０）はパソコンの内蔵カメラで、WEBカメラの場合、（１）で宣>言
        //if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
        //{
        //     //読み込みに失敗したときの処理
        //     return -1;
        //}

	cv::Mat imagA;
	cv::Mat frame; //取得したフレーム
        //cap.read(frame);//カメラ取得の場合
	
	//imag8.jpg の場合
        imagA= cv::imread("img8.jpg");

	//カメラの場合
	//imagA = frame;
	
        ////////////////////////////////////////////////////////////////////
        //画像に表示させる軸の準備


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
        
        //baseMarkerPoints[1].x=(float)1*MARKER_SIZE;
        //baseMarkerPoints[1].y=(float)0*MARKER_SIZE;	
	baseMarkerPoints[1].x=(float)0*MARKER_SIZE;
        baseMarkerPoints[1].y=(float)1*MARKER_SIZE;
        baseMarkerPoints[1].z=0.0;

	baseMarkerPoints[2].x=(float)1*MARKER_SIZE;
        baseMarkerPoints[2].y=(float)1*MARKER_SIZE;
        baseMarkerPoints[2].z=0.0;

	//baseMarkerPoints[3].x=(float)0*MARKER_SIZE;
        //baseMarkerPoints[3].y=(float)1*MARKER_SIZE;
	baseMarkerPoints[3].x=(float)1*MARKER_SIZE;
        baseMarkerPoints[3].y=(float)0*MARKER_SIZE;
        baseMarkerPoints[3].z=0.0;

	//軸の基本座標を求める
	for(int i=0 ;i<4;i++)
	{
		switch(i)
		{
			case 0: srcPoints3D.at<cv::Vec3f>(0,0) = {0,0,0};
				break;
			case 1: srcPoints3D.at<cv::Vec3f>(1,0) = {(float)MARKER_SIZE,0,0};
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
                switch(i)// おもて
                {
                        case 0: AsrcPoints3D.at<cv::Vec3f>(0,0) = {-25.5,-25.5,0.0};
                                break;
                        case 1: AsrcPoints3D.at<cv::Vec3f>(1,0) = {-25.5,44,0.0};
                                break;
                        case 2: AsrcPoints3D.at<cv::Vec3f>(2,0) = {45,44,0.0};
                                break;
                        case 3: AsrcPoints3D.at<cv::Vec3f>(3,0) = {45,-25.5,0.0};
                                break;
                }
        }

        
	for(int i=0 ;i<4;i++)
        {
                switch(i)//うら
                {
                        case 0: BsrcPoints3D.at<cv::Vec3f>(0,0) = {-26,-32,0.0};
                                break;
                        case 1: BsrcPoints3D.at<cv::Vec3f>(1,0) = {-26,39,0.0};
                                break;
                        case 2: BsrcPoints3D.at<cv::Vec3f>(2,0) = {45,39,0.0};
                                break;
                        case 3: BsrcPoints3D.at<cv::Vec3f>(3,0) = {45,-32,0.0};
                                break;
                }
        }
        //std::cout << "BsrcPoints3D:" << BsrcPoints3D << std::endl;
	

        //軸の準備　ここまで
	///////////////////////////////////////////////////////////////////////////////////////

	cv::Mat imag,gray, binary,binarycontour,contourbinary,mapImg,binary_1;//,dst = cv::Mat::zeros(imag.rows, imag.cols, CV_8UC3); //画像格納用のオブジェクト宣言  
	// imag  輪郭近似データ用
	// gray グレースケールデータ用
	// binary　二値化データ用
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
	
	contourbinary= imagA;


	//
	//グレースケール
	
	//gray = cv::imread("img8.jpg",0); //画像データの読み込み グレースケール
	cv::cvtColor(imagA , gray ,cv::COLOR_RGB2GRAY);

	if (gray.empty()) { //エラー処理
		std::cout << "error" << std::endl;
		return 0;
	}
	//cv::imwrite("gray.jpg", gray); //グレースケール画像の保存
	
	//
	//二値化
        
	//(2)
	//std::cout << "閾値を入力してください" << std::endl;
	//
	//int tsd_v;
	//std::cin >> tsd_v;
        //
	
        
	//二値化処理
	//(1)
	//cv::adaptiveThreshold(gray,binary,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY,7,0); 
	// //cv::ADAPTIVE_THRESH_MEAN_C  違う手法画像処理
	cv::threshold(gray,binary, 0, 255,cv::THRESH_BINARY | cv::THRESH_OTSU);
	//
	//(2)
	//cv::threshold(gray,binary,tsd_v,255,cv::THRESH_BINARY); //二値化
        //
	
	////////////////////////////画像データ保存
        //cv::imwrite("binary.jpg", binary);      //二値化画像の保存
	// cv::imwrite("binary1.jpg", binary_1);      //二値化画像の保存
        //
	
	
	//輪郭抽出

	binarycontour = binary; //コピー
        std::vector< std::vector< cv::Point > > contours ; //輪郭の座標リスト
        std::vector<cv::Vec4i> lines; //階層　オプションの出力ベクター
        cv::findContours(binarycontour,contours,lines,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);//輪郭抽出


	////////////////////////////画像データ保存      
        //輪郭のデータをカラー画像に上書き
        //for (auto contour=contours.begin() ; contour!=contours.end() ; contour++){
        //      cv::polylines(contourbinary,*contour,true , cv::Scalar(0,200,0),1);
        //}
        //
        //cv::imwrite("contourbinary.jpg", contourbinary); //輪郭抽出画像を保存
        // 
        //imag=cv::imread("imag8.jpg");
        for ( auto contour=contours.begin() ;contour!=contours.end(); contour++){

                std::vector<cv::Point > approx;

                //輪郭を直線近似する

                cv::approxPolyDP(cv::Mat(*contour),approx,0.001*cv::arcLength(*contour,true),true);
             /////////////////////////////////////////////////////////////////////////////////////////
             std::vector<cv::Point> approxcurve_1;//直線近似データ保存
                cv::approxPolyDP(cv::Mat(*contour),approxcurve_1,0.05*cv::arcLength(*contour,true),true);
                double area_1 = cv::contourArea(approxcurve_1);//面積を保存
                if(approxcurve_1.size()==4 && area_1 > 1000 && cv::isContourConvex(approxcurve_1)){
             //  
             //cv::polylines(imag,approx,true , cv::Scalar(0,0,200),2);
                //cv::polylines(imagA,approx,true , cv::Scalar(0,0,200),1);
                }

        }

        ////////////////////////////画像データ保存      
        //cv::imwrite("contourapprox.jpg",imag); 
        //cv::imwrite("contourapprox_1.jpg",imagA);


        //
	//グーロバル変数初期化
	count=0;
        for(int i=0; i<14 ; i++){
        id_nunber[i].id = 333;
        }


	//
	//四角形の座標と透視変換
 
	for ( auto contour=contours.begin();contour!=contours.end(); contour++){
		//輪郭を直線近似する
		std::ostringstream oss;//正方形データ
		std::vector<cv::Point> approxcurve;//直線近似データ保存
		cv::approxPolyDP(cv::Mat(*contour),approxcurve,0.05*cv::arcLength(*contour,true),true);
		double area = cv::contourArea(approxcurve);//面積を保存
		if(approxcurve.size()==4 /*&& area > 1000 && cv::isContourConvex(approxcurve)*/){ //頂点が４つの場合と面積が1000以上の場合　//凸型の場合
		
			cv::Point2f srcPoint[]={approxcurve[0], approxcurve[1], approxcurve[2], approxcurve[3]};//４点データをコピー
			cv::Mat map = cv::getPerspectiveTransform(srcPoint,dstPoint);//原点に寄せる
			cv::warpPerspective(binary , mapImg , map, srcMarker.size());//透視変換
			//cv::Mat candidateImg = mapImg.clone();
			//cv::cvtColor(mapImg,mapImg,cv::COLOR_BGR2GRAY);
			//cv::threshold(mapImg,mapImg,250/*tsd_v*/,255,cv::THRESH_BINARY);//二値化を行う
			////decodeMarker(mapImg)でマーカのIDと向きを判別
		        decodeMarker(mapImg);
			//マーカーでないと判断したらタグIDを０とする
			if(id_nunber[count].id!=333){
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

			//3次元のときのデータを計算に使用	
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

			//回転ベクトルと並進ベクトルを取得
			cv::solvePnP(object_points,image_points,intrinsic,distortion,rotation,translation);

			//マーカーのカメラからの回転ベクトルと並進ベクトルを保存
                        id_nunber[count].id_rotation[0].x = rotation.at<float>(0,0);
                        id_nunber[count].id_rotation[0].y = rotation.at<float>(1,0);
                        id_nunber[count].id_rotation[0].z = rotation.at<float>(2,0);
                        id_nunber[count].id_translation[0].x = translation.at<float>(0,0);
                        id_nunber[count].id_translation[0].y = translation.at<float>(1,0);
                        id_nunber[count].id_translation[0].z = translation.at<float>(2,0);


                        //回転ベクトルと並進ベクトルからタグの３次元変換
			cv::projectPoints(srcPoints3D,rotation,translation,intrinsic,distortion,dstPoints2D);
			//std::cout <<"2次元座標="<< dstPoints2D << std::endl;
			cv::Point2f dstPoints2DD[4];
 			dstPoints2DD[0] = dstPoints2D.at<cv::Vec2f>(0,0);
                        dstPoints2DD[1] = dstPoints2D.at<cv::Vec2f>(1,0);
                        dstPoints2DD[2] = dstPoints2D.at<cv::Vec2f>(2,0);
                        dstPoints2DD[3] = dstPoints2D.at<cv::Vec2f>(3,0);

			//タグのIDからMPSの長辺の頂点４点の位置を画像の２次元に変換
                        
			//MPSのinput側の
			if(id_nunber[i].id==0 || id_nunber[i].id==10 || id_nunber[i].id==20 || id_nunber[i].id==30 || id_nunber[i].id==40 || id_nunber[i].id==50 || id_nunber[i].id==60 || id_nunber[i].id==100 || id_nunber[i].id==110 || id_nunber[i].id==120 || id_nunber[i].id==130 ||id_nunber[i].id==140 || id_nunber[i].id==150 || id_nunber[i].id==160){
      
	cv::projectPoints(AsrcPoints3D,rotation,translation,intrinsic,distortion,BdstPoints2D);
			} //ASrc3D 3次元からBdst 2次元
                        //MPSのoutput側
			if(id_nunber[i].id==1 || id_nunber[i].id==11 || id_nunber[i].id==21 || id_nunber[i].id==31 || id_nunber[i].id==41 || id_nunber[i].id==51 || id_nunber[i].id==61 || id_nunber[i].id==101 || id_nunber[i].id==111 || id_nunber[i].id==121 || id_nunber[i].id==131 || id_nunber[i].id==141 || id_nunber[i].id==151 || id_nunber[i].id==161 ){
              
                        cv::projectPoints(BsrcPoints3D,rotation,translation,intrinsic,distortion,BdstPoints2D);
                        }
                        
                        //保存
			id_nunber[count].MPS_Pint[0] = BdstPoints2D.at<cv::Vec2f>(0,0);
                        id_nunber[count].MPS_Pint[1] = BdstPoints2D.at<cv::Vec2f>(1,0);
                        id_nunber[count].MPS_Pint[2] = BdstPoints2D.at<cv::Vec2f>(2,0);
                        id_nunber[count].MPS_Pint[3] = BdstPoints2D.at<cv::Vec2f>(3,0);
                 

        

			///////////////////////////////画像にタグ名前と３次元を出力////////////////

                        ////valueでID＝認識したIDで保存
                        //sprintf(value,"ID=%d",id_nunber[count].id);
                        //// (0,225,0));//緑  (0,0,225));//赤 (225,0,0));//青
                        //cv::polylines(imagA, approxcurve, true, cv::Scalar(0 ,255, 0), 2);
                        //cv::circle(imagA, id_nunber[count].id_dstPint[0], 10, cv::Scalar(255, 0, 0));
                        //cv::circle(imagA, id_nunber[count].id_dstPint[1], 10, cv::Scalar(0, 255, 0));
                        //cv::circle(imagA, id_nunber[count].id_dstPint[2], 10, cv::Scalar(0, 0, 255));
                        //cv::circle(imagA, id_nunber[count].id_dstPint[3], 10, cv::Scalar(255,255,0));
                        //cv::line(imagA,dstPoints2DD[0],dstPoints2DD[1],cv::Scalar(255, 0, 0),3);
                        //cv::line(imagA,dstPoints2DD[0],dstPoints2DD[2],cv::Scalar(0,255, 0),3);
                        //cv::line(imagA,dstPoints2DD[0],dstPoints2DD[3],cv::Scalar(0, 0, 255),3);
                        //cv::putText(imagA,value,cv::Point((dstPoints2DD[0].x+dstPoints2DD[2].x)/2,(dstPoints2DD[0].y+dstPoints2DD[1].y)/2),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,255),1.5);
                        ///////////////////////////////////////////////////////////////////

			//保存　タグ
                        oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
			cv::imwrite("mapImg"+ oss.str() +  ".jpg",mapImg);
                       
			count++;
			}
			//oss << std::setfill( '0' ) << std::setw( 3 ) << i++;
                        //cv::imwrite("mapImg"+ oss.str() +  ".jpg",mapImg);


		}
			


	}
       


       //カメラ座標系に変換
        for(int i=0; i<14 ; i++){
                if(id_nunber[i].id!=333){
                   //確認
                   //std::cout << "カメラID=" << id_nunber[i].id << std::endl;
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

		   //std::cout << "回並１=" << rt_points << std::endl;

                   cv::Mat matA(4,4,CV_64F,id_nunber[i].a);
		   //std::cout << "回並2=" << matA << std::endl;

                   rotation_points = rotation_points.t(); //rotation of inverse
                   id_translation = -rotation_points  * id_translation;

                   cv::Mat T = cv::Mat::eye(4, 4, rotation_points.type()); // T is 4x4
                   T( cv::Range(0,3), cv::Range(0,3) ) = rotation_points * 1; // copies R into T
                   T( cv::Range(0,3), cv::Range(3,4) ) = id_translation * 1; // copies tvec into T
                   
		   //std::cout << "逆1=" << T << std::endl;

                   id_nunber[i].f[0][0]=T.at<double>(0,0);
		   id_nunber[i].f[1][0]=T.at<double>(1,0);
		   id_nunber[i].f[2][0]=T.at<double>(2,0);
		   id_nunber[i].f[3][0]=T.at<double>(3,0);
		   id_nunber[i].f[0][1]=T.at<double>(0,1);
		   id_nunber[i].f[1][1]=T.at<double>(1,1);
		   id_nunber[i].f[2][1]=T.at<double>(2,1);
		   id_nunber[i].f[3][1]=T.at<double>(3,1);
		   id_nunber[i].f[0][2]=T.at<double>(0,2);
		   id_nunber[i].f[1][2]=T.at<double>(1,2);
		   id_nunber[i].f[2][2]=T.at<double>(2,2);
		   id_nunber[i].f[3][2]=T.at<double>(3,2);
		   id_nunber[i].f[0][3]=T.at<double>(0,3);
		   id_nunber[i].f[1][3]=T.at<double>(1,3);
		   id_nunber[i].f[2][3]=T.at<double>(2,3);
		   id_nunber[i].f[3][3]=T.at<double>(3,3);

		   //std::cout << "逆=" << id_nunber[i].f[0][0] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[1][0] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[2][0] << std::endl;
                   //std::cout << "逆=" << id_nunber[i].f[3][0] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[0][1] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[1][1] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[2][1] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[3][1] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[0][2] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[1][2] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[2][2] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[3][2] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[0][3] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[1][3] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[2][3] << std::endl;
		   //std::cout << "逆=" << id_nunber[i].f[3][3] << std::endl;
                   
                   
                   //確認
                   //std::cout << "=" << rt_points <<std::endl;
                   //std::cout << "回転ー並進行列=" << matA <<std::endl;

                   //MPSの中心の位置で座標変換
		   //タグ　　　手前がz  input  y 9.0 z -17.5
		   //　→　y　           output y 4.0 z -17.5
		   //↓ 
		   //x
                   double ac[4]={0.0,9.0,-17.5,1.0};//input
                   double ad[4]={0.0,4.0,-17.5,1.0};//output

                   cv::Mat matB(4,1,CV_64F,ac);//input行列式
                   cv::Mat matC(4,1,CV_64F,ad);//output行列式
                   cv::Mat matX(4,1,CV_64F);//カメラ原点座標系でのタグ原点の座標
                   cv::Mat matY(4,1,CV_64F);//タグ原点座標系でのカメラ原点の座標
                   
                   //MPSのinput側の
		   if(id_nunber[i].id==0 || id_nunber[i].id==10 || id_nunber[i].id==20 || id_nunber[i].id==30 || id_nunber[i].id==40 || id_nunber[i].id==50 || id_nunber[i].id==60 || id_nunber[i].id==100 || id_nunber[i].id==110 || id_nunber[i].id==120 || id_nunber[i].id==130 ||id_nunber[i].id==140 || id_nunber[i].id==150 || id_nunber[i].id==160){
                   matX=matA*matB;
		   //std::cout << "matB=" << matB <<std::endl;

		   }
                   
		   //MPSのoutput側
                   if(id_nunber[i].id==1 || id_nunber[i].id==11 || id_nunber[i].id==21 || id_nunber[i].id==31 || id_nunber[i].id==41 || id_nunber[i].id==51 || id_nunber[i].id==61 || id_nunber[i].id==101 || id_nunber[i].id==111 || id_nunber[i].id==121 || id_nunber[i].id==131 || id_nunber[i].id==141 || id_nunber[i].id==151 || id_nunber[i].id==161 ){
                   matX=matA*matC;
		   //std::cout << "matC=" << matC <<std::endl;

                   }

		   //std::cout << "matA=" << matA <<std::endl;
                   //std::cout << "ID=" << id_nunber[i].id << std::endl;
                   //std::cout << "カメラ原点座標でのMPSの中心座標=" << matX << std::endl;
                   matY =T*matX;
                   //std::cout << "タグ原点座標系でのカメラ原点の座標=" << matY << std::endl;
                   //確認 タグ原点座標系でのカメラ原点の座標をカメラ座標系に戻すと０となる　
                   //matY = matA*matY;
                   //std::cout << "タグ原点座標系でのカメラ原点の座標をカメラ座標系に戻す=" << matY << std::endl;
                   //保存
                   id_nunber[i].b[0] = matX.at<double>(0,0);
                   id_nunber[i].b[1] = matX.at<double>(1,0);
                   id_nunber[i].b[2] = matX.at<double>(2,0);
                   id_nunber[i].b[3] = matX.at<double>(3,0);

                   //std::cout << "カメラ座標からMPSの中心ｘ=" << id_nunber[i].b[0] << std::endl;
                   //std::cout << "カメラ座標からMPSの中心y=" << id_nunber[i].b[1] << std::endl;
                   //std::cout << "カメラ座標からMPSの中心ｚ=" << id_nunber[i].b[2] << std::endl;
                   
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

       //
       //refboxとMPSの位置を合わせる　そして　回転させてrefboxの位置と合うかどうか確かめる
       for(int i=0; i<14 ; i++){
                if(id_nunber[i].id!=333){
			cv::Mat matA(4,4,CV_64F,id_nunber[i].f);//　基準MPSの回転と平行行列
			//std::cout << "逆2=" << matA << std::endl;
			std::cout << "基準ID=" << id_nunber[i].id << std::endl;
			
			for(int k=0; k<14; k++){
				if(id_nunber[k].id!=333){
					cv::Mat matX(4,1,CV_64F,id_nunber[k].b);
					cv::Mat matF(4,1,CV_64F);
					//std::cout << "MPS=" << matX << std::endl;
					matF = matA*matX;
                                        std::cout << "ID=" << id_nunber[k].id << std::endl;
					std::cout << "MPS=" << matF << std::endl;
					matX.release();
					matF.release();
				}
			}

                        matA.release();

		}
       }

       //	
       //判定結果画像に合成
       for(int i=0; i<14 ; i++){
              
	       float a,b,c,d,e,f,g,h;
               a=id_nunber[i].MPS_Pint[0].x;
               b=id_nunber[i].MPS_Pint[0].y;
               c=id_nunber[i].MPS_Pint[1].x;
               d=id_nunber[i].MPS_Pint[1].y;
               e=id_nunber[i].MPS_Pint[2].x;
               f=id_nunber[i].MPS_Pint[2].y;
               g=id_nunber[i].MPS_Pint[3].x;
               h=id_nunber[i].MPS_Pint[3].y;
        
               std::vector<cv::Point> pts;
               for(int i=0 ;i<4;i++)
                {
                switch(i)
                   {
                    case 0: pts.push_back(cv::Point(a,b));
                    break;
		    case 1: pts.push_back(cv::Point(c,d));
                    break;
                    case 2: pts.push_back(cv::Point(e,f));
                    break;
                    case 3: pts.push_back(cv::Point(g,h));
                    break;
                   }
               }
         
	       if(id_nunber[i].id==111){
                 cv::fillConvexPoly(imagA,pts,cv::Scalar(0,225,0));//緑
               }
               if(id_nunber[i].id==20){
                 cv::fillConvexPoly(imagA,pts,cv::Scalar(0,0,225));//赤
               }
               if(id_nunber[i].id==60){
                 cv::fillConvexPoly(imagA,pts,cv::Scalar(0,225,0));
               }
               if(id_nunber[i].id==0){
                cv::fillConvexPoly(imagA,pts,cv::Scalar(0,225,225));//黄色
               } if(id_nunber[i].id==131 || id_nunber[i].id==151 || id_nunber[i].id==1 || id_nunber[i].id==121){
                 cv::fillConvexPoly(imagA,pts,cv::Scalar(0,225,0));
               }
        }





      //文字表示
	cv::rectangle(imagA,cv::Point(345+500,2324.48),cv::Point(495.00+500,2474.48),cv::Scalar(0,225,0),-1);
                        sprintf(value,"location:OK orientation:OK");
                        cv::putText(imagA,value,cv::Point(125.0+600+500,1624.48+800),cv::FONT_HERSHEY_SIMPLEX,6,cv::Scalar(0,0,0),15);
                        cv::rectangle(imagA,cv::Point(125+220+500,1624.48+1100),cv::Point(125+370+500,1624.48+1250),cv::Scalar(0,0,225),-1);
                        sprintf(value,"location:NG");
                        cv::putText(imagA,value,cv::Point(125+600+500,1624.48+1200),cv::FONT_HERSHEY_SIMPLEX,6,cv::Scalar(0,0,0),15);
                        cv::rectangle(imagA,cv::Point(125+220+500,1624.48+900),cv::Point(125+370+500,1624.48+1050),cv::Scalar(0,225,225),-1);
                        sprintf(value,"location:OK orientation:NG");
                        cv::putText(imagA,value,cv::Point(125+600+500,1624.48+1000),cv::FONT_HERSHEY_SIMPLEX,6,cv::Scalar(0,0,0),15);

	
//        std::cout<<"m1="<< mapImg  << std::endl;
//	for(int y = 0; y < mapImg.rows; y++){
//	for(int x = 0; x < mapImg.cols; x++){
//			std::cout << (int)mapImg.at<unsigned char>(x,y) << std::endl;
//	}
//}

        cv::Mat imagB;
        cv::resize(imagA,imagB,cv::Size(),0.2,0.2);
                
	cv::namedWindow("Components",1);
	cv::imshow("Components",imagB);
	cv::waitKey(0);
        
        cv::imwrite("imagA.jpg",imagA);	

	//開放
        gray.release();
        binary.release();
        binary_1.release();
        binarycontour.release();
        contourbinary.release();
        imag.release();
        imagA.release();
        imagB.release();
        mapImg.release();
        srcMarker.release();

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
        frame.release();

	cv::destroyAllWindows();

	return 0;
	}


int decodeMarker(cv::Mat src)
{
	
	if(src.channels()!=1)
	{ //チャンネルが１つであること
		id_nunber[count].id=333;
		return -1;
	}

	if(src.cols!=src.rows)
	{//縦と横が同じサイズであること
		id_nunber[count].id=333;
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

	int cellRow=9;
	int cellCol=9;
	int result[9][9];
	int cellsize=10;  //pixcel size of cell



	///////////////////////////////////////
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
        //std::ostringstream oss_1;//正方形データ
        //oss_1 << std::setfill( '0' ) << std::setw( 3 ) << count_1++;
        //cv::imwrite("mapImg_1"+ oss_1.str() +  ".jpg",src);

        src_1.release();

	/////////////////////////////////////////////////
        
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
		id_nunber[count].id=333;
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
			id_nunber[count].id=30;
			id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 178 C-RS2 Output
                        id_nunber[count].id=31;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
		{
			//ID 65 C-BS Input
			id_nunber[count].id=40;
			id_nunber[count].markerDirection=Direction*90;
		}else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 66 C-BS Output
                        id_nunber[count].id=41;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 81 C-DS Input 
                        id_nunber[count].id=50;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 82 C-DS Output
                        id_nunber[count].id=51;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 1 C-CS1 Input
                        id_nunber[count].id=0;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 2 C-CS1 Output
                        id_nunber[count].id=1;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 17 C-CS2 Input
                        id_nunber[count].id=10;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 18 C-CS2 Output
                        id_nunber[count].id=11;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 33 C-RS1 Input 
                        id_nunber[count].id=20;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 34 C-RS1 Output 
                        id_nunber[count].id=21;
                        id_nunber[count].markerDirection=Direction*90;
       
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 193 C-SS Input 
                        id_nunber[count].id=60;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 194 C-SS Output 
                        id_nunber[count].id=61;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 97 M-CS1 Input 
                        id_nunber[count].id=100;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 98 M-CS1 Output 
                        id_nunber[count].id=101;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==1 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 113 M-CS2 Input 
                        id_nunber[count].id=110;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==1 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==0 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 114 M-CS2 Output 
                        id_nunber[count].id=111;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 129 M-RS1 Input 
                        id_nunber[count].id=120;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==0 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 130 M-RS1 Output 
                        id_nunber[count].id=121;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 145 M-RS2 Input 
                        id_nunber[count].id=130;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==1 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 146 M-RS2 Output 
                        id_nunber[count].id=131;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 161 M-BS Input 
                        id_nunber[count].id=140;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==0 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==1 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 162 M-BS Output 
                        id_nunber[count].id=141;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 49 M-DS Input 
                        id_nunber[count].id=150;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==1 && src_result[0][4]==1 &&
                         src_result[1][0]==1 && src_result[1][1]==0 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==0 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 50 M-DS Output 
                        id_nunber[count].id=151;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==0 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==1 && src_result[4][2]==1 && src_result[4][3]==0 && src_result[4][4]==1)
                {
                        //ID 209 M-SS Input 
                        id_nunber[count].id=160;
                        id_nunber[count].markerDirection=Direction*90;
                }else if(src_result[0][0]==0 && src_result[0][1]==1 && src_result[0][2]==0 && src_result[0][3]==0 && src_result[0][4]==1 &&
                         src_result[1][0]==0 && src_result[1][1]==1 && src_result[1][2]==0 && src_result[1][3]==0 && src_result[1][4]==1 &&
                         src_result[2][0]==1 && src_result[2][1]==0 && src_result[2][2]==1 && src_result[2][3]==0 && src_result[2][4]==1 &&
                         src_result[3][0]==1 && src_result[3][1]==0 && src_result[3][2]==1 && src_result[3][3]==1 && src_result[3][4]==0 &&
                         src_result[4][0]==1 && src_result[4][1]==0 && src_result[4][2]==1 && src_result[4][3]==1 && src_result[4][4]==0)
                {
                        //ID 210 M-SS Output 
                        id_nunber[count].id=161;
                        id_nunber[count].markerDirection=Direction*90;
                }









                
                if(id_nunber[count].id!=333){
			break;
		}

		
	}


	return 0;

}
