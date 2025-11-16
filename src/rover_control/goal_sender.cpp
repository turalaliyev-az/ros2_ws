#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <thread>

class GoalSender : public rclcpp::Node
{
public:
  GoalSender() : Node("goal_sender")
  {
    pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/goal_pose", 10);
    RCLCPP_INFO(this->get_logger(), "Koordinatla robotu gönder! Örnek: ros2 run rover_control goal_sender 1.0 2.0 0.0");
  }

  void send_goal(double x, double y, double yaw)
  {
    auto msg = geometry_msgs::msg::PoseStamped();
    msg.header.frame_id = "map";
    msg.header.stamp = this->now();
    msg.pose.position.x = x;
    msg.pose.position.y = y;
    msg.pose.position.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0, 0, yaw);
    msg.pose.orientation.x = q.x();
    msg.pose.orientation.y = q.y();
    msg.pose.orientation.z = q.z();
    msg.pose.orientation.w = q.w();

    pub_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "Hedef gönderildi: x=%.2f, y=%.2f, yaw=%.2f rad", x, y, yaw);
  }

private:
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GoalSender>();

  if (argc != 4)
  {
    RCLCPP_ERROR(node->get_logger(), "Kullanım: ros2 run rover_control goal_sender <x> <y> <yaw>");
    RCLCPP_INFO(node->get_logger(), "Örnek: ros2 run rover_control goal_sender 1.0 2.0 1.57");
    return 1;
  }

  double x = std::atof(argv[1]);
  double y = std::atof(argv[2]);
  double yaw = std::atof(argv[3]);

  node->send_goal(x, y, yaw);
  rclcpp::spin_some(node);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  rclcpp::shutdown();
  return 0;
}