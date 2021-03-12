#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>

void chatterCallback(const std_msgs::Float64MultiArray& msg)
{
  int num = msg.data.size();
  ROS_INFO("I susclibed [%i]", num);
  for (int i = 0; i < num; i++)
  {
    ROS_INFO("[%i]:%f", i, msg.data[i]);
  }
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "basic_array_listener");
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe("tag_1", 1000, chatterCallback);

  ros::spin();
  return 0;
}
