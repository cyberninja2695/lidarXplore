from setuptools import find_packages, setup
import os
from glob import glob


package_name = 'ros2jazzy_opencv'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # ...
        (os.path.join('share', package_name), glob('launch/*launch.[pxy][yma]*')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='nijin26',
    maintainer_email='nijin.lee@gmail.com',
    description='TODO: Package description',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': ['cameraPublisher_node = ros2jazzy_opencv.cameraPublisher:main',
                            'subscriberImage_node = ros2jazzy_opencv.subscriberImage:main',
                            'detectCircle_node = ros2jazzy_opencv.detectCircle:main',
                            'minimalPublisher_node = ros2jazzy_opencv.minimalPublisher:main',
                            'minimalSubscriber_node = ros2jazzy_opencv.minimalSubscriber:main',
                            'minimalParameters_node = ros2jazzy_opencv.minimalParameters:main',
        ],
    },
)
