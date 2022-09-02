#include <usermodel.h>
#include <iostream>
#include <db.h>

// 如果不将表和表操作封装为两个类，而是封装到一起，那么对于每个表，都要编译额外的操作用代码，量更大。

bool UserModel::insert(User &user)
{
    // 1 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO user(name, password, state) VALUES('%s', '%s', '%s')",
            user.getName().c_str(), user.getPWD().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取新注册用户的主键（id），直接放到user类中
            // mysql_insert_id()是mysql API方法
            // https://baike.baidu.com/item/mysql_insert_id/4350559?fr=aladdin
            user.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM user WHERE id = %d", id);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPWD(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user; //值拷贝返回
            }
        }
    }
    return User();
}

bool UserModel::updateState(User &user)
{
    char sql[1024] = {0};
    sprintf(sql,"UPDATE user SET state = '%s' WHERE id = %d", user.getState().c_str(), user.getId());
    // 注意，sprintf之类函数的%s 不能打印string类型，只能用于C风格字符串，因为其是以\0结尾。
    // sprintf不执行类型检查，打印string就乱码
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return 1;
        }
    }
    return 0;
}

bool UserModel::reset()
{
    char sql[1024] = "UPDATE user SET state = 'offline' WHERE state = 'online'";

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}