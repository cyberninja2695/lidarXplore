import cv2

import rclpy
from sensor_msgs.msg import Image
from rclpy.node import Node
from cv_bridge import CvBridge

class SubscriberNodeClass(Node):

    def __init__(self):

        super().__init__('subscriberImage_node')
        self.bridgeObject = CvBridge()
        self.topicNameFrames = 'topic_camera_image'
        self.queueSize = 0
        
        #create_subscription(messageType, topicName, callbackFunction, queueSize)
        self.subscription = self.create_subscription(Image, self.topicNameFrames, self.listener_callbackFunction, self.queueSize)
        self.subscription #used to prevent unused variable warning
    
    def listener_callbackFunction(self, imageMessage):
        self.get_logger().info('image frame is received')
        openCVImage = self.bridgeObject.imgmsg_to_cv2(imageMessage)
        cv2.imshow("Camera Video", openCVImage)
        cv2.waitKey(1)

def main(args = None):
    rclpy.init(args = args)
    subscriberObject = SubscriberNodeClass()
    rclpy.spin(subscriberObject)
    subscriberObject.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
