#ifndef GROUP_H
#define GROUP_H
#include <string>
#include <vector>
#include "groupuser.h"
using namespace std;

// 对应AllGroup表
class Group
{
public:
    Group(int id = -1, string groupName = "", string groupDesc = "")
    {
        groupId = id;
        this->groupName = groupName;
        this->groupDesc = groupDesc;
    }

    void setGId(int id) { groupId = id; }
    void setGName(string name) { groupName = name; }
    void setGDesc(string desc) { groupDesc = desc; }

    int getGID() { return groupId; }
    string getGName() { return groupName; }
    string getGDesc() { return groupDesc; }
    // 传引用的作用就在于可以直接往返回的vector里面添加值
    vector<GroupUser> &getGUsers() { return gUsers; }

private:
    int groupId;
    string groupName;
    string groupDesc;
    vector<GroupUser> gUsers;
};

#endif