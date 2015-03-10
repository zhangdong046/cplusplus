#ifndef USERINFO_H_
#define USERINFO_H_

#include "LogInit.h"

class UserInfo{
public:
	UserInfo(int num_u, const char *path_u);
	~UserInfo();

private:
	const char *path_user_info;  //用户信息文件
	LogInit *path_log;
	char *path_user_info_log;  //数据处理日志

public:
	int num_user; //用户数
	int **user_info;  //原始用户信息
};

#endif