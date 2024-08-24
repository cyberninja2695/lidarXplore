#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

from example_interfaces.msg import Int64
 
 
class NodeNumberCounter(Node): # MODIFY NAME
    def __init__(self):
        super().__init__("node_number_counter") # MODIFY NAME
        
        ##subscriber
        self.subscriber_ = self.create_subscription(Int64, "topic_number", self.callback_subscriber, 10)

        ## publisher
        self.publisher_ = self.create_publisher(Int64, "topic_number_count", 10)
        #self.timer_ = self.create_timer(0.5, self.publish_counter)
        self.get_logger().info("node_number_counter started successfully")

        self.counter_ = 0

    def callback_subscriber(self, msg):
        self.msg_in_ = Int64()
        self.msg_in_.data = msg.data
        self.get_logger().info("received number = " + str(self.msg_in_.data ))

        self.publish_counter()

    def publish_counter(self):
        self.counter_+=1
        self.get_logger().info("counter = " + str(self.counter_))
        msg_out = Int64()
        msg_out.data = self.msg_in_.data + self.counter_
        self.publisher_.publish(msg_out)
        self.get_logger().info("published counter = " + str(msg_out.data))
 
def main(args=None):
    rclpy.init(args=args)
    node = NodeNumberCounter() # MODIFY NAME
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()