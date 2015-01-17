#!/usr/bin/env python
import rospy
from std_msgs.msg import String

#called when the message is recived
def callback(data):
    print data

def activateFeeds():
    rospy.init_node('activate feeds')
    rospy.Subscriber("control/activateFeeds", String, callback)
    
    #loops while ros is runnign
    rospy.spin()
    
    
if __name__ == '__main__':
    activateFeeds()