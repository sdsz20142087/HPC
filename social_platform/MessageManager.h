#include "common/macro.h"
#include "MessageInfo.h"

class MessageManager {
private:
	MessageInfo messages_[256];
	GETSETVAR(int,message_count)

public:
	void Start();
	void Proc();
	void Shutdown();
	void Restart();

public:
	MessageInfo* GetMessage(int user_id,int message_id);
	MessageInfo* GetMessage(int message_id);
	int PublishMessage(MessageInfo message);
	int DeleteMessage(int message_id);
};

