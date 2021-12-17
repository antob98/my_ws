Conents of the Assingment_2 Repository:
==========================================

The contents of this repository delve into my own solution for the 2nd graded assignment given to us in the Research Track class of the Robotics Engineering Master course held in Università degli Studi di Genova. The simulator I used has been provided to us by our professor, so if you are interested in the explaination of all its functionalities or want to know more about ROS, follow the specifications listed in """ [CarmineD8's Github Repository](https://github.com/CarmineD8/second_assignment) """.

Installing and running this code:
------------------------------------

To run this code, make sure you have installed ROS, then simply clone all the contents of this repository in your own environment or workspace, using the command:  
> `git clone url`

where "url" is the link to this repository. After that, go in the root of your acquired workspace with the command "cd correct_path/my_ws" (where correct_path is your specific path to the workspace) and run the commands:  
> `catkin_make` and `rospack profile`

As the last step, open three different shells and write the commands:  
> `roscore &` and then `rosrun stage_ros stageros $(rospack find second_assignment)/world/my_world.world` (on the first shell)  
> `rosrun my_control my_control_node` (on the second shell)  
> `rosrun my_user my_user_node` (on the third shell)

If you haven't downloaded Ros, you can do it by following the instuctions listed in """ [ROS Wiki](https://wiki.ros.org) """.  

Note: if not already present, add the line "source /opt/ros/noetic/setup.bash" to your .bashrc file using the command:  
> `gedit .bashrc`  
Also, add the line "source /correct_path/my_ws/devel/setup.bash", where correct_path is your specific path to the workspace.

For other useful info on the simulator used to produce this code, or the installation of the components required to make it work, you can always refer to the original repository granted to us by our professor. The link to his repository has been already given in the previous paragraph.

## Objectives and Useful Info:

The code produced for this assignment is contained in "my_ws/src", specifically in the 2 folders "my_control" and "my_user" and has been carried out in ROS. Other than that, we produced this README to explain its contents.  
The core of the assignment was:  
> Use the simulator provided to make the robot move autonomously inside a circuit. Use ROS for controlling the robot, which is endowed with laser scanners.  
> Produce 2 nodes:  
> one for control of the robot;  
> one which interacts with the user to either: increase / decrease the speed of the robot; reset the robot’s position.

The code we produced handles all points of the assignment without any problems. We implemented the control node both as a controller and a server, so that it handles the service used by the user node to augment the velocity directly within it. The user node waits for the user's input, who can choose to reset the robot's position or change its velocity. Thus, the user can write as input the velocity he/she desires, either as an integer or a float. There is to note, though, that our code works best for velocities up to 6 times the starting one. Higher velocities may cause crashes in specific parts of the circuit or turnarounds in some angles, but may as well handle it fine depending on the situation (since the behaviour of the robot isn't always the same when its velocity changes). However, note that our experiments on different velocities have been performed always from the starting point of the robot in the circuit (when we wanted to see the effects of a new velocity input we also issued a reset for the robot's position). This means that, especially in tight spots or complex situations, issuing any higher velocity to the robot during its course could lead it to a crash. The maximum velocity limitation could be improved, however we preferred to focus on the quality of the robot's movement and on smoothing its trajectory instead of making it go faster. Also, on this topic, we made one important consideration: at the starting velocity of 1.0, our robot finishes the circuit in approximately 6 minutes; if we consider that the true Monza circuit has a total lenght of 5793 m, we can say that our robot moves initially at around 58 km/h; this means that if we set the velocity to 6 times the starting one, the robot would be moving at 348 km/h, thus almost at the highest speed ever recorded on the real circuit in a F1 competition, which is 372 km/h. But this was a peak velocity reached during a specific section! In fact, the average sustained speed record on this track is even lower than 300 km/h. This leads us to think that there's not a real need to have it move at higher speeds. On the control side, we implemented it as both a publisher and a server: it provides 5 functions, one that implements the service that the user node uses to modify the robot's velocity, one callback and three other functions useful for it. To control the robot, we divided its range of vision in six cones, taking the minimum perceived distance from the /base_scan in each cone and, when the minimum is lower than a threshold, having the robot move accordingly to avoid the ostacles. We also implemented a supervisor function to avoid the robot blocking itself in place in some specific situations (a behaviour that will be discussed in the "Possible Improvements" section) and accurately sets new thresholds based on the current given velocity input. This was made thinking about real life situations: if you go at higher speed while driving, you also need to pay more attention, and this becomes more and more true the higher your velocity gets. For example, you might need to start pushing the brackets or adjusting your trajectory progressively earlier, so it makes sense that our thresholds also change based on the current velocity. 

### Code Behaviour (pseudocode):

The main part of the control.cpp code handles the subscription to the /base_scan topic, the publications to the /cmd_vel topic and the server to the Velocity service calls, which we implemented ourself. It does so, by doing the following things:
- initialize the node
- build the NodeHandle
- subscribe to /base_scan:
- publish on /cmd_vel:
- be a server for the Velocity service
- while looping:  
___do nothing.  

In the next part of this section we will describe the Callback function provided for the subscription to /base_scan. However, before that, note that in this pseudocode:
- "find minimum between x and y" is equivalent to:  
___If x is lower than y:  
_____return the value 1.  
___Else:  
_____return the value -1.  

- "find obstacle between x and y" is equivalent to:  
___Set m to x;  
___For each range i between the ranges x and y of the received /base_scan message:  
_____If "find minimum between m and i" is positive:  
_______Do nothing.  
_____Else:  
_______Set m to i.  
___return the range m.  

- "supervise number" is equivalent to:  
___If number is 50:  
_____Set antiblock to 0;  
_____Set antiblock2 to 0;  
___Else if number is 60:  
_____If "linear newvel" is lower than or equal to 2.0:  
_______Set front to 0.85;  
_______Set lat to 0.55;  
_____If "linear newvel" is lower than or equal to 4.0:  
_______Set front to 1.0;  
_______Set lat to 0.6;  
_____Else:  
_______Set front to 1.3;  
_______Set lat to 0.65;  
___Else:  
_____Set number to "number+1";  
___return the value number.  

- "augment" is equivalent to:  
___Set "linear newvel" to "request on x";  
___Set "angular newvel" to "0.0";  
___Set "response on x" to "linear newvel";  
___Set "response on z" to "angular newvel";  
___supervise 60;  
___Set check to false;  
___return true.  

The following part describes the base_scanCallback function:
- Set j to "find obstacle between 0 and 90";  
- Set h to "find obstacle between 90 and 270";  
- Set k to "find obstacle between 629 and 720";  
- Set l to "find obstacle between 450 and 629";  
- Set f to "find obstacle between 270 and 360";  
- Set g to "find obstacle between 361 and 450";  
- If antiblock is lower than 20 and f range's distance or g range's distance are lower than front:  
___If f range's distance is lower than g range's distance:  
_____supervise antiblock;  
_____Set linear my_vel to 0.0;  
_____Set angular my_vel to 0.5;  
___Else if h range's distance is lower than l range's distance:  
_____supervise antiblock;  
_____Set linear my_vel to 0.0;  
_____Set angular my_vel to 0.5;  
___Else:  
_____supervise antiblock;  
_____Set linear my_vel to 0.0;  
_____Set angular my_vel to -0.5.  

- Else if antiblock2 is lower than 15 and h range's distance or l range's distance are lower than lat:  
___If h range's distance is lower than l range's distance:  
_____supervise antiblock2;  
_____Set linear my_vel to 0.0;  
_____Set angular my_vel to 0.5;  
___Else:  
_____supervise antiblock2;  
_____Set linear my_vel to 0.0;  
_____Set angular my_vel to -0.5.  

- Else if j range's distance is lower than k range's distance:  
___If antiblock is greater or equal to 20:  
_____Set angular my_vel to 1.0;  
___Else:  
_____Set angular my_vel to 0.015;  
___supervise 50;  
___If check is equal to true:  
_____Set linear my_vel to 1.0;  
___Else:  
_____Set linear my_vel to linear newvel.  

- Else if j range's distance is higher than k range's distance:  
___If antiblock is greater or equal to 20:  
_____Set angular my_vel to -1.0;  
___Else:  
_____Set angular my_vel to -0.015;  
___supervise 50;  
___If check is equal to true:  
_____Set linear my_vel to 1.0;  
___Else:  
_____Set linear my_vel to linear newvel.  

- Else:  
___If antiblock is greater or equal to 20:  
_____Set angular my_vel to 1.0;  
___Else:  
_____Set angular my_vel to 0.015;  
___supervise 50;  
___If check is equal to true:  
_____Set linear my_vel to 1.0;  
___Else:  
_____Set linear my_vel to linear newvel.  

- Publish my_vel on the /cmd_vel topic.  

The following part is dedicated to the user code side of the assignment, that acts as a client for the control server, and is contained in my_user.cpp:
- Set a "loop rate" and initialize all things required;  
- While ros is active:  
___Print "hi user, please write 'a' if you want to augment the velocity, 'r' if you want to reset, or 'q' if you want to quit: \n";  
___Acquire input from keyboard and set "in" to it;  
___If "in" is "a":  
_____Print "input a number for the linear velocity";  
_____Acquire input from keyboard and set "request on x" to it;  
_____Set "request on z" to 0.0;  
_____Wait for the existence of the service;  
_____Call the service to issue the augment velocity request;  
___Else if "in" is "r":  
_____Wait for the existence of the service;  
_____Call the service to issue the reset request;  
___Else if "in" is "q":  
_____Print "bye! \n";  
_____Terminate the program;  
___Else:  
_____Print "wrong input, try again! \n";  
___Sleep for "loop rate".

#### Possible Improvements:
Two main improvements could be made to our code: 

- the first one, regarding movement: The way our robot moves is just pseudo-continuous, meaning that when it perceives an ostacle through its checks, it stops before turning to a new orientation to avoid it. This means that, unlike a real driving car in a competition, it doesn't move constantly, but rather only when sure to be correctly orientated to avoid crashes. This, of course, is an "easy way" of implementing the code required for the assignment. To make the robot's movement continuous would be a big improvement of this solution. Also, we observed in our first implementation of this code that if the robot orientates itself in a way that causes it to face forward into a steep angle (in particular the first one encountered from the starting point in the circuit), it could happen that it blocks itself in place. We avoided this through our "supervisor" function, however now in the same situation to avoid crashing the robot may sometimes change its course direction, starting to traverse the circuit backwards. This is very rare, though, and it's due to the fact that the circuit walls are not exactly smooth, thus it may happen that the robot gets stuck in a loop of turning in opposite directions until it finds an orientation that allows it to escape the current position, thanks to our supervisor function. However, the direction it will go to free itself depends on how it was last orientated, so there's no guarantee it will be the correct one. This may be fixed by implementing a memory system through which the robot remembers the direction from where it came before getting stuck, and decides based on that.  

- the second one, regarding speed: the robot could be made to go faster without crushing by fiddling with either the angles of orientation, the cones of vision or the thresholds considered for the obstacle checks. Supposing that there are better options, or specific options that work for higher velocities, one could reason more deeply on this implementation and through that (or a trial and error approach) improve on this aspect. However, there's to say that our robot can already handle speeds superior to the limitations we talked about, just "not as well", and considering also the considerations made on real life cases on the same circuit, this kind of improvement might just be nitpicking.

