#include "ros/ros.h"
#include "std_msgs/String.h" 
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h" 
#include "my_control/Velocity.h"

//declarations for publisher and messages.
ros::Publisher pub;
geometry_msgs::Twist my_vel;
geometry_msgs::Twist newvel;
//parameters used.
int num=50;
int num2=60;
int antiblock=0;
int antiblock2=0;
int j,k,f,g,l,h;
bool check=true;
float front=0.85;
float lat=0.55;

//function that returns a positive value if the x argument is the lower one, a negative otherwise.
int min(float x, float y){
	if(x<=y){return 1;}
	else{return -1;}
}

//function that finds the nearer obstacle based on the perceived distances by /base_scan
//in a cone of vision defined by two angles (ranges) and returns the angle at which that
//minimum distance is perceived.
int find_wall(const sensor_msgs::LaserScan::ConstPtr& msg, int x, int y){
	int m=x;
  	for(int i=x; i<y; i++){
  		if(min(msg->ranges[m],msg->ranges[i])>0){}
  		else{m=i;}
  	}
  	return m;
}

//function that does one of three things, based on the argument with which it's called: 
//- if called with num (50) as argument, it resets 2 variables used to avoid that the robot
//makes too many checks without ever moving from its current spot (thus constantly looping 
//between one direction and the opposite, remaining blocked in its place);
//- if called with num2 (60) as argument, it sets frontal and lateral thresholds for the 
//robot based on the new velocity (the logic for this implementation is described in the "Objectives and Useful Info" section of our README);
//- if called with any other integer number as argument, it simply increases it by one (it will be 
//used to increase the 2 variables that prevent blocking -> they can't reach 50 or 60, so they don't
//create conflict with the  first two calls).
int supervisor(int &num){
	if(num==50){
		antiblock=0;
		antiblock2=0;
	}
	else if(num==60){
		if(newvel.linear.x<=2.0){front=0.85; lat=0.55;} 
  		else if(newvel.linear.x<=4.0){front=1.0; lat=0.6;} 
  		else{ front=1.3; lat=0.65; } 
  	}
	else{ num++; }
	return num;
}


//function that implements our service, used to change the current velocity to the
//one the user gave as input (the callback function notices the change thanks to the
//status of the "check" boolean variable -> from the moment this function is called once,
//the callback will always consider newvel as the current velocity).  
bool augment(my_control::Velocity::Request &req, my_control::Velocity::Response &res){
ROS_INFO("input:@[%f] \n",req.x);
newvel.linear.x=req.x;
newvel.angular.z=0.0;
res.x=newvel.linear.x;
res.z=newvel.angular.z;
supervisor(num2);
ROS_INFO("linear velocity augmented to:@[%f] \n",res.x);
check=false;
return true;
}

//function called each time that the topic /base_scan (to which this node is subscribed) publishes
//info about the obstacles currently perceived around the robot (thus, it continuously checks the 
//topic). Its functioning will be described inside the function, step by step:
void base_scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
  //Step (1): the robot's vision is split into six cones; the obstacle whose distance from the 
  //robot is the minimum perceived within each of these cones is found and its angular position is
  //retreived, to indirectly have access to that minimum distance for comparision with the others.
  j=find_wall(msg,0,90);
  h=find_wall(msg,90,270);
  k=find_wall(msg,629,720);
  l=find_wall(msg,450,629);
  f=find_wall(msg,270,360);
  g=find_wall(msg,361,450);
  
  ROS_INFO("@[antiblock: %d , antiblock2: %d] \n @[front-sx: %f; front-dx: %f; \n sx-cone: %f; dx-cone: %f; \n wall-sx: %f; wall-dx: %f]", antiblock, antiblock2, msg->ranges[g], msg->ranges[f], msg->ranges[l], msg->ranges[h], msg->ranges[k], msg->ranges[j]); 
  
  //Step (2): if the minimum distance perceived in one or both of the frontal cones is lower than
  //the set front threshold, turn left or right based on which cone has the lower distance; checks 
  //also the lateral cones distances to avoid wrong turns in certain situations.
  if((antiblock<20) && (msg->ranges[f]<front || msg->ranges[g]<front)){ 
  	
  	if ((msg->ranges[f])<(msg->ranges[g])){
  		supervisor(antiblock);
  		ROS_INFO("frontal check sx");
  		my_vel.linear.x = 0.0;
  		my_vel.angular.z = 0.5;
  	}
  	else if ((msg->ranges[h])<(msg->ranges[l])){
  		supervisor(antiblock);
  		ROS_INFO("frontal check sx");
  		my_vel.linear.x = 0.0;
  		my_vel.angular.z = 0.5;
  	}
  	else{
  		supervisor(antiblock);
  		ROS_INFO("frontal check dx");
  		my_vel.linear.x = 0.0;
  		my_vel.angular.z = -0.5;
  	}
  }
  
  //Step (3): if the minimum distance perceived in one or both of the lateral cones is lower than
  //the set lat threshold, turn left or right based on which cone has the lower distance.
  else if((antiblock2<15) && (msg->ranges[h]<lat || msg->ranges[l]<lat)){
  	if ((msg->ranges[h])<(msg->ranges[l])){
  		supervisor(antiblock2);
  		ROS_INFO("sx check");
  		my_vel.linear.x = 0.0;
  		my_vel.angular.z = 0.5;
  	}
  	else{
  		supervisor(antiblock2);
  		ROS_INFO("dx check");
  		my_vel.linear.x = 0.0;
  		my_vel.angular.z = -0.5;
  	
  	}
  }
  
  //Step (4): if the minimum distance perceived in one of the lateral-wall cones is lower than the
  //one perceived by the other, turn left or right based on which cone has the lower distance; it
  //also carries out two checks: one to avoid blocking by forcing an immediate strong turn of the
  //robot, the other to decide at which speed the robot must move. Substantially, it is a function
  //that handles lateral+forward movement of the robot.
  else if((msg->ranges[j])<(msg->ranges[k])){
  	if(antiblock>=20){
  		my_vel.angular.z = 1.0;  	
  	}
  	else{
  		my_vel.angular.z = 0.015; 
  	}
  	supervisor(num);
  	ROS_INFO("sx wall check");
  	if(check){my_vel.linear.x = 1.0;}
  	else{my_vel.linear.x = newvel.linear.x;}
  }
  
  //Step (5): if the minimum distance perceived in one of the lateral-wall cones is lower than the
  //one perceived by the other, turn left or right based on which cone has the lower distance; it
  //also carries out two checks: one to avoid blocking by forcing an immediate strong turn of the
  //robot, the other to decide at which speed the robot must move. Substantially, it is a function
  //that handles lateral+forward movement of the robot.
  else if((msg->ranges[j])>(msg->ranges[k])){
  	if(antiblock>=20){
  		my_vel.angular.z = -1.0;  	
  	}
  	else{
  		my_vel.angular.z = -0.015;
  	}
  	supervisor(num);
  	ROS_INFO("dx wall check");
  	if(check){my_vel.linear.x = 1.0;}
  	else{my_vel.linear.x = newvel.linear.x;}
  }
  
  //Step (6): if the minimum distances perceived in the lateral-wall cones is the same, give
  //priority to turning left and then move (arbitrary). For all other things, it functions the 
  //same as the previous ones.
  else{
  	if(antiblock>=20){
  		my_vel.angular.z = 1.0; 	
  	}
  	else{
  		my_vel.angular.z = 0.0;
  	}
  	supervisor(num);
  	if(check){my_vel.linear.x = 1.0;}
  	else{my_vel.linear.x = newvel.linear.x;}
  }
  
  //Step (7): publishes the new current calculated linear and angular velocity. 
  pub.publish(my_vel);
}

int main(int argc, char **argv){
  /* You must call ros::init() and define a NodeHandle before using any other part of ROS. */  
  ros::init(argc, argv, "my_control_node");

  ros::NodeHandle n;

  /* Initializations of subsriber, publisher and server. */
  ros::Subscriber sub = n.subscribe("/base_scan", 1000, base_scanCallback); 
  pub = n.advertise<geometry_msgs::Twist> ("/cmd_vel", 1); 
  ros::ServiceServer service= n.advertiseService("/velocity", augment);
  
  /* Continuous loop that keeps the process alive. */
  ros::spin();

  return 0;
}
