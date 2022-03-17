.SSP:all 

# protobuf
INC_PROTOBUF = -I/usr/local/include/google/protobuf
LIB_PROTOBUF = -L/usr/local/lib/libprotobuf.a -lprotobuf
OBJ_SSP = proto/*.o

# mysql
LIB_MYSQL = -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto

all:user rela mess photo svr #cln

user:UserManager.cpp
	g++ -c -o user.o $^

rela:RelationManager.cpp
	g++ -c -o relaI.o RelationInfo.cpp
	g++ -c -o realM.o RelationManager.cpp

mess:MessageManager.cpp
	g++ -c -o mess.o $^

photo:PhotoManager.cpp
	g++ -c -o photo.o $^

db:DbManager.cpp
	g++ -c -o db.o $^

svr:main.cpp UserManager.cpp RelationManager.cpp RelationInfo.cpp MessageManager.cpp PhotoManager.cpp DbManager.cpp 
	g++ -o server_ssp $^ $(OBJ_SSP) $(INC_PROTOBUF) $(LIB_PROTOBUF) $(LIB_MYSQL)

dbt:dbmain.cpp
	g++ -c -o db.o DbManager.cpp
	g++ -o dbtest db.o $^ $(OBJ_SSP) $(INC_PROTOBUF) $(LIB_PROTOBUF) $(LIB_MYSQL)

cln:client.cpp
	g++ -o client_ssp $^ $(OBJ_SSP) $(LIB_PROTOBUF)

.SSP:clean
clean:
	rm -f *.o server_ssp client_ssp

