/*
 * Main class for pbuff relays
 * Author: Harry J.E Day for Bluesat OWR
 * Date: 14/12/14
 */
 

//#include "bluesat_owr_protobuf/Message1Relay.h"
#include "ButtonDefs.h"
#include "RoverDefs.h"

#include "JoystickFilter.h" 

#include <iostream>
#include <list>
#include <cmath>
#include <stdio.h>




#define TOPIC "/owr/position"
//minum number of lat/long inputs to calculate the heading
#define MIN_H_CALC_BUFFER_SIZE 2 
#define JOYSTICK_TOPIC "/owr/joysticks"
#define MID_IN 0
#define DIFF 0.25
#define MAX_IN 1.0
#define MOTOR_MID 1500.0
#define MOTOR_MAX 1900.0
#define MOTOR_MIN 1100.0
#define ROTATION_MID 0.5
 
int main(int argc, char ** argv) {
    
    
    //init ros
    ros::init(argc, argv, "owr_position_node");
    
    JoystickFilter p(JOYSTICK_TOPIC);
    p.spin();
    
    return EXIT_SUCCESS;   
}

JoystickFilter::JoystickFilter(const std::string topic) {
    altitude = 0;
    latitude = 0;
    longitude = 0;
    pitch = 0;
    roll = 0;
    heading = 0;


    //publisher =  node.advertise<owr_messages::position>(topic,10,true);
    publisher = node.advertise<sensor_msgs::Joy>(topic,10,true);
    
    //msgsOut.axes = std::vector<float>(20);
    msgsOut.axes.resize(20);
    joySubscriber = node.subscribe<sensor_msgs::Joy>("joy",2, &JoystickFilter::joyCallback, this);
    armSubscriber = node.subscribe<sensor_msgs::Joy>("arm_joy", 2, &JoystickFilter::armCallback, this);

}


void JoystickFilter::joyCallback(const sensor_msgs::Joy::ConstPtr& joy) {
    

    // Set sensitivity between 0 and 1: 
    //  - 0 makes it output = input 
    //  - 1 makes output = input ^3
    if (joy->buttons[BUTTON_A]) {

        msgsOut.axes[CAMERA_BOTTOM_ROTATE] = joy->axes[STICK_L_LR];
        msgsOut.axes[CAMERA_BOTTOM_TILT] = joy->axes[STICK_L_UD];

        // cameraBottomRotateIncRate = joy->axes[STICK_L_LR] * CAMERA_SCALE;
        // cameraBottomTiltIncRate = joy->axes[STICK_L_UD] * CAMERA_SCALE;
    } else if (joy->buttons[BUTTON_B]) {

        msgsOut.axes[CAMERA_TOP_ROTATE] = joy->axes[STICK_L_LR];
        msgsOut.axes[CAMERA_TOP_TILT] = joy->axes[STICK_L_UD];

        // cameraTopRotateIncRate = joy->axes[STICK_L_LR] * CAMERA_SCALE;
        // cameraTopTiltIncRate = joy->axes[STICK_L_UD] * CAMERA_SCALE;
    } else {
        msgsOut.axes[LEFT_WHEELS] = joy->axes[STICK_L_UD];
        msgsOut.axes[RIGHT_WHEELS] = joy->axes[STICK_R_UD];
    }
    publisher.publish(msgsOut);
}

void JoystickFilter::armCallback(const sensor_msgs::Joy::ConstPtr& joy) {

    // Handle arm movement
    msgsOut.axes[ARM_STICK_TOP] = joy->axes[STICK_R_UD];
    msgsOut.axes[ARM_STICK_BOTTOM] = (joy->axes[STICK_L_UD]) ;//* 0.2;
    msgsOut.axes[ARM_ROTATE] = joy->axes[STICK_CH_LR];

    // Handle claw opening and closing
    if(joy->buttons[BUTTON_LB]) {
        msgsOut.axes[CLAW_STATE] = CLOSE;
    } else if (joy->buttons[BUTTON_RB]) {
        msgsOut.axes[CLAW_STATE] = OPEN;
    } else {
        msgsOut.axes[CLAW_STATE] = STOP;
    }

    //Handle arm rotation
    if(joy->buttons[DPAD_LEFT]) {
        msgsOut.axes[ARM_ROTATE] = ANTICLOCKWISE;
    } else if (joy->buttons[DPAD_RIGHT]) {
        msgsOut.axes[ARM_ROTATE] = CLOCKWISE;
    } else {
        msgsOut.axes[ARM_ROTATE] = STOP;
    }
    publisher.publish(msgsOut);
}


//main loop
void JoystickFilter::spin() {
    while(ros::ok()) {
        ros::spinOnce();
    }
}
