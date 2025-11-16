cmake_minimum_required(VERSION 3.8)
project(rover_control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cartographer REQUIRED)
find_package(cartographer_msgs REQUIRED)
find_package(nav2_map_server REQUIRED)

add_executable(goal_sender goal_sender.cpp)
ament_target_dependencies(goal_sender
  rclcpp
  geometry_msgs
  tf2
)

add_executable(odom_printer src/odom_printer.cpp)
ament_target_dependencies(odom_printer
  rclcpp
  nav_msgs
  geometry_msgs
  tf2
  tf2_ros
)

install(TARGETS
  goal_sender
  odom_printer
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
