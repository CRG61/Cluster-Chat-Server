#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include <vector>
#include <user.h>

class FriendModel
{
public:
    // 添加、删除（未实现）
    bool insert(int userid, int friendid);
    vector<User> query(int userid);
    bool remove(int userid, int friendid);
};

#endif