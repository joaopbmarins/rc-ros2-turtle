#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <iostream>
#include <thread>
#include <mutex>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class Send_msg : public rclcpp::Node
{
public:
    Send_msg() : Node("send_msg")
    {
        publisher_ = this->create_publisher<std_msgs::msg::String>("/turtle_control", 10);
        timer_ = this->create_wall_timer(500ms, std::bind(&Send_msg::timer_callback, this));

        thread_teclado_ = std::thread(&Send_msg::read_keyboard, this);
    }
    ~Send_msg()
    {
        if (thread_teclado_.joinable())
        {
            thread_teclado_.detach();
        }
    }

private:
    void read_keyboard()
    {
        std::string input;
        while (rclcpp::ok())
        {
            std::cout << "Digite um comando para o ROS2: ";
            std::cin >> input;

            {
                std::lock_guard<std::mutex> lock(mutex_dados_);
                comando_atual_ = input;
            }
        }
    }

    void timer_callback()
    {
        std_msgs::msg::String message;
        {
            std::lock_guard<std::mutex> lock(mutex_dados_);
            message.data = comando_atual_;
            comando_atual_ = "";
        }

        if (!message.data.empty())
        {
            RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
            publisher_->publish(message);
        }
    }
    std::thread thread_teclado_;
    std::string comando_atual_;
    std::mutex mutex_dados_;

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Send_msg>());
    rclcpp::shutdown();
    return 0;
}
