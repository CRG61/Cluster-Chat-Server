#include "groupmodel.h"

bool GroupModel::createGroup(Group &group, int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO AllGroup (groupname,groupdesc) VALUES ('%s','%s')", group.getGName().c_str(), group.getGDesc().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 虽然不懂，但是可以直接获得id
            group.setGId(mysql_insert_id(mysql.getConnection()));
        }
        char sql2[1024] = {0};
        sprintf(sql2, "INSERT INTO GroupUser (groupid,userid,grouprole) VALUES (%d,%d,'%s')", group.getGID(), userid, 'creator');
        if (mysql.update(sql2))
            return true;
    }
    return false;
}

void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 按照QQ的方式，加群默认是普通群员就行，不过这里还是写上好了
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO GroupUser (userid,groupid,grouprole) VALUES (%d,%d,'%s')", userid, groupid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

void GroupModel::quitGroup(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM GroupUser WHERE userid = %d AND groupid = %d", userid, groupid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询该用户在哪些群里面
vector<Group> GroupModel::queryGroups(int userid)
{
    vector<Group> ret;
    char sql[1024] = {0};
    sprintf(sql, "SELECT a.groupid, a.grouprole, b.groupname, b.groupdesc \
    FROM GroupUser a INNER JOIN AllGroup b ON a.groupid = b.groupid WHERE a.userid = %d", userid);

    MySQL mysql;
    if (mysql.connect())  // 这个connect会一直有效至出函数作用域
    {
        if (MYSQL_RES *res = mysql.query(sql))
        {
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                Group group;
                group.setGId(atoi(row[0]));
                group.setGName(row[2]);
                group.setGDesc(row[3]);
                ret.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询群组用户信息
    for(Group group : ret)
    {
        sprintf(sql,"SELECT a.id,a.name,a.state,b.grouprole FROM user a \
        INNER JOIN GroupUser b ON b.userid = a.id WHERE b.groupid = %d",group.getGID());

        if(MYSQL_RES *res = mysql.query(sql))
        {
            while(MYSQL_ROW row = mysql_fetch_row(res))
            {
                GroupUser Guser;
                Guser.setId(atoi(row[0]));
                Guser.setName(row[1]);
                Guser.setRole(row[3]);
                Guser.setState(row[2]);
                group.getGUsers().push_back(Guser);
            }
            mysql_free_result(res);
        }
    }
    return ret;
}

// 用于群聊，返回特定群里的用户id，除了输入的userid之外
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    vector<int> ret;
    char sql[1024] = {0};
    sprintf(sql, "SELECT userid FROM GroupUser WHERE groupid = %d AND userid != %d", groupid,userid);

    MySQL mysql;
    if (mysql.connect())
    {
        if (MYSQL_RES *res = mysql.query(sql))
        {
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                if (atoi(row[0]) != userid)
                {
                    ret.push_back(atoi(row[0]));
                }
            }
            mysql_free_result(res);
        }
    }
    return ret;
}