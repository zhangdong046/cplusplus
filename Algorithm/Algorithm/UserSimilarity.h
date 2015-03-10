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
	char *path_usersimi_log; //���ݴ�����־
	int *num_item_user_list; //��Ŀ��Ȥ�û���
	int **item_user_list; //��Ŀ-�û����ű�

public:
	double **user_user_simi; //�û����ƶȾ���
};
#endif