import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/nijin26/ros2_ws/src/install/ros2jazzy_opencv'
