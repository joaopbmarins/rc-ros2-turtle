#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber() : Node("turtle")
    {
      subscription_ = this->create_subscription<std_msgs::msg::String>(
      "topic", 10, std::bind(&MinimalSubscriber::command_callback, this, _1));

      publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    }

  private:
    void topic_callback(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
    }

    void command_callback(const std_msgs::msg::String & msg){
      std::string command = msg.data;

      auto message = geometry_msgs::msg::Twist();
      message.linear.x = 0.5;  // Ir para frente (m/s)
      message.angular.z = 0.1; // Girar (rad/s)

      publisher_->publish(message);
      if(command == "up"){

      }
      else if(command == "down"){

      }
      else if(command == "right"){

      }
      else if(command == "left"){

      }
      else{
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", command.c_str());
      }
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}