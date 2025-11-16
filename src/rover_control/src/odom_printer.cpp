#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <thread>
#include <iostream>
#include <iomanip>



class OdomPrinter : public rclcpp::Node
{
public:
   OdomPrinter() : Node("odom_printer")
   {
     sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
         "/odom", 10, std::bind(&OdomPrinter::odom_callback, this, std::placeholders::_1));
     pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

     RCLCPP_INFO(this->get_logger(), "Manuel kontrol için WASD kullan! Çıkış: Ctrl+C");
     RCLCPP_INFO(this->get_logger(), "Koordinatlar gerçek zamanlı gösteriliyor...\n");

     last_print_time_ = std::chrono::steady_clock::now();
     teleop_thread_ = std::thread(&OdomPrinter::teleop_loop, this);
   }

  ~OdomPrinter()
  {
    if (teleop_thread_.joinable()) teleop_thread_.join();
  }

private:
   void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
   {
     auto now = std::chrono::steady_clock::now();
     std::chrono::duration<double> elapsed = now - last_print_time_;
     if (elapsed.count() < 0.1) return; // Print at most 10 times per second
     last_print_time_ = now;

     double x = msg->pose.pose.position.x;
     double y = msg->pose.pose.position.y;
     auto q = msg->pose.pose.orientation;
     double yaw = atan2(2.0 * (q.w * q.z + q.x * q.y), 1.0 - 2.0 * (q.y * q.y + q.z * q.z));
     if (yaw < 0) yaw += 2 * M_PI;

     std::cout << "\rX: " << std::fixed << std::setw(6) << std::setprecision(2) << x
               << " | Y: " << std::setw(6) << std::setprecision(2) << y
               << " | Yaw: " << std::setw(5) << std::setprecision(2) << yaw << " rad ("
               << std::setw(6) << std::setprecision(1) << yaw * 180 / M_PI << "°)" << std::flush;
   }

  void teleop_loop()
  {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    geometry_msgs::msg::Twist twist;
    double speed = 0.2, turn = 1.0;
    char key = '\0';

    fd_set set;
    struct timeval timeout;

    while (rclcpp::ok())
    {
      FD_ZERO(&set);
      FD_SET(STDIN_FILENO, &set);
      timeout.tv_sec = 0;
      timeout.tv_usec = 10000; // 10ms timeout

      int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
      if (rv > 0 && FD_ISSET(STDIN_FILENO, &set))
      {
        if (read(STDIN_FILENO, &key, 1) == 1)
        {
          switch (key)
          {
            case 'w': twist.linear.x = speed; twist.angular.z = 0; break;
            case 's': twist.linear.x = -speed; twist.angular.z = 0; break;
            case 'a': twist.linear.x = 0; twist.angular.z = turn; break;
            case 'd': twist.linear.x = 0; twist.angular.z = -turn; break;
            case ' ': twist.linear.x = 0; twist.angular.z = 0; break;
            default: continue;
          }
          pub_->publish(twist);
        }
      }
    }

    twist.linear.x = twist.angular.z = 0;
    pub_->publish(twist);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  }

  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
  std::thread teleop_thread_;
  std::chrono::steady_clock::time_point last_print_time_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<OdomPrinter>());
  rclcpp::shutdown();
  return 0;
}
