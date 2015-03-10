#ifndef USERINFO_H_
#define USERINFO_H_

#include "LogInit.h"

class UserInfo{
public:
	UserInfo(int num_u, const char *path_u);
	~UserInfo();

private:
	const char *path_user_info;  //�û���Ϣ�ļ�
	LogInit *path_log;
	char *path_user_info_log;  //���ݴ�����־

public:
	int num_user; //�û���
	int **user_info;  //ԭʼ�û���Ϣ
};

#endif