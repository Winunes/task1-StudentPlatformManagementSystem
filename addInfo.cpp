#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

void addStudentToJson(const std::string& filename, const std::string& name, int age, const std::string& id) {
    json j;
    std::ifstream inFile(filename); // 实例化一个 std::ifstream 类（用于操作文件的输入流类）的对象
    if (inFile.is_open()) {
        inFile >> j; // 读取现有的 JSON 数据
        inFile.close(); // 关闭输入文件流
    } else {
        j = json::array(); // 如果文件不存在或无法打开，初始化为一个空的 JSON 数组
    }

    json newStudent = {
        {"name", name},
        {"age", age},
        {"id", id}
    };

    j.push_back(newStudent); // 将新学生信息添加到 JSON 数组中

    std::ofstream outFile(filename); // 实例化一个 std::ofstream 类（用于操作文件的输出流类）的对象
    if (!outFile.is_open()) {
        std::cerr << "无法打开文件写入: " << filename << std::endl;
        return;
    }
    outFile << j.dump(4); // 将 JSON 数据写入文件，使用 4 个空格缩进格式化输出
    outFile.close();

    std::cout << "成功添加学生：" << name << std::endl;
}

int main() {
    std::string filename = "../students.json";

    while (true) {
        std::string name, id;
        int age;

        std::cout << "请输入学生姓名（输入exit退出）：";
        std::getline(std::cin, name);
        if (name == "exit") break;

        std::cout << "请输入年龄：";
        std::cin >> age;
        std::cin.ignore(64, '\n'); // 清理输入缓冲区

        std::cout << "请输入学号：";
        std::getline(std::cin, id);

        addStudentToJson(filename, name, age, id);

        std::cout << std::endl;
    }

    std::cout << "程序结束，所有学生信息已保存到 " << filename << std::endl;
    return 0;
}
