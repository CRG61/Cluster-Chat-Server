#include "offlinemessagemodel.h"
#include "db.h"

void OfflineMessage::insert(int userid, string msg)
{
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO OfflineMessage(userid,message) VALUES(%d,'%s')", userid, msg.c_str());
    // sprintf(sql, "INSERT INTO OfflineMessage VALUES(%d,'%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
void OfflineMessage::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "DELETE FROM OfflineMessage WHERE userid = %d", userid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
vector<string> OfflineMessage::query(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "SELECT message FROM OfflineMessage WHERE userid = %d", userid);
    vector<string> ret;

    MySQL mysql;
    if (mysql.connect())
    {
        if (MYSQL_RES *res = mysql.query(sql))
        {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res))
            {
                ret.push_back(row[0]);
            }
            mysql_free_result(res);
        }
        }
    return ret;
}