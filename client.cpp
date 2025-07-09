// client.cpp 客户端主程序
#include <iostream>    // 标准输入输出流
#include <unistd.h>    // Unix 标准函数定义，如 close()
#include <arpa/inet.h> // 提供 inet_pton 等网络相关函数
#include <cstring>     // 字符串处理函数
//#include <limits>

#define TCP_PORT 8888      // TCP 目的端口号
#define UDP_PORT 9999      // UDP 目的端口号
#define MAX_BUF 1024       // 缓冲区最大长度
#define SERVER_IP "192.168.43.151"
// UDP 查询结构体
struct UdpQuery {
    int type;         // 查询类型（1：按姓名查询，2：其他功能）
    char name[50];    // 学生姓名
};

int main() {
    // 1. 创建 TCP 套接字
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0); // 创建 TCP socket
    sockaddr_in addr_tcp{}; // TCP 服务器地址结构体
    addr_tcp.sin_family = AF_INET; // IPv4
    addr_tcp.sin_port = htons(TCP_PORT); // 设置端口
    inet_pton(AF_INET, SERVER_IP, &addr_tcp.sin_addr); // 设置 IP 地址

    // 2. 连接到 TCP 服务器
    if(connect(tcp_sock, (sockaddr*)&addr_tcp, sizeof(addr_tcp)) == -1) {
        std::cerr << "连接到服务器失败！请确保服务器正在运行。\n";
        close(tcp_sock); // 关闭 TCP 套接字
        return 1; // 返回错误代码
    } else {
        // sockaddr_in local_addr_tcp{};
        // socklen_t addr_len_tcp = sizeof(local_addr_tcp);
        // getsockname(tcp_sock, (sockaddr*)&local_addr_tcp, &addr_len_tcp);
        // char ip_str[INET_ADDRSTRLEN]; // 用于存放本地 IP 地址字符串
        // inet_ntop(AF_INET, &local_addr_tcp.sin_addr, ip_str, sizeof(ip_str)); // 把 IP 地址从二进制格式转换成字符串
        std::cout << "已连接到服务器" << "\n";
        // std::cout << "源IP: " << ip_str << ", 源端口: " << ntohs(local_addr_tcp.sin_port) << "\n";
        // std::cout << "目的IP: " << inet_ntoa(addr_tcp.sin_addr) << ", 目的端口: " << ntohs(addr_tcp.sin_port) << "\n\n";
    }

    char msg[MAX_BUF] = {};
    // 3. 接收服务器欢迎消息
    recv(tcp_sock, msg, MAX_BUF, 0);
    std::cout << msg << std::endl;
    close(tcp_sock); // 关闭 TCP 套接字
    std::cout << "TCP 连接已关闭，接下来使用 UDP。\n";

    // 4. 创建 UDP 套接字
    int udp_sock = socket(AF_INET, SOCK_DGRAM, 0); // 创建 UDP socket
    sockaddr_in addr_udp{}; // UDP 服务器地址结构体
    addr_udp.sin_family = AF_INET;
    addr_udp.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, SERVER_IP, &addr_udp.sin_addr);

    // 5. 循环进行 UDP 查询
    while (true) {
        int choice;
        std::cout << "请输入功能编号（1或2）：";
        std::cin >> choice; //未进行输入验证（比如输入1 2这种），假设用户输入正确
        if(choice < 1 || choice > 2) {
            std::cout << "输入无效，请重新输入数字。\n\n";
            continue; // 如果输入无效，重新开始循环
        }

        UdpQuery query{};
        query.type = choice; // 设置查询类型
        if (choice == 1) {
            std::cout << "请输入要查询的学生姓名：";
            //std::cin >> query.name; // 输入学生姓名
            std::cin.ignore(64, '\n'); // 清理残留
            std::cin.getline(query.name, sizeof(query.name)); // 支持空格
        }
        std::cout << (choice == 1 ? "正在发送查询...\n" : "正在发送系统信息查询...\n");
        // 发送查询到服务器
        sendto(udp_sock, &query, sizeof(query), 0, (sockaddr*)&addr_udp, sizeof(addr_udp));

        // sockaddr_in local_addr_udp{};
        // socklen_t addr_len_udp = sizeof(local_addr_udp);
        // getsockname(udp_sock, (sockaddr*)&local_addr_udp, &addr_len_udp);
        // char ip_str[INET_ADDRSTRLEN]; // 用于存放本地 IP 地址字符串
        // inet_ntop(AF_INET, &local_addr_udp.sin_addr, ip_str, sizeof(ip_str)); // 把 IP 地址从二进制格式转换成字符串
        // std::cout << "源IP: " << ip_str << ", 源端口: " << ntohs(local_addr_udp.sin_port) << "\n";
        // std::cout << "目的IP: " << inet_ntoa(addr_udp.sin_addr) << ", 目的端口: " << ntohs(addr_udp.sin_port) << "\n";

        // 接收服务器回复
        // 客户端没有用 bind 主动绑定本地IP和端口，所以操作系统会自动分配一个本地IP（通常是本机可用的IP）和一个临时端口。此时：
        // recvfrom 会监听这个自动分配的本地IP和端口，只要有数据包发到这个端口（无论哪个远程IP/端口发来的），都能收到。
        memset(msg, 0, sizeof(msg));
        recvfrom(udp_sock, msg, MAX_BUF, 0, nullptr, nullptr);
        std::cout << msg << std::endl;
    }

    close(udp_sock); // 关闭 UDP 套接字
    return 0;
}
