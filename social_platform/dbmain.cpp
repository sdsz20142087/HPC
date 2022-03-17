#include <stdio.h>
#include "DbManager.h"
using namespace std;
int main(){
	DbManager db_svr;
	db_svr.initDb("127.0.0.1","hank","12341234","ssp");
	db_svr.execSql("truncate tb_user;");
	return 0;
}

