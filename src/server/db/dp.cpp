#include <db.h>

MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败!";
        return false;
    }
    return true;
}

bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        // C和C++代码默认编码字符是ASCII，如果不设置，从Mysql拉下来的中文会乱码
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "successfully connected to mysql server.";
    }
    else
    {
        LOG_INFO << "mysql server connection failed.";
    }
    return p;
}

 MYSQL* MySQL::getConnection(){return _conn;}