
colcon build

source install/setup.bash

ros2 run turtle_control turtle
ros2 run turtle_control send_msg


ros2 run turtlesim turtlesim_node