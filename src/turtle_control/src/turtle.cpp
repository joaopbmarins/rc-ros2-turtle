#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

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

      teleport_client_ = this->create_client<turtlesim::srv::TeleportAbsolute>("/turtle1/teleport_absolute");
      timer_ = this->create_wall_timer(100ms, std::bind(&Turtle::teleport_turtle, this));
      
    }

  private:
    void teleport_turtle(){
      if (!teleport_client_->wait_for_service(0s)) {
        RCLCPP_INFO(
          this->get_logger(),
          "Waiting for /turtle1/teleport_absolute...");
        return;
      }

      auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();

      request->x = 0.0;
      request->y = 0.0;
      request->theta = 0.0;

      RCLCPP_INFO( this->get_logger(), "Teleporting turtle1 to (0, 0)...");

      auto future = teleport_client_->async_send_request(
        request,
        [this](rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedFuture)
        {
          RCLCPP_INFO(this->get_logger(), "Turtle teleported to (0, 0)");
          timer_->cancel();
        });
    }

    void command_callback(const std_msgs::msg::String & msg){
      std::string command = msg.data;

      if (moving_) {
        RCLCPP_INFO(this->get_logger(), "Turtle is still moving...");
        return;
      }

      if(command == "up"){
        target_x_ = current_x_;
        target_y_ = current_y_ + 1.0;

        moving_ = true;
      }
      else if(command == "down"){
        target_x_ = current_x_;
        target_y_ = current_y_ - 1.0;

        moving_ = true;
      }
      else if(command == "right"){
        target_x_ = current_x_ + 1.0;
        target_y_ = current_y_;

        moving_ = true;
      }
      else if(command == "left"){
        target_x_ = current_x_ - 1.0;
        target_y_ = current_y_;

        moving_ = true;
      }
      else{
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", command.c_str());
      }
    }

    void position_callback(const turtlesim::msg::Pose &pos){
      current_x_ = pos.x;
      current_y_ = pos.y;

      if (moving_) {

        auto message = geometry_msgs::msg::Twist();

        double distance = std::sqrt( std::pow(target_x_ - pos.x, 2) + std::pow(target_y_ - pos.y, 2));

        if (distance < 0.05) {

          message.linear.x = 0.0;
          message.linear.y = 0.0;
          message.angular.z = 0.0;

          pub_command_->publish(message);

          current_x_ = target_x_;
          current_y_ = target_y_;

          moving_ = false;

          RCLCPP_INFO( this->get_logger(), "Reached grid position (%.0f, %.0f)", current_x_, current_y_);

          return;
      }

      double dx = target_x_ - pos.x;
      double dy = target_y_ - pos.y;

      double desired_theta = std::atan2(dy, dx);

      double angle_error = desired_theta - pos.theta;

      while (angle_error > M_PI) {
        angle_error -= 2.0 * M_PI;
      }

      while (angle_error < -M_PI) {
        angle_error += 2.0 * M_PI;
      }

      if (std::abs(angle_error) > 0.05) {

        message.linear.x = 0.0;
        message.angular.z =
          (angle_error > 0) ? 1.5 : -1.5;

      }
      else {

        message.linear.x = 1.0;
        message.angular.z = 0.0;
      }

      pub_command_->publish(message);

      }
    }

    double current_x_ = 0.0;
    double current_y_ = 0.0;

    double target_x_ = 0.0;
    double target_y_ = 0.0;

    bool moving_ = false;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_position_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_receive_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_command_;
    rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;

    rclcpp::TimerBase::SharedPtr timer_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Turtle>());
  rclcpp::shutdown();
  return 0;
}
