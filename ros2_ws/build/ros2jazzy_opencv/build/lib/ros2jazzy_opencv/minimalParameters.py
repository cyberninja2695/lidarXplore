import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter

class MinimalParam(Node):
    def __init__(self):
        super().__init__('minimalParameters_node')

        self.declare_parameter('my_parameter', 'world')

        self.timer = self.create_timer(1, self.timer_callback)

    def timer_callback(self):
        my_param = self.get_parameter('my_parameter').get_parameter_value().string_value

        self.get_logger().info('Hello %s!' % my_param)

        my_new_param = Parameter(
            'my_parameter',
            Parameter.Type.STRING,
            'world'
        )
        all_new_parameters = [my_new_param]
        #self.set_parameters(all_new_parameters)

def main():
    rclpy.init()
    node = MinimalParam()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()