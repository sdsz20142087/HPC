.proto:all

all:*.proto
	protoc --cpp_out=./ *.proto
	g++ -c -o user_pb.o user_info_base.pb.cc
	g++ -c -o rela_pb.o relation_info_base.pb.cc
	g++ -c -o mess_pb.o message_info_base.pb.cc
	g++ -c -o photo_pb.o photo_info_base.pb.cc
	g++ -c -o mess_def_pb.o message_define.pb.cc
	
proto:
	protoc --cpp_out=./ *.proto
user:
	g++ -c -o user_pb.o user_info_base.pb.cc
rela:
	g++ -c -o rela_pb.o relation_info_base.pb.cc
mess:
	g++ -c -o mess_pb.o message_info_base.pb.cc
photo:
	g++ -c -o photo_pb.o photo_info_base.pb.cc
mess_def:
	g++ -c -o mess_def_pb.o message_define.pb.cc

clean:
	rm *.pb.h *.pb.cc *.o 
