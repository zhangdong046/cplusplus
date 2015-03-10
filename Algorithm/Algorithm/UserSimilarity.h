#ifndef USERSIMILARUTY_H_
#define USERSIMILARUTY_H_

#include "FileInit.h"

class UserSimilarity :public FileInit{
public:
	UserSimilarity(int num_u, int num_i, const char *path_tr, const char *path_te);
	~UserSimilarity();
	
	void GetSimiMatrix();

private:
	LogInit* path_log;
	char *path_usersimi_log; //数据处理日志
	int *num_item_user_list; //项目兴趣用户数
	int **item_user_list; //项目-用户倒排表

public:
	double **user_user_simi; //用户相似度矩阵
};
#endif