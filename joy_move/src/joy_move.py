#!/usr/bin/env python


import rospy 
import sys 
import math 
from std_msgs.msg import Float32 
from std_msgs.msg import Float32MultiArray
from std_msgs.msg import Bool

from sensor_msgs.msg import Joy
from std_msgs.msg import String


class Joy_move(object):
    def __init__(self):
        self._joy_sub = rospy.Subscriber('joy', Joy, self.joy_callback, queue_size=10)
        self._robotino_1_pub = rospy.Publisher('robotino1/velocity', Float32MultiArray, queue_size=10)
        self._robotino_2_pub = rospy.Publisher('robotino2/velocity', Float32MultiArray, queue_size=10)
    def joy_callback(self, joy_msg):
        velocity = Float32MultiArray()
        x = joy_msg.axes[1]*300
        y = joy_msg.axes[0]*300
        z = joy_msg.axes[3]*100
        velocity.data = (x, y, z)
        if joy_msg.buttons[4] == 1:
            rospy.loginfo("Robotino1")
            self._robotino_1_pub.publish(velocity)
        if joy_msg.buttons[5] == 1:
            rospy.loginfo("Robotino2")
            self._robotino_2_pub.publish(velocity)
        if joy_msg.buttons[4] == 0:
            velocity.data= (0, 0, 0)
            self._robotino_1_pub.publish(velocity)
        if joy_msg.buttons[5] == 0:
            velocity.data= (0, 0, 0)
            self._robotino_2_pub.publish(velocity)




if __name__ == '__main__':
    rospy.init_node('joystick_move')
    joystick_move = Joy_move()
    rospy.spin()

