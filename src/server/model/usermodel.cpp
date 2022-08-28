#include<usermodel.h>
#include<iostream>
#include<db.h>

// 如果不将表和表操作封装为两个类，而是封装到一起，那么对于每个表，都要编译额外的操作用代码，量更大。

bool UserModel::insert(User &user)
{
    //1 组装sql语句
    char sql[1024]={0};
    sprintf(sql,"INSERT INTO User(name, password, state,) VALUES('%s', '%s', '%s')",
        user.getName().c_str(),user.getPWD().c_str(),user.getState().c_str());
    
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
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