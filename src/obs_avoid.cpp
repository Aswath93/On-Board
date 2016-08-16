#include <ros/ros.h>
#include <stdio.h>
#include "sensor_msgs/LaserScan.h"
#include <dji_sdk/dji_drone.h>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <dji_sdk/obs_msg.h>

////////////////////////////////////////////////////////////////
/* TODO change the frame of reference to the body of the aircraft , the flag in  attitude control*/
///////////////////////////////////////////////////////////////


using namespace DJI::onboardSDK;
bool s1,s2,s3,s4;

std::vector<float> obs_dist;
int sensor_num;
const float obstacle_dist_thresh = 0.5;
//const float x_vel = 0.3;

void get_dist(const sensor_msgs::LaserScan::ConstPtr& dist)
{
    obs_dist=dist->ranges;
    if(obs_dist.size()!=0)
    {
        ROS_INFO("inside callbak");
        ROS_INFO("The distance to obstacle 0 : %f",obs_dist[0]);
        ROS_INFO("The distance to obstacle 1 : %f",obs_dist[1]);
        ROS_INFO("The distance to obstacle 2 : %f",obs_dist[2]);
        ROS_INFO("The distance to obstacle 3 : %f",obs_dist[3]);
        ROS_INFO("The distance to obstacle 4 : %f",obs_dist[4]);
    }

}



bool is_obs()
{
    bool obs_flag = 0;
    for (int i=1;i<5;i++)
    {
        if(obs_dist[i] < obstacle_dist_thresh)
        {
            if (i==1)
            {
                s1=false;

            }
            else if(i==2)
            {
                s2=false;

            }
            else if(i==3)
            {
                s3=false;
            }
            else if(i==4)
            {
                s4=false;
            }
            obs_flag = 1;


            ROS_INFO("OBSTACLE DETECTED in sensor location %d!!\nstopping drone!!",i);

         }

    }

    if (obs_flag==1)
    {
    return 1;
    }
    else
    {
    return 0;
    }

}

int main(int argc,char** argv)
{
    ros::init(argc,argv,"obs_avoid");
    ros::NodeHandle nh;
    ros::Subscriber dist_sub;
    ros::Publisher obs_flag_pub = nh.advertise<dji_sdk::obs_msg>("obs_avoid",1000);
    ROS_INFO("inside main");
   // DJIDrone* drone = new DJIDrone(nh);
    dji_sdk::obs_msg obs_msg ;
    obs_msg.is_obs = false;
    obs_msg.sensor_1=true;
    obs_msg.sensor_2=true;
    obs_msg.sensor_3=true;
    obs_msg.sensor_4=true;


    ROS_INFO("initialized sensor");



  dist_sub = nh.subscribe<sensor_msgs::LaserScan>("/guidance/obstacle_distance",10,get_dist);

   while(ros::ok() )
    {
        ros::spinOnce();



        if (obs_dist.size() != 0)
        {
            if (is_obs()==1)
            {

                obs_msg.is_obs = false;

                if (s1==false)
                {
                    obs_msg.sensor_1=false;

                }
                if (s2==false)
                {
                   obs_msg.sensor_2=false;

                }
                if (s3==false)
                {
                    obs_msg.sensor_3=false;
                }
                if (s4==false)
                {
                    obs_msg.sensor_4=false;
                }



            }
            else
            {
                obs_msg.is_obs = true;

            }


        }
        else
        {
             ROS_INFO("guidance is off");
             obs_msg.is_obs = true;
        }



        obs_flag_pub.publish(obs_msg);

        if (!obs_msg.is_obs)
        {
            usleep(20000);
        }
        else
        {
            usleep(2000);
        }

        s1=true;
        s2=true;
        s3=true;
        s4=true;
        obs_msg.sensor_1=true;
        obs_msg.sensor_2=true;
        obs_msg.sensor_3=true;
        obs_msg.sensor_4=true;



    }
    return 0;

}
