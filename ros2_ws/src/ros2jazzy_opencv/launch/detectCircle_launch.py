from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='ros2jazzy_opencv',
            executable='cameraPublisher_node',
            name='cameraPublisher_node',
            output='screen',
            emulate_tty=True,
            parameters=[
                {}
            ]
        ),
        
        Node(
            package='ros2jazzy_opencv',
            executable='detectCircle_node',
            name='detectCircle_node',
            output='screen',
            emulate_tty=True,
            parameters=[
                {}
            ]
        )
    ])