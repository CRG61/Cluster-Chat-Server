#include "friendmodel.h"
#include "db.h"

bool FriendModel::insert(int userid, int friendid)
{
    //暂未实现需要验证的加好友过程
    MySQL mysql;
    if (mysql.connect())
    {
        char sql[1024] = {0};
        sprintf(sql, "INSERT INTO Friend (userid,friendid) VALUES (%d, %d)", userid, friendid);
        mysql.update(sql);
        return 1;
    }
    return 0;
}

vector<User> FriendModel::query(int userid)
{
    // 查询并返回所有好友信息
    vector<User> ret;

    MySQL mysql;
    MYSQL_RES *res;
    if (mysql.connect())
    {
        char sql[1024] = {0};
        sprintf(sql, "SELECT a.id,a.name,a.state FROM user a INNER JOIN Friend b ON b.friendid = a.id WHERE b.userid = %d", userid);
        if (res = mysql.query(sql))
        {
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                ret.push_back(user);
            }
        }
        mysql_free_result(res);
    }

    return ret;
}

bool FriendModel::remove(int userid, int friendid)
{
}