#!/usr/bin/env python3
import rclpy
from std_msgs.msg import Int64 ## MODIFY MSG
from rclpy.node import Node
 
 
class NodeTemplatePY(Node): # MODIFY NODE CLASS NAME
    def __init__(self):
        super().__init__("node_template_py") # MODIFY NODE NAME
        self.get_logger().info("node_template_py started successfully")
        
        self.counter_ = 0

        ## publisher
        self.publisher_ = self.create_publisher(Int64, "template_topic_name", 10) # MODIFY TOPIC NAME
        self.publish_frequency_ = 2 # PUBLISH FREQUENCY (Hz)
        self.timer_ = self.create_timer((1/self.publish_frequency_), self.publisher_function) # Start Timer to publish at publish_frequency

        ##subscriber
        self.subscriber_ = self.create_subscription(Int64, "template_topic_name", self.callback_subscriber, 10) # MODIFY TOPIC NAME

        
    def publisher_function(self): 
    # This function publishes a counter into /template_topic_name
        self.counter_+=1
        msg_out = Int64() # Initialize the msg to be published
        msg_out.data = self.counter_
        self.publisher_.publish(msg_out)
        self.get_logger().info("published counter msg = " + str(msg_out.data))

    def callback_subscriber(self, msg):
    # This function is called when a msg is available in /template_topic_name
        self.msg_in_ = Int64() # Initialize the msg to be subscribed
        self.msg_in_.data = msg.data
        self.get_logger().info("subscribed counter msg = " + str(self.msg_in_.data))
 
 
def main(args=None):
    rclpy.init(args=args)
    node = NodeTemplatePY() # MODIFY NODE CLASS NAME
    rclpy.spin(node)
    rclpy.shutdown()
 
 
if __name__ == "__main__":
    main()