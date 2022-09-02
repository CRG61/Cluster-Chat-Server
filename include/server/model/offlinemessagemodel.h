#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <vector>
#include <string>
using namespace std;

//插入+查询+删除
class OfflineMessage
{
public:
    void insert(int userid, string msg);
    void remove(int userid);
    vector<string> query(int userid);
};

#endif