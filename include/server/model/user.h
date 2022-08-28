#ifndef USER_H
#define USER_H

// 这个头文件用于实现ORM，由于比较简单，没有写cpp
// user表的ORM类,映射表的字段。每一个这个类都可以封装user表中的一行，即一个用户的数据
// user表的实际操作方法在usermodel中
// 这样封装的目的是为了剥离业务层和数据库操作之间的耦合性
// 如此一来业务层得到的也都是对象，而不是数据库字段
#include <string>
using namespace std;

class User
{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }
    void setId(int id) { this->id = id; } //这两个变量同名，因此必须用this指针指明
    void setName(string name) { this->name = name; }
    void setPWD(string pwd) { password = pwd; }//变量不同名，User私有变量password不会被隐藏，不需要用this，用了也是对的
    void setState(string state) { this->state = state; }

    int getId() { return id; }
    string getName(){return name;}
    string getPWD(){return password;}
    string getState(){return state;}


private:
    int id;
    string name;
    string password;
    string state;
};

#endif