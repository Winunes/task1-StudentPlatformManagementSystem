// server.cpp 服务器主程序，负责 TCP/UDP 服务与学生信息管理
#include "student.hpp"
#include <iostream>      // 标准输入输出流
#include <cstring>       // 字符串处理
#include <ctime>         // 时间相关
#include <unistd.h>      // Unix 标准函数，如 close、fork
#include <arpa/inet.h>   // 网络地址转换
#include <netdb.h>       // 网络数据库操作
#include <sys/socket.h>  // 套接字相关
#include <sys/types.h>
#include <sys/utsname.h> // 系统信息
#include <csignal>       // 信号处理
#include "json.hpp" // 注意不是 <json.hpp>
#include <fstream>

#define TCP_PORT 8888    // TCP 监听端口
#define UDP_PORT 9999    // UDP 监听端口
#define MAX_BUF 1024     // 缓冲区最大长度

using json = nlohmann::json;
// UDP 查询结构体，和 client.cpp 保持一致
struct UdpQuery {
    int type;      // 1: 查询学生，2: 查询系统信息
    char name[50]; // 学生姓名
};

StudentManager manager; // 全局学生信息管理器

void loadStudentsFromJson(const std::string& filename, StudentManager& manager) {
    std::ifstream inFile(filename); // 实例化一个 std::ifstream 类（用于操作文件的输入流类）的对象
    if (!inFile.is_open()) { // 检查当前的文件流对象 inFile 是否成功打开了文件
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }
    json j;
    inFile >> j; // 读取 JSON
    inFile.close(); // 关闭输入文件流
    for (const auto& student : j) {
        std::string name = student["name"];
        int age = student["age"];
        std::string id = student["id"];
        manager.add(name, age, id);
    }
}
// 处理 TCP 客户端连接，发送欢迎信息和系统信息
void handle_tcp(int client_sock) {
    struct utsname sysinfo;
    uname(&sysinfo); // 获取系统信息
    time_t now = time(nullptr);
    std::string time_str = ctime(&now); // 当前时间字符串

    // 构造欢迎消息和菜单
    std::string msg = "【服务器回复】\n系统: " + std::string(sysinfo.sysname) +
                      " " + std::string(sysinfo.release) +
                      "\n时间: " + time_str +
                      "功能菜单：\n1. 查询学生\n2. 查询系统信息\n";

    send(client_sock, msg.c_str(), msg.length(), 0); // 发送消息给客户端
}

// 处理 UDP 查询请求
void handle_udp() {
    int udp_sock; // UDP 套接字描述符
    struct sockaddr_in server_addr{}, client_addr{}; // 服务器和客户端地址结构体
    socklen_t len = sizeof(client_addr); // 客户端地址结构体长度
    char buffer[MAX_BUF]; // 接收数据的缓冲区

    // 1. 创建 UDP socket
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    // 2. 配置本地 UDP 服务器地址
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_port = htons(UDP_PORT);        // 监听端口
    server_addr.sin_addr.s_addr = INADDR_ANY;      // 监听所有网卡
    // 3. 绑定 socket 到本地地址和端口
    bind(udp_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "UDP 服务监听中，端口 " << UDP_PORT << "...\n\n";

    // 4. 循环处理客户端 UDP 查询请求
    while (true) {
        // 4.1 接收客户端 UDP 查询数据，buffer 用于存放收到的内容
        recvfrom(udp_sock, buffer, sizeof(UdpQuery), 0, (sockaddr*)&client_addr, &len);
        // 4.2 将 buffer 转换为 UdpQuery 结构体指针，方便访问查询类型和姓名
        auto* query = reinterpret_cast<UdpQuery*>(buffer);

        std::string reply; // 用于存放服务器回复内容
        if (query->type == 1) { // 查询学生信息
            std::cout << "收到来自 " << inet_ntoa(client_addr.sin_addr) << " 的 UDP 查询学生请求。\n";
            Student* s = manager.search(query->name); // 查找学生
            if (s) {
                // 找到学生，拼接详细信息
                reply = "姓名: " + s->name + "\n年龄: " + std::to_string(s->age) + "\n学号: " + s->id + "\n";
            } else {
                // 未找到学生
                reply = "未找到学生: " + std::string(query->name) + "\n";
            }
        } else if (query->type == 2) { // 查询系统信息
            std::cout << "收到来自 " << inet_ntoa(client_addr.sin_addr) << " 的 UDP 查询系统信息请求。\n";
            struct utsname sysinfo;
            uname(&sysinfo); // 获取系统信息
            time_t now = time(nullptr); // 获取当前时间
            // 拼接系统信息和时间
            reply = "系统: " + std::string(sysinfo.sysname) + " " + std::string(sysinfo.release) +
                    "\n时间: " + std::string(ctime(&now));
        }
        //std::cout << query->type << std::endl;
        // 4.3 通过 sendto 回复客户端查询结果
        sendto(udp_sock, reply.c_str(), reply.length(), 0, (sockaddr*)&client_addr, len);
        std::cout << "回复客户端:\n" << reply << "\n\n";
    }
}

int main() {
    // char cwd[1024];
    // if (getcwd(cwd, sizeof(cwd)) != nullptr) {
    //     std::cout << "当前工作目录: " << cwd << std::endl;
    // }
    // 初始化学生信息
    loadStudentsFromJson("../students.json", manager);  // 读取 JSON 文件，导入学生数据

    // 创建 TCP 套接字并绑定
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in tcp_addr{}, client_addr{};
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_port = htons(TCP_PORT);
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    bind(tcp_sock, (sockaddr*)&tcp_addr, sizeof(tcp_addr));
    listen(tcp_sock, 5); // 开始监听
    std::cout << "TCP 监听中，端口 " << TCP_PORT << "...\n";

    // 创建子进程处理 UDP 服务
    if (fork() == 0) {
        handle_udp();
        exit(0);
    }

    // 主进程循环处理 TCP 客户端
    socklen_t addr_len = sizeof(client_addr);
    while (true) {
        int client_sock = accept(tcp_sock, (sockaddr*)&client_addr, &addr_len); // 接受新连接
        std::cout << "新客户端TCP连接，IP: " << inet_ntoa(client_addr.sin_addr) << "\n";
        handle_tcp(client_sock); // 发送欢迎和菜单
        close(client_sock);      // 关闭连接
        std::cout << "关闭与客户端(" << inet_ntoa(client_addr.sin_addr) << ")的TCP连接。\n\n";
    }
}
