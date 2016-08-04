#include <ros/ros.h>
#include <stdio.h>
#include "sensor_msgs/LaserScan.h"
#include <dji_sdk/dji_drone.h>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>


////////////////////////////////////////////////////////////////
/* TODO change the frame of reference to the body of the aircraft , the flag in  attitude control*/
///////////////////////////////////////////////////////////////


using namespace DJI::onboardSDK;

std::vector<float> obs_dist;
int sensor_num;
const float obstacle_dist_thresh = 1.5;
const float x_vel = 0.3;

void get_dist(const sensor_msgs::LaserScan::ConstPtr& dist)
{
    obs_dist=dist->ranges;
    if(obs_dist.size()!=0)
    {
        ROS_INFO("inside callbak");
        ROS_INFO("The distance to obstacle 1 : %f",obs_dist[0]);
        ROS_INFO("The distance to obstacle 2 : %f",obs_dist[1]);
        ROS_INFO("The distance to obstacle 3 : %f",obs_dist[2]);
        ROS_INFO("The distance to obstacle 4 : %f",obs_dist[3]);
        ROS_INFO("The distance to obstacle 5 : %f",obs_dist[4]);
    }
}

int stop_drone(DJIDrone* drone)
{
     drone->attitude_control(0x40, 0, 0, 0, 0);
     usleep(20000);
     return 1;
}

void move_fwd(DJIDrone* drone)
{

    drone->attitude_control(0x40, x_vel, 0, 0, 0);
    usleep(20000);
}

void land(DJIDrone* drone)
{
    ROS_INFO("LANDING DRONE");
    drone->landing();
    usleep(20000);
}

bool is_obs()
{

    for (int i=1;i<5;i++)
    {
        if(obs_dist[i] < obstacle_dist_thresh)
        {
            sensor_num = i;
            ROS_INFO("OBSTACLE DETECTED in sensor location %d!!\nstopping drone!!",sensor_num);
            return 1;
        }
    }
    return 0;
    /*if (obs_dist[0] < 1 | obs_dist[1] < 1 | obs_dist[2] < 1 | obs_dist[3] < 1 | obs_dist[4] < 1 )
    {
        ROS_INFO("OBSTACLE DETECTED!!\nstopping drone!!");
        sensor_num =
        return 1;
    }
    else
    {
        return 0;
    }*/
}

int main(int argc,char** argv)
{
    ros::init(argc,argv,"obs_avoid");
    ros::NodeHandle nh;
    ros::Subscriber dist_sub;
    DJIDrone* drone = new DJIDrone(nh);
    drone->request_sdk_permission_control();
    ROS_INFO("Acquired permission");
    drone->takeoff();
    ROS_INFO("Taking off");
    usleep(20000);
    sleep(10);
    move_fwd(drone);
    ROS_INFO("MOving forward");
    usleep(20000);



  dist_sub = nh.subscribe<sensor_msgs::LaserScan>("/guidance/obstacle_distance",10,get_dist);
    //ros::spinOnce();
   while(1)
    {
        ros::spinOnce();

        if (obs_dist.size() != 0)
        {
            if (is_obs()==1)
            {
                stop_drone(drone);
                sleep(10);
                land(drone);
                sleep(2);
                drone->release_sdk_permission_control();
                ROS_INFO("Released control");
                break;

            }
            else
            {
                move_fwd(drone);
            }

        }
        else
        {
         ROS_INFO("guidance is off");
        }

    }
    return 0;

}
