#include "ros/ros.h" 
#include "std_msgs/String.h"  
#include "geometry_msgs/Twist.h" 
#include "std_srvs/Empty.h" 
#include "my_control/Velocity.h"

int main(int argc, char **argv){
//needed definitions and initializations:
ros::init(argc, argv, "my_user_node");
ros::NodeHandle n;
ros::Rate loop_rate(10); 

char in[80];
ros::ServiceClient client1 = n.serviceClient<std_srvs::Empty>("/reset_positions");
ros::ServiceClient client2 = n.serviceClient<my_control::Velocity>("/velocity");

std_srvs::Empty srv1;
my_control::Velocity srv2;

//Loop that handles the entire user-interface: takes an imput from the user, checks if the input is 
//one of those supported and does the corresponding instructions (the first check asks a velocity
//input from the user, then calls the control Velocity service issuing its request; the second
//check calls the reset service; the third check closes the interface, terminating the process).  
while(ros::ok()){
    ROS_INFO("hi user, please write 'a' if you want to augment the velocity, 'r' if you want to reset, or 'q' if you want to quit: \n");
    std::cin>>in;
    
    if(strcmp(in,"a")==0){
    ROS_INFO("input a number for the linear velocity");
    std::cin>>srv2.request.x;
    ROS_INFO("output:@[%f] \n",srv2.request.x);
    srv2.request.z=0.0;
    client2.waitForExistence();
    client2.call(srv2);
    }
    else if(strcmp(in,"r")==0){
    client1.waitForExistence();
    client1.call(srv1);
    }
    else if(strcmp(in,"q")==0){
    ROS_INFO("bye! \n");
    return 0;
    }
    else{
    ROS_INFO("wrong input, try again! \n");
    }

    loop_rate.sleep();
}
return -1;
}
