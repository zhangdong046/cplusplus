#ifndef ITEMSIMILARUTY_H_
#define ITEMSIMILARUTY_H_

#include "FileInit.h"

class ItemSimilarity :public FileInit{
public:
	ItemSimilarity(int num_u, int num_i, const char *path_tr, const char *path_te);
	~ItemSimilarity();

	void GetSimiMatrix();

private:
	LogInit* path_log;
	char *path_itemsimi_log; 
	int *num_user_item_list; //用户兴趣项目数
	int **user_item_list; //用户-项目倒排表

public:
	double **item_item_simi; //项目相似度矩阵
};
#endif