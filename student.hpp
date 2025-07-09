// student.hpp 学生信息管理相关声明
#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>
#include <list>
#include <algorithm>

// 学生信息结构体
struct Student {
    std::string name; // 学生姓名
    int age;          // 学生年龄
    std::string id;   // 学号
};

// 学生信息管理类
class StudentManager {
private:
    std::list<Student> students; // 学生链表容器

public:
    // 添加学生
    // const std::string&：高效传递大对象，避免拷贝。
    // int：小型数据类型，传值即可。
    void add(const std::string& name, int age, const std::string& id);
    // 按姓名删除学生
    void remove(const std::string& name);
    // 按姓名查找学生，返回指针，未找到返回nullptr
    Student* search(const std::string& name);
};

#endif
