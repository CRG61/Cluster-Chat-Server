#ifndef PUBLIC_H
#define PUBLIC_H

enum EnMsgType
{
    LOGIN_MSG = 1,  //√
    REG_MSG,        //√
    P2P_CHAT,       //√
    ADD_FRIEND,     //√ 
    LOGIN_MSG_ACK,  //√
    REG_MSG_ACK,    //√
    LOG_OUT_MSG,    //√

    CREATE_GROUP,   //√
    ADD_GROUP,      //√
    GROUP_ACK_MSG,  //√
    GROUP_CHAT      //√
};

#endif