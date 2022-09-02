#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.h"
#include "db.h"

class GroupModel
{
public:
    // 创建群 自动设置role = 'creator'
    bool createGroup(Group &group, int userid);
    // 加入群 role不可以为'creator'
    void addGroup(int userid, int groupid, string role);
    //退出群(自实现)  如果是群主退群，需要设计群主的转让逻辑
    void quitGroup(int userid, int groupid);
    // 查询用户所在群的信息
    vector<Group> queryGroups(int userid);
    // 根据groupid查询成员信息，除了userid自己。用于群发消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif