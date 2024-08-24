#include "rclcpp/rclcpp.hpp"
 
class NodeTemplateCPP : public rclcpp::Node // MODIFY NAME
{
public:
    NodeTemplateCPP() : Node("node_template_cpp") // MODIFY NAME
    {
        RCLCPP_INFO(this->get_logger(), "node_template_cpp started successfully");
    }
 
private:
};
 
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NodeTemplateCPP>(); // MODIFY NAME
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}