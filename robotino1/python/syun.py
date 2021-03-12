#!/usr/bin/env python
#
import rospy
import sys
import math
from std_msgs.msg import Float32
from std_msgs.msg import Float32MultiArray
from std_msgs.msg import Bool

from std_msgs.msg import String

class move_spin(object):
    def __init__(self):
    
        self._robotino_1 = rospy.Publisher('robotino1/velocity', Float32MultiArray, queue_size = 10)

        
        velocity = Float32MultiArray()
        while not rospy.is_shutdown():
         spin = raw_input('k,s,return : stop >')
         if 'k' in spin: z = 100
         if 's' in spin: z = 0
         velocity.data = (0,0,z)
         rospy.loginfo(velocity)
         self._robotino_1.publish(velocity)
        

if __name__ == '__main__':
    rospy.init_node('syun_spin')
    syun_spin = move_spin()
    rospy.spin()
