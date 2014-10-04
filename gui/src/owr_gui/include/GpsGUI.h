/*
 * This manages input into the GUI from ROS
 * By Harry J.E Day for BlueSat OWR <Harry@dayfamilyweb.com>
 */

#include "comms.h"
#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>

class GPSGUI {

    public:
        GPSGUI();
        void spin();
        void reciveGpsMsg(const sensor_msgs::NavSatFix::ConstPtr& msg);
        
    private:
        std::string coOrdList;
        ros::Subscriber gpsSub;
        ListNode list;
        ListNode end;
};
