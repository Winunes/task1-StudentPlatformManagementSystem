// student.cpp 学生信息管理相关实现
#include "student.hpp"

// 添加学生信息到链表
void StudentManager::add(const std::string& name, int age, const std::string& id) {
    students.push_back({name, age, id});
}

// 按姓名删除学生
void StudentManager::remove(const std::string& name) {
    // [&] 按引用捕获所有在 lambda 体内使用的外部变量
    students.remove_if([&](const Student& s) { return s.name == name; });
}

// 按姓名查找学生，找到返回指针，否则返回nullptr
Student* StudentManager::search(const std::string& name) {
    for (auto& s : students) {
        if (s.name == name) return &s;
    }
    return nullptr;
}
