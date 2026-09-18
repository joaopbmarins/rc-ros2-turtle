cmake_minimum_required(VERSION 3.8)
project(turtle_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)


add_executable(send_msg src/send_msg.cpp)
ament_target_dependencies(send_msg rclcpp std_msgs)

add_executable(turtle src/turtle.cpp)
ament_target_dependencies(turtle rclcpp std_msgs geometry_msgs turtlesim)

install(TARGETS
  send_msg
  turtle
  DESTINATION lib/${PROJECT_NAME})

ament_package()