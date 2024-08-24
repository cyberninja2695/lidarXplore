import cv2
import psycopg2

import rclpy
from sensor_msgs.msg import Image
from rclpy.node import Node
from cv_bridge import CvBridge

# Defining database connection parameters
db_params = {
    "host": "localhost",
    "database": "testdb",
    "user": "postgres",
    "password": "123"
}
try:
    # Establishing a connection to the database
    connection = psycopg2.connect(**db_params)
    # Creating a cursor object to interact with the database
    cursor = connection.cursor()
    # Performing database operations here...
    #cursor.execute('SELECT * FROM tutorials')
    #for record in cursor.fetchall():
    #    print(record)

except (Exception, psycopg2.Error) as error:
    print(f"Error connecting to the database: {error}")

#finally:
#    if connection:
#        cursor.close()
#        connection.close()
#        print("Database connection closed.")

#argument Node means PublisherNodeClass inherits Node

class PublisherNodeClass(Node):

    def __init__(self):

        super().__init__('cameraPublisher_node')

        self.cameraDeviceNumber=0
        self.camera = cv2.VideoCapture(self.cameraDeviceNumber)
        res = [640, 480]
        self.camera.set(cv2.CAP_PROP_FRAME_WIDTH, res[0])
        self.camera.set(cv2.CAP_PROP_FRAME_HEIGHT, res[1])

        self.bridgeObject = CvBridge()
        self.topicNameFrames = 'topic_camera_image'
        self.queueSize = 0
        self.frameRate = 200 #Hz
        
        #create_publisher(messageType, topicName, queueSize)
        self.publisher = self.create_publisher(Image, self.topicNameFrames, self.queueSize)
        
        self.periodCommunication = 1/self.frameRate 
        self.timer = self.create_timer(self.periodCommunication, self.timer_callbackFunction)
        self.count = 0

    def timer_callbackFunction(self):

        success, frame = self.camera.read()
        #frame = cv2.resize(frame, (320,240), interpolation=cv2.INTER_CUBIC)

        if success == True :
            ROS2ImageMessage = self.bridgeObject.cv2_to_imgmsg(frame)
            self.publisher.publish(ROS2ImageMessage)

        # Performing database operations here...
        #cursor.execute('SELECT * FROM tutorials')
        #for record in cursor.fetchall():
        #    print(record)

        #self.get_logger().info('Publishing image number %d'% self.count)
        self.count += 1

def main(args = None):
    rclpy.init(args = args)
    publisherObject = PublisherNodeClass()
    #Here we spin the ROS code
    rclpy.spin(publisherObject)
    
    if connection:
        cursor.close()
        connection.close()
        print("Database connection closed.")

    publisherObject.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()