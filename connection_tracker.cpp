#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

//运行：ros2 run <功能包> <节点><节点名自定义>
/*get_num_connections() 
发布者调用：返回订阅者数量
订阅者调用：返回发布者数量


在ros2中：
发布者可以调用：get_subscription_count()获得订阅者数量
订阅者可以调用：get_publicsher_count()获得发布者数量

根据不同参数类型（发布者还是订阅者）调用对应函数*/
using std::placeholders::_1;

// NodeUtility 是一个具有静态模板方法的工具类
class NodeUtility
{
public:
  // get_num_connections 是一个静态模板函数，接收任意类型的 node_element
  // 默认它返回 0
  template <typename T>
  static size_t get_num_connections(const T &node_element)
  {
    return 0;
  }
};

// NodeUtility 的模板函数 get_num_connections 为 rclcpp::Publisher<std_msgs::msg::String> 类型的参数进行特化
// 如果传入的参数是 rclcpp::Publisher<std_msgs::msg::String>，则返回其订阅数(subscription_count)
template <>
size_t NodeUtility::get_num_connections<rclcpp::Publisher<std_msgs::msg::String>>(const rclcpp::Publisher<std_msgs::msg::String> &publisher)
{
  return publisher.get_subscription_count();
}

// NodeUtility 的模板函数 get_num_connections 为 rclcpp::Subscription<std_msgs::msg::String> 类型的参数进行特化
// 如果传入的参数是 rclcpp::Subscription<std_msgs::msg::String>，则返回其发布数(publisher_count)
template <>
size_t NodeUtility::get_num_connections<rclcpp::Subscription<std_msgs::msg::String>>(const rclcpp::Subscription<std_msgs::msg::String> &subscription)
{
  return subscription.get_publisher_count();
}

// CustomNode 是一个继承自 rclcpp::Node 的自定义节点类
class CustomNode : public rclcpp::Node
{
public:
  // 在构造函数中，创建发布者(publisher)和订阅者(subscription)，以及定时器(timer)
  CustomNode(std::string name)
      : Node(name)
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
    subscription_ = this->create_subscription<std_msgs::msg::String>(
        "topic", 10, std::bind(&CustomNode::message_callback, this, _1));

    // 设置定时器每500毫秒执行一次 timer_callback 方法
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(500),
        std::bind(&CustomNode::timer_callback, this));
  }

private:
  // 输出收到的信息
  void message_callback(const std_msgs::msg::String::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
  }

  //输出发布者和订阅者的连接数
  // void timer_callback()
  // {
  //   RCLCPP_INFO(this->get_logger(), "Publisher connections: '%zu'", NodeUtility::get_num_connections(*publisher_));
  //   RCLCPP_INFO(this->get_logger(), "Subscriber connections: '%zu'", NodeUtility::get_num_connections(*subscription_));
  // }

  // CustomNode 类的成员变量：定时器、发布者、订阅者
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};
// 主函数，启动节点
int main(int argc, char *argv[])
{
  if (argc == 0)
  {
    return 0;
  }

  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<CustomNode>(argv[1]));//自己传入名字，解决启动多个终端，相同名字

  rclcpp::shutdown();
  return 0;
}
