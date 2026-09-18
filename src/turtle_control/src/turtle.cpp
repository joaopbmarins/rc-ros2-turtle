#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

using std::placeholders::_1;

class Turtle : public rclcpp::Node
{
  public:
    Turtle() : Node("turtle")
    {
      sub_receive_ = this->create_subscription<std_msgs::msg::String>(
      "/turtle_control", 10, std::bind(&Turtle::command_callback, this, _1));

      sub_position_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10, std::bind(&Turtle::position_callback, this, _1));

      pub_command_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    }

  private:
    void command_callback(const std_msgs::msg::String & msg){
      std::string command = msg.data;

      auto message = geometry_msgs::msg::Twist();
      message.linear.x = 0;  // Ir para frente eixo x (m/s)
      message.linear.y = 1;  // Ir para frente eixo y (m/s)
      message.angular.z = 0.1; // Girar (rad/s)

      pub_command_->publish(message);
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

    void position_callback(const turtlesim::msg::Pose &pos){
      RCLCPP_INFO(this->get_logger(), "Posição -> x: %.2f, y: %.2f, theta: %.2f", 
                pos.x, pos.y, pos.theta);
      RCLCPP_INFO(this->get_logger(), "Velocidades -> Linear: %.2f, Angular: %.2f", 
                pos.linear_velocity, pos.angular_velocity);
    }

    
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_position_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_receive_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_command_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Turtle>());
  rclcpp::shutdown();
  return 0;
}