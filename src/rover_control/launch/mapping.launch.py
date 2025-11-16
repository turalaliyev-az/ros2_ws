import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    cartographer_config_dir = os.path.join(get_package_share_directory('turtlebot3_cartographer'), 'config')
    cartographer_config_basename = 'turtlebot3_lds_2d.lua'

    return LaunchDescription([
        Node(
            package='cartographer',
            executable='cartographer_node',
            name='cartographer_node',
            output='screen',
            parameters=[{'use_sim_time': False}],
            arguments=[
                '-configuration_directory', cartographer_config_dir,
                '-configuration_basename', cartographer_config_basename
            ],
            remappings=[
                ('scan', '/scan'),
                ('odom', '/odom'),
            ]
        ),
        Node(
            package='cartographer',
            executable='cartographer_occupancy_grid_node',
            name='cartographer_occupancy_grid_node',
            output='screen',
            parameters=[{'use_sim_time': False}],
            arguments=[
                '-resolution', '0.05',
                '-publish_period_sec', '1.0'
            ]
        ),
        Node(
            package='nav2_map_server',
            executable='map_saver_server',
            name='map_saver_server',
            output='screen',
            parameters=[
                {'save_map_timeout': 5000},
                {'free_thresh_default': 0.196},
                {'occupied_thresh_default': 0.65}
            ]
        )
    ])