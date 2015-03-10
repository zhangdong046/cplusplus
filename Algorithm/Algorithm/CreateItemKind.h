#ifndef CREATEITEMKIND_H_
#define CREATEITEMKIND_H_
#include "ItemCluster.h"
#include "ItemInfo.h"

class CreateItemKind :public ItemInfo
{
public:
	CreateItemKind(int num_u, int num_i, int num_c, const char* path_tr, const char* path_te, const char* item_info);
	~CreateItemKind();

	int Cluster();

private:
	LogInit* path_log;
	char *path_createitemkind_log;

	const char* path_train;
	const char* path_test;

public:
	int num_user;
	int num_cluster; //层次聚类数目
};

#endif