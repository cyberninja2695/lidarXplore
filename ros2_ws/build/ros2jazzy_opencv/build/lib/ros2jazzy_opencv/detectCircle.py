import cv2

import rclpy
from sensor_msgs.msg import Image
from rclpy.node import Node
from cv_bridge import CvBridge

import numpy as np

class DetectCircleClass(Node):

    def __init__(self):

        super().__init__('detectCircle_node')
        self.bridgeObject = CvBridge()
        self.topicNameFrames = 'topic_camera_image'
        self.queueSize = 0
        
        #create_subscription(messageType, topicName, callbackFunction, queueSize)
        self.subscription = self.create_subscription(Image, self.topicNameFrames, self.listener_callbackFunction, self.queueSize)
        self.subscription #used to prevent unused variable warning
    
    def listener_callbackFunction(self, imageMessage):
        self.get_logger().info('image frame is received')
        openCVImage = self.bridgeObject.imgmsg_to_cv2(imageMessage)
        output = openCVImage.copy()

        gray = cv2.cvtColor(openCVImage, cv2.COLOR_BGR2GRAY)
        # It converts the BGR color space of image to HSV color space 
        hsv = cv2.cvtColor(openCVImage, cv2.COLOR_BGR2HSV) 

        # Threshold of white in HSV space 
        lower_white = np.array([0, 0, 245]) 
        upper_white = np.array([255, 255, 255]) 
        # preparing the mask to overlay 
        mask = cv2.inRange(hsv, lower_white, upper_white)
        # The black region in the mask has the value of 0, 
        # so when multiplied with original image removes all non-blue regions 
        result = cv2.bitwise_and(openCVImage, openCVImage, mask = mask) 

        # Blur using 3 * 3 kernel. 
        gray_blurred = cv2.blur(gray, (3, 3)) 
        canny = cv2.Canny(gray_blurred, threshold1=100, threshold2=200)
        
        
        # detect circles in the image // from: https://www.geeksforgeeks.org/circle-detection-using-opencv-python/
        #circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1.2, 100)
        #img = gray_blurred
        img = canny
        #img = result

        valDp = 2
        valMinDist = 10
        valParam1 = 32
        valParam2 = 200
        valMinRadius = 0
        valMaxRadius = 100
        circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 
                                   dp = valDp, minDist = valMinDist, 
                                   param1 = valParam1, param2 = valParam2, 
                                   minRadius = valMinRadius, maxRadius = valMaxRadius)
        # draw found circles
        if circles is not None:
            # convert the (x, y) coordinates and radius of the circles to integers
            circles = np.round(circles[0, :]).astype("uint16")
            #circles = np.uint16(np.around(circles))
            # loop over the (x, y) coordinates and radius of the circles
            for (x, y, r) in circles:
                # draw the circle in the output image, then draw a rectangle
                # corresponding to the center of the circle
                cv2.circle(output, (x, y), r, (0, 255, 0), 4)
                cv2.rectangle(output, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)

		# draw the circle in the grayscale image, then draw a rectangle
                # corresponding to the center of the circle
                #cv2.circle(gray, (x, y), r, (0, 255, 0), 4)
                #cv2.rectangle(gray, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)

                #cv2.circle(canny, (x, y), r, (0, 255, 0), 4)
                #cv2.rectangle(canny, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)

        # show the output image
        #cv2.imshow("output", np.hstack([openCVImage, output]))
        #cv2.imshow("output", output)
        #cv2.imshow("output", gray)
        #cv2.imshow("output", canny)
        cv2.imshow("output", result)
        cv2.waitKey(1)

def main(args = None):
    rclpy.init(args = args)
    subscriberObject = DetectCircleClass()
    rclpy.spin(subscriberObject)
    subscriberObject.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
