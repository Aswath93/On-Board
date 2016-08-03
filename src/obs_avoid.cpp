#include <ros/ros.h>
#include <stdio.h>
#include "sensor_msgs/LaserScan.h"
#include <dji_sdk/dji_drone.h>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>

using namespace DJI::onboardSDK;

std::vector<float> obs_dist;
/*
void get_dist(const sensor_msgs::LaserScan::ConstPtr& dist)
{
    obs_dist=dist->ranges;
    ROS_INFO("inside callbak");
    ROS_INFO("The distance to obstacle 1 : %f",obs_dist[0]);
    ROS_INFO("The distance to obstacle 2 : %f",obs_dist[1]);
    ROS_INFO("The distance to obstacle 3 : %f",obs_dist[2]);
    ROS_INFO("The distance to obstacle 4 : %f",obs_dist[3]);
    ROS_INFO("The distance to obstacle 5 : %f",obs_dist[4]);
}*/

/*void stop_drone(DJIDrone* drone)
{
    ROS_INFO("OBSTACLE DETECTED!!\nstopping drone!!");
     drone->attitude_control(0x40, 0, 0, 0, 0);
     usleep(20000);
}*/

int main(int argc,char** argv)
{
    ros::init(argc,argv,"obs_avoid");
    ros::NodeHandle nh;
    ros::Subscriber dist_sub;
    DJIDrone* drone = new DJIDrone(nh);
    drone->request_sdk_permission_control();
    drone->takeoff();
    drone->attitude_control(0x40, 0, 2, 0, 0);



  //  dist_sub = nh.subscribe<sensor_msgs::LaserScan>("/guidance/obstacle_distance",10,get_dist);
    //ros::spinOnce();
   while(1)
    {
        ros::spinOnce();
        /*if (obs_dist[1] < 0.5)
        {

             drone->attitude_control(0x40, 0, 0, 0, 0);
             usleep(20000);
        }*/
    }
    return 0;

}
