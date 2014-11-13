/*
 * Rellay Node for transmitting using the protobuf protocol
 * Author: Harry J.E Day for Bluesat OWR
 * Date: 2/08/2014
 */
 
#ifndef PBuffRelay_H
#define PBuffRelay_H

#define MESSAGE_CLASS message1
#define MESSAGE_CLASS_ROS message1_ros

#define TOPIC "/owr_protobuf/" "MESSAGE_CLASS"

#include "messages1.pb.h"
#include <ros/ros.h>

class PBuffRelay {
    
    public:
        PBuffRelay();
        void spin();
        
    private:
        ros::Subscriber sub;
        bluesat_owr_protobuf::MESSAGE_CLASS testMessage;
        ros::NodeHandle node;
};


#endif
