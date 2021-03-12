#include <ros/ros.h>
#include <time.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs/Joy.h>
#include <string>

#include <iostream>
#include <vector>
#include <stdio.h>


struct getDateCamera
{
	double Rob1ToCamera[3]; //ロボット１から見たカメラの位置
	double CameraToRob1[3]; //カメラから見たロボット１の位置
	double Rob2ToCamera[3]; //ロボット２から見たカメラの位置
        double CameraToRob2[3]; //カメラから見たロボット３の位置
	double Rob3ToCamera[3]; //ロボット２から見たカメラの位置
        double CameraToRob3[3]; //カメラから見たロボット３の位置
};

struct getDateCamera robotino1[1];
struct getDateCamera robotino2[1];
struct getDateCamera robotino3[1];

struct getDateJoy
{
        int Rob1button[1]; //
        int Rob2button[1]; //
        int Rob3button[1]; //
	float xbutton[1];
	float ybutton[1];
        float zbutton[1];
	
};

struct getDateJoy Buttons[1];



class SubPub{
	private:
		ros::NodeHandle nh;
                ros::Publisher robotino_1_pub;
		ros::Publisher robotino_2_pub;
                ros::Publisher robotino_3_pub;
		ros::Subscriber joy_sub;
                ros::Subscriber robotino_1_camra_sub;
		ros::Subscriber robotino_2_camra_sub;
		ros::Subscriber robotino_3_camra_sub;
	public:
		SubPub(){
			robotino_1_pub  = nh.advertise<std_msgs::Float32MultiArray>("robotino1/velocity",1000);
                        robotino_2_pub  = nh.advertise<std_msgs::Float32MultiArray>("robotino2/velocity",1000);
                        robotino_3_pub  = nh.advertise<std_msgs::Float32MultiArray>("robotino3/velocity",1000);
                        joy_sub = nh.subscribe("joy", 1000, &SubPub::Joy_Callback,this);
                        robotino_1_camra_sub = nh.subscribe("ca_1", 1000, &SubPub::Robo1Callback,this);
			robotino_2_camra_sub = nh.subscribe("ca_2", 1000, &SubPub::Robo2Callback,this);
			robotino_3_camra_sub = nh.subscribe("ca_3", 1000, &SubPub::Robo3Callback,this);
		}
                void Robo1Callback(const std_msgs::Float64MultiArray& msg);
		void Robo2Callback(const std_msgs::Float64MultiArray& msg);
		void Robo3Callback(const std_msgs::Float64MultiArray& msg);
                void Joy_Callback(const sensor_msgs::Joy& joy_msg);
		void Publication(void);
};

void SubPub::Robo1Callback(const std_msgs::Float64MultiArray& msg)
{
  //int num = msg.data.size();
  int count1 =0;
  int count2 =0;
  int count3 =0;
  //ROS_INFO("I susclibed [%i]", num);
  for (int i = 0; i < 2; i++)
  {
	  if(msg.data[i*7] == 1.0){
		  robotino1[0].Rob1ToCamera[0] = msg.data[i*7+1]; 
                  robotino1[0].Rob1ToCamera[1] = msg.data[i*7+2];
		  robotino1[0].Rob1ToCamera[2] = msg.data[i*7+3];

		  robotino1[0].CameraToRob1[0] = msg.data[i*7+4];
		  robotino1[0].CameraToRob1[1] = msg.data[i*7+5];
		  robotino1[0].CameraToRob1[2] = msg.data[i*7+6];
		  count1++;
		  //ROS_INFO("Robotino1-> Robotino1 ");
	  }
	  if(msg.data[i*7] == 2.0){
                  robotino1[0].Rob2ToCamera[0] = msg.data[i*7+1];
                  robotino1[0].Rob2ToCamera[1] = msg.data[i*7+2];
                  robotino1[0].Rob2ToCamera[2] = msg.data[i*7+3];

                  robotino1[0].CameraToRob2[0] = msg.data[i*7+4];
                  robotino1[0].CameraToRob2[1] = msg.data[i*7+5];
                  robotino1[0].CameraToRob2[2] = msg.data[i*7+6];
		  count2++;
		  //ROS_INFO("Robotino1-> Robotino2 ");
          }
	  if(msg.data[i*7] == 3.0){
                  robotino1[0].Rob3ToCamera[0] = msg.data[i*7+1];
                  robotino1[0].Rob3ToCamera[1] = msg.data[i*7+2];
                  robotino1[0].Rob3ToCamera[2] = msg.data[i*7+3];

                  robotino1[0].CameraToRob3[0] = msg.data[i*7+4];
                  robotino1[0].CameraToRob3[1] = msg.data[i*7+5];
                  robotino1[0].CameraToRob3[2] = msg.data[i*7+6];
		  count3++;
		  //ROS_INFO("Robotino1-> Robotino3 ");
          }

	 
    //ROS_INFO("[%i]:%f", i, msg.data[i]);
  }
  if(count1 == 0){
         robotino1[0].Rob1ToCamera[0] = 0.0;
         robotino1[0].Rob1ToCamera[1] = 0.0;
         robotino1[0].Rob1ToCamera[2] = 0.0;

         robotino1[0].CameraToRob1[0] = 0.0;
         robotino1[0].CameraToRob1[1] = 0.0;
         robotino1[0].CameraToRob1[2] = 0.0;
  }

  if(count2 == 0){
         robotino1[0].Rob2ToCamera[0] = 0.0;
         robotino1[0].Rob2ToCamera[1] = 0.0;
         robotino1[0].Rob2ToCamera[2] = 0.0;

         robotino1[0].CameraToRob2[0] = 0.0;
         robotino1[0].CameraToRob2[1] = 0.0;
         robotino1[0].CameraToRob2[2] = 0.0;
  }

  if(count3 == 0){
         robotino1[0].Rob3ToCamera[0] = 0.0;
         robotino1[0].Rob3ToCamera[1] = 0.0;
         robotino1[0].Rob3ToCamera[2] = 0.0;

         robotino1[0].CameraToRob3[0] = 0.0;
         robotino1[0].CameraToRob3[1] = 0.0;
         robotino1[0].CameraToRob3[2] = 0.0;
  }

}

void SubPub::Robo2Callback(const std_msgs::Float64MultiArray& msg)
{
  //int num = msg.data.size();
  //ROS_INFO("I susclibed [%i]", num);
  int count1 =0;
  int count2 =0;
  int count3 =0;
  
  for (int i = 0; i < 2; i++)
  {
          if(msg.data[i*7] == 1.0){
                  robotino2[0].Rob1ToCamera[0] = msg.data[i*7+1];
                  robotino2[0].Rob1ToCamera[1] = msg.data[i*7+2];
                  robotino2[0].Rob1ToCamera[2] = msg.data[i*7+3];

                  robotino2[0].CameraToRob1[0] = msg.data[i*7+4];
                  robotino2[0].CameraToRob1[1] = msg.data[i*7+5];
                  robotino2[0].CameraToRob1[2] = msg.data[i*7+6];
		  count1++;
		  //ROS_INFO("Robotino2-> Robotino1 ");
          }
          if(msg.data[i*7] == 2.0){
                  robotino2[0].Rob2ToCamera[0] = msg.data[i*7+1];
                  robotino2[0].Rob2ToCamera[1] = msg.data[i*7+2];
                  robotino2[0].Rob2ToCamera[2] = msg.data[i*7+3];

                  robotino2[0].CameraToRob2[0] = msg.data[i*7+4];
                  robotino2[0].CameraToRob2[1] = msg.data[i*7+5];
                  robotino2[0].CameraToRob2[2] = msg.data[i*7+6];
		  count2++;
		  //ROS_INFO("Robotino2-> Robotino2 ");
          }
          if(msg.data[i*7] == 3.0){
                  robotino2[0].Rob3ToCamera[0] = msg.data[i*7+1];
                  robotino2[0].Rob3ToCamera[1] = msg.data[i*7+2];
                  robotino2[0].Rob3ToCamera[2] = msg.data[i*7+3];

                  robotino2[0].CameraToRob3[0] = msg.data[i*7+4];
                  robotino2[0].CameraToRob3[1] = msg.data[i*7+5];
                  robotino2[0].CameraToRob3[2] = msg.data[i*7+6];
		  count3++;
		  //ROS_INFO("Robotino2-> Robotino3 ");
          }
  }
  //ROS_INFO("[%i]:%f", i, msg.data[i]);
  if(count1 == 0){
         robotino2[0].Rob1ToCamera[0] = 0.0;
         robotino2[0].Rob1ToCamera[1] = 0.0;
         robotino2[0].Rob1ToCamera[2] = 0.0;

         robotino2[0].CameraToRob1[0] = 0.0;
         robotino2[0].CameraToRob1[1] = 0.0;
         robotino2[0].CameraToRob1[2] = 0.0;
  }

  if(count2 == 0){
         robotino2[0].Rob2ToCamera[0] = 0.0;
         robotino2[0].Rob2ToCamera[1] = 0.0;
         robotino2[0].Rob2ToCamera[2] = 0.0;

         robotino2[0].CameraToRob2[0] = 0.0;
         robotino2[0].CameraToRob2[1] = 0.0;
         robotino2[0].CameraToRob2[2] = 0.0;
  }

  if(count3 == 0){
         robotino2[0].Rob3ToCamera[0] = 0.0;
         robotino2[0].Rob3ToCamera[1] = 0.0;
         robotino2[0].Rob3ToCamera[2] = 0.0;

         robotino2[0].CameraToRob3[0] = 0.0;
         robotino2[0].CameraToRob3[1] = 0.0;
         robotino2[0].CameraToRob3[2] = 0.0;
  }


}

void SubPub::Robo3Callback(const std_msgs::Float64MultiArray& msg)
{
  //int num = msg.data.size();
  //ROS_INFO("I susclibed [%i]", num);
  int count1 =0;
  int count2 =0;
  int count3 =0;
  
  for (int i = 0; i < 2; i++)
  {
          if(msg.data[i*7] == 1.0){
                  robotino3[0].Rob1ToCamera[0] = msg.data[i*7+1];
                  robotino3[0].Rob1ToCamera[1] = msg.data[i*7+2];
                  robotino3[0].Rob1ToCamera[2] = msg.data[i*7+3];

                  robotino3[0].CameraToRob1[0] = msg.data[i*7+4];
                  robotino3[0].CameraToRob1[1] = msg.data[i*7+5];
                  robotino3[0].CameraToRob1[2] = msg.data[i*7+6];
		  count1++;
		  //ROS_INFO("Robotino3-> Robotino1 ");
          }
          if(msg.data[i*7] == 2.0){
                  robotino3[0].Rob2ToCamera[0] = msg.data[i*7+1];
                  robotino3[0].Rob2ToCamera[1] = msg.data[i*7+2];
                  robotino3[0].Rob2ToCamera[2] = msg.data[i*7+3];

                  robotino3[0].CameraToRob2[0] = msg.data[i*7+4];
                  robotino3[0].CameraToRob2[1] = msg.data[i*7+5];
                  robotino3[0].CameraToRob2[2] = msg.data[i*7+6];
		  count2++;
		  //ROS_INFO("Robotino3-> Robotino2 ");
          }
          if(msg.data[i*7] == 3.0){
                  robotino3[0].Rob3ToCamera[0] = msg.data[i*7+1];
                  robotino3[0].Rob3ToCamera[1] = msg.data[i*7+2];
                  robotino3[0].Rob3ToCamera[2] = msg.data[i*7+3];

                  robotino3[0].CameraToRob3[0] = msg.data[i*7+4];
                  robotino3[0].CameraToRob3[1] = msg.data[i*7+5];
                  robotino3[0].CameraToRob3[2] = msg.data[i*7+6];
		  count3++;
		  //ROS_INFO("Robotino3-> Robotino3 ");
          }
    //ROS_INFO("[%i]:%f", i, msg.data[i]);
  }
  if(count1 == 0){
         robotino3[0].Rob1ToCamera[0] = 0.0;
         robotino3[0].Rob1ToCamera[1] = 0.0;
         robotino3[0].Rob1ToCamera[2] = 0.0;

         robotino3[0].CameraToRob1[0] = 0.0;
         robotino3[0].CameraToRob1[1] = 0.0;
         robotino3[0].CameraToRob1[2] = 0.0;
  }

  if(count2 == 0){
         robotino3[0].Rob2ToCamera[0] = 0.0;
         robotino3[0].Rob2ToCamera[1] = 0.0;
         robotino3[0].Rob2ToCamera[2] = 0.0;

         robotino3[0].CameraToRob2[0] = 0.0;
         robotino3[0].CameraToRob2[1] = 0.0;
         robotino3[0].CameraToRob2[2] = 0.0;
  }

  if(count3 == 0){
         robotino3[0].Rob3ToCamera[0] = 0.0;
         robotino3[0].Rob3ToCamera[1] = 0.0;
         robotino3[0].Rob3ToCamera[2] = 0.0;

         robotino3[0].CameraToRob3[0] = 0.0;
         robotino3[0].CameraToRob3[1] = 0.0;
         robotino3[0].CameraToRob3[2] = 0.0;
  }
}

void SubPub::Joy_Callback(const sensor_msgs::Joy& joy_msg)
{
        Buttons[0].Rob1button[0] = joy_msg.buttons[4];
	Buttons[0].Rob2button[0] = joy_msg.buttons[5];
	Buttons[0].Rob3button[0] = joy_msg.buttons[3];

	Buttons[0].xbutton[0] = joy_msg.axes[1];
        Buttons[0].ybutton[0] = joy_msg.axes[0];
        Buttons[0].zbutton[0] = joy_msg.axes[3];
        
}


void SubPub::Publication(void){

  std_msgs::Float32MultiArray velocity;
  std_msgs::Float32MultiArray robo1velocity;
  std_msgs::Float32MultiArray robo2velocity;
  std_msgs::Float32MultiArray robo3velocity;

  velocity.data.resize(3);
  robo1velocity.data.resize(3);
  robo2velocity.data.resize(3);
  robo3velocity.data.resize(3);

  velocity.data[0] = Buttons[0].xbutton[0]*50;
  velocity.data[1] = Buttons[0].ybutton[0]*50;
  velocity.data[2] = Buttons[0].zbutton[0]*50;


  //robotino1 move
        if(Buttons[0].Rob1button[0] == 1 && Buttons[0].Rob2button[0] == 0 && Buttons[0].Rob3button[0] == 0){
		//robotino1
		robotino_1_pub.publish(velocity);

                //robotino2
                if(robotino2[0].CameraToRob1[2] == 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 10; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
		if(robotino2[0].CameraToRob1[0] <= -100 && robotino2[0].CameraToRob1[2]!= 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 9; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
		if(robotino2[0].CameraToRob1[0] >= 100 && robotino2[0].CameraToRob1[2]!= 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = -9; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
	        if(robotino2[0].CameraToRob1[0] > -100 && robotino2[0].CameraToRob1[0] < 100 && robotino2[0].CameraToRob1[2]!= 0.0){
                        //ROS_INFO("robotino2_4");
                        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
                        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 0.0 ; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
		}



                //robotino3
		if(robotino3[0].CameraToRob1[2] == 0.0){
			//ROS_INFO("robotino3_1");
			//ROS_INFO("robotino3[0].CameraToRob1[0]:%f",robotino3[0].CameraToRob1[0]);
                        //ROS_INFO("robotino3[0].CameraToRob1[2]:%f",robotino3[0].CameraToRob1[2]);
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 10; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if(robotino3[0].CameraToRob1[0] <= -100 && robotino3[0].CameraToRob1[2]!= 0.0){
                        //ROS_INFO("robotino3_2");
			robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 9; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if(robotino3[0].CameraToRob1[0] >= 100 && robotino3[0].CameraToRob1[2]!= 0.0){
                        //ROS_INFO("robotino3_3");
			robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = -9; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if(robotino3[0].CameraToRob1[0] > -100 && robotino3[0].CameraToRob1[0] < 100 && robotino3[0].CameraToRob1[2]!= 0.0){
                        //ROS_INFO("robotino3_4");
                        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
                        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 0.0 ; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                

        }
        
       	//robotino2 move
        if(Buttons[0].Rob1button[0] == 0 && Buttons[0].Rob2button[0] == 1 && Buttons[0].Rob3button[0] == 0){
                //robotino2
                robotino_2_pub.publish(velocity);
                //robotino1
                //ROS_INFO("robotino2");
                if(robotino1[0].CameraToRob2[2] == 0.0){
                        //ROS_INFO("robotino2_1");
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 10; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
                if( robotino1[0].CameraToRob2[0] <= -100 && robotino1[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_2");
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 9; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
                if(robotino1[0].CameraToRob2[0] >= 100 && robotino1[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_3");
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = -9; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
		if(robotino1[0].CameraToRob2[0] > -100 && robotino1[0].CameraToRob2[0] < 100 && robotino1[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_4");
		        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
		        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 0.0 ; //反時計周り
                        robotino_1_pub.publish(robo1velocity);

                }

                //robotino3
		if(robotino3[0].CameraToRob2[2] == 0.0){
                        //ROS_INFO("robotino2_1");
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 9; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if( robotino3[0].CameraToRob2[0] <= -100 && robotino3[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_2");
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 9; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if(robotino3[0].CameraToRob2[0] >= 100 && robotino3[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_3");
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = -9; //反時計周り
                        robotino_3_pub.publish(robo3velocity);
                }
                if(robotino3[0].CameraToRob2[0] > -100 && robotino3[0].CameraToRob2[0] < 100 && robotino3[0].CameraToRob2[2]!= 0.0){
                        //ROS_INFO("robotino2_4");
                        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
                        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo3velocity.data[0] = 0.0 ;
                        robo3velocity.data[1] = 0.0 ;
                        robo3velocity.data[2] = 0.0 ; //反時計周り
                        robotino_3_pub.publish(robo3velocity);

                }
	}

                
        if(Buttons[0].Rob1button[0] == 0 && Buttons[0].Rob2button[0] == 0 && Buttons[0].Rob3button[0] == 1){
                //robotino3
                robotino_3_pub.publish(velocity);
                //robotino1
                //ROS_INFO("robotino3");
                if(robotino1[0].CameraToRob3[2] == 0.0){
                        //ROS_INFO("robotino2_1");
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 9; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
                if( robotino1[0].CameraToRob3[0] <= -100 && robotino1[0].CameraToRob3[2]!= 0.0){
                        //ROS_INFO("robotino2_2");
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 9; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
                if(robotino1[0].CameraToRob3[0] >= 100 && robotino1[0].CameraToRob3[2]!= 0.0){
                        //ROS_INFO("robotino2_3");
			robo1velocity.data[0] = 0.0 ;
			robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = -9; //反時計周り
                        robotino_1_pub.publish(robo1velocity);
                }
                if(robotino1[0].CameraToRob3[0] > -100 && robotino1[0].CameraToRob3[0] < 100 && robotino1[0].CameraToRob3[2]!= 0.0){
                        //ROS_INFO("robotino2_4");
                        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
                        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo1velocity.data[0] = 0.0 ;
                        robo1velocity.data[1] = 0.0 ;
                        robo1velocity.data[2] = 0.0 ; //反時計周り
                        robotino_1_pub.publish(robo1velocity);

                }

		//robotino2
                if(robotino2[0].CameraToRob3[2] == 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 9; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
                if(robotino2[0].CameraToRob3[0] <= -100 && robotino2[0].CameraToRob3[2]!= 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 9; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
                if(robotino2[0].CameraToRob3[0] >= 100 && robotino2[0].CameraToRob3[2]!= 0.0){
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = -9; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }
                if(robotino2[0].CameraToRob3[0] > -100 && robotino2[0].CameraToRob3[0] < 100 && robotino2[0].CameraToRob3[2]!= 0.0){
                        //ROS_INFO("robotino2_4");
                        //ROS_INFO("robotino1[0].CameraToRob2[0]:%f",robotino1[0].CameraToRob2[0]);
                        //ROS_INFO("robotino1[0].CameraToRob2[2]:%f",robotino1[0].CameraToRob2[2]);
                        robo2velocity.data[0] = 0.0 ;
                        robo2velocity.data[1] = 0.0 ;
                        robo2velocity.data[2] = 0.0 ; //反時計周り
                        robotino_2_pub.publish(robo2velocity);
                }



        }

  
	if(Buttons[0].Rob1button[0] == 0 && Buttons[0].Rob2button[0] == 0 && Buttons[0].Rob3button[0] == 0)
        {
                velocity.data[0] = 0;
                velocity.data[1] = 0;
                velocity.data[2] = 0;
                robotino_1_pub.publish(velocity);
                robotino_2_pub.publish(velocity);
                robotino_3_pub.publish(velocity);

        }
}



int main(int argc, char** argv)
{
  ros::init(argc, argv, "basic_array_listener");
  SubPub subpub;
  ros::Rate loop_rate(20);

while(ros::ok()){
        subpub.Publication();
	ros::spinOnce();
        loop_rate.sleep();
}

  return 0;
}
