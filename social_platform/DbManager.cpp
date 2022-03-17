#include <stdio.h>
#include <string>
#include "common/ret_value.h"
#include "DbManager.h"

char mysql_username[256] = "dongyingzhe";
char mysql_password[256] = "123456";

// int ReadMysqlPassword(){
// 	FILE* f=fopen("mysql.ini","r");
// 	fscanf(f,"%s",mysql_username);
// 	fscanf(f,"%s",mysql_password);
// 	/*printf("mysql_username:%s,mysql_password:%s\n",mysql_username,mysql_password);*/
// 	fclose(f);
// }

int DbManager::Init(){
	int ret=initDb("101.201.75.177","username","password","ssp");
	set_transection(0);
	result=NULL;
	// mysql_query(conn,"set character_set_client = utf8mb4;");
	// mysql_query(conn,"set character_set_connection = utf8mb4;");
	// mysql_query(conn,"set character_set_results = utf8mb4;");
	// mysql_query(conn,"set character_set_server = utf8mb4;");
	// ret = mysql_query(conn,"set character_set_system = utf8mb4;");
	// printf("[]%s\n", mysql_error(conn));
	printf("[INFO    ]DbManager Init:%d\n",ret);
	return ret;
}

int DbManager::initDb(string host,string user,string pswd,string db_name){
	//ReadMysqlPassword();
	conn = mysql_init(NULL);
	if(conn == NULL){
		return DB_CONN_INIT_FAIL;
	}
	conn = mysql_real_connect(conn,host.c_str(),mysql_username,mysql_password,db_name.c_str(),0,NULL,0);
	if(conn == NULL){
		return DB_CONN_CONNECT_FAIL;
	}
	return SUCCESS;
}

int DbManager::execSql(string sql){
	if(mysql_query(conn,sql.c_str())){
		printf("[WARN    ]query fail : %s \n",mysql_error(conn));
		return DB_QUERY_FAIL;
	}else{
		result = mysql_use_result(conn);
		if(result){
			int num_fields_1 = mysql_num_fields(result);
			int num_fields_2 = mysql_field_count(conn);
			int row_count = mysql_num_rows(result);
			printf("[DEBUG   ]row_num %d, fields_num %d, fields_num %d\n",row_count,num_fields_1,num_fields_2);
			while(result){
				row = mysql_fetch_row(result);
				if(row == NULL) {
					printf("[DEBUG   ]empty row\n");
					break;
				}
				for(int j=0;j<num_fields_1;j++){
					printf("%s\t",row[j]);
				}
				printf("\n");

			}
			row_count = mysql_num_rows(result);
			printf("[DEBUG   ]row_num %d, fields_num %d, fields_num %d\n",row_count,num_fields_1,num_fields_2);
		}
		mysql_free_result(result);
	}

	return SUCCESS;
}

int DbManager::GetUsersBegin(){
	if(transection()==1){
		return DB_IS_BUSY;
	}
	set_transection(1);
	int ret=mysql_query(conn,"select * from tb_user;");
	if(ret){
		printf("[WARN    ]query fail : %d %s \n",ret,mysql_error(conn));
		set_transection(0);
		return DB_QUERY_FAIL;
	}
	result = mysql_use_result(conn);
	return SUCCESS;
}

int DbManager::GetUsersOneByOne(UserInfo* user){
	if(result){
		row = mysql_fetch_row(result);
		if(row==NULL){
			printf("[INFO    ]no more user\n");
			return DB_NO_MORE_DATA;
		}
		ssp::UserInfoBase pb_user;
		pb_user.ParseFromArray(row[1],10240);
		printf("[INFO    ]user_id:%d,user_name:%s\n",pb_user.user_id(),pb_user.user_name().c_str());
		user->set_user_id(pb_user.user_id());
		user->set_user_name(pb_user.user_name().c_str());
		user->set_password(pb_user.password().c_str());
	}else{
		return DB_NO_MORE_DATA;
	}
	return SUCCESS;
}

int DbManager::GetUsersEnd(){
	mysql_free_result(result);
	set_transection(0);
	return SUCCESS;
}
int DbManager::UpdateUser(UserInfo* user){
	return SUCCESS;
}
int DbManager::DeleteUser(UserInfo* user){
	return SUCCESS;
}
int DbManager::InsertUser(UserInfo* user){
	ssp::UserInfoBase pb_user;
	pb_user.set_ver(1);
	pb_user.set_user_id(user->user_id());
	pb_user.set_user_name(user->user_name());
	pb_user.set_password(user->password());
	char data[10245], data2[20490];
	printf("ver = %d, user_id = %d, user_name = %s, password = %s\n", pb_user.ver(),
	 pb_user.user_id(), pb_user.user_name().c_str(), pb_user.password().c_str());
	pb_user.SerializeToArray(data,pb_user.ByteSize());
	printf("debug %s %d\n", data, pb_user.ByteSize());
	printf("=====\n");
	for(int i=0;i<pb_user.ByteSize();i++) printf("%2x ", data[i]);
	cout << endl;
	for(int i=0;i<pb_user.ByteSize();i++){
		data2[2*i]=data[i]/16+'a';
		data2[2*i+1]=data[i]%16+'a';
	}
	for(int i=0;i<strlen(data2);i++) printf("%2x ", data2[i]);
	cout << endl;
	printf("debug %s %d\n", data2, pb_user.ByteSize());
	printf("=====\n");
	char user_id[256];
	sprintf(user_id,"%d",user->user_id());
	string insertSql="insert into tb_user values(";
	insertSql+=user_id;
	insertSql+=",'";
	insertSql+=data2;
	insertSql+="');";
	printf("[DEBUG   ]insertSql:%s\n",insertSql.c_str());
	for(int i=0;i*2< strlen(data2);i++){
		data[i]=(data2[2*i]-'a')*16+(data2[2*i+1]-'a');
	}
	printf("debug %s %d\n", data, pb_user.ByteSize());
	pb_user.ParseFromArray(data,10240);
	printf("final, %d\n", pb_user.user_id());
	int ret=mysql_query(conn,insertSql.c_str());
	if(ret==0){
		return SUCCESS;
	}else{
		printf("[WARN    ]insert fail : %d %s \n",ret,mysql_error(conn));
		return DB_QUERY_FAIL;
	}	
	return SUCCESS;
}

int DbManager::GetUserId(){
	int used_user_id=10000;
	int ret=mysql_query(conn,"select value from tb_var where key_str='user_id';");
	if(ret){
		printf("[WARN    ]query user_id fail : %d %s \n",ret,mysql_error(conn));
		return used_user_id;
	}else{
		result = mysql_use_result(conn);
		if(result){
			row = mysql_fetch_row(result);
			if(row == NULL) {
				printf("[DEBUG   ]query user_id empty\n");
			}
			sscanf(row[0],"%d",&used_user_id);
		}
		mysql_free_result(result);
	}
	return used_user_id;
}

