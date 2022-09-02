#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "user.h"
using namespace std;

// 继承自user,因此只需要额外一个role字段就行
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return role; }

private:
    string role;
};

#endif