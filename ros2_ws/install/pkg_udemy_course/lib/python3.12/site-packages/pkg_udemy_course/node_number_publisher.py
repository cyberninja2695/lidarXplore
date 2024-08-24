#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from example_interfaces.msg import Int64
 
class NodeNumberPublisher(Node): # MODIFY NAME
    def __init__(self):
        super().__init__("node_number_publisher") # MODIFY NAME

        ## publisher
        self.publisher_ = self.create_publisher(Int64, "topic_number", 10)
        self.timer_ = self.create_timer(0.5, self.publish_number)
        self.get_logger().info("node_number_publisher started successfully")

    def publish_number(self):
        msg = Int64()
        msg.data = 2
        self.publisher_.publish(msg)
        self.get_logger().info("number = " + str(msg.data) + " published successfully")
 
 
def main(args=None):
    rclpy.init(args=args)
    node = NodeNumberPublisher() # MODIFY NAME
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()