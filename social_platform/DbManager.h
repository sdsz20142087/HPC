#ifndef _MYDB_H_
#define _MYDB_H_
#include <iostream>
#include <string>
#include "mysql/mysql.h"
#include "proto/user_info_base.pb.h"
#include "UserInfo.h"

using namespace std;

class DbManager{
private:
	GETSETVAR(int,transection)
private:
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
public:
	int Init();
	int initDb(string host,string user,string pswd,string db_name);
	int execSql(string sql);
	int GetUserId();
	int GetUsersBegin();
	int GetUsersOneByOne(UserInfo* user);
	int GetUsersEnd();
	int InsertUser(UserInfo* user);
	int UpdateUser(UserInfo* user);
	int DeleteUser(UserInfo* user);
};

#endif

