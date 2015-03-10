#ifndef CLUSTERSVDPPBASE_H_
#define CLUSTERSVDPPBASE_H_
#include "FileInit.h"
#include "CreateItemKind.h"
#include "UserCluster.h"

typedef struct category_item
{
	int start;
	int end;
	int num;
}CategoryItem;

typedef struct category_users
{
	int id;
	int num;
	int atom[NUM_USER_SUM];  
	double weight[NUM_USER_SUM];  
}CategoryUsers;

class ClusterSvdPPBase :public FileInit
{
public:
	ClusterSvdPPBase(int num_u, int num_i, const char *path_tr, const char *path_te);
	~ClusterSvdPPBase();

private:
	LogInit* path_log;
	char *path_clustersvdbase_log;

public:
	CategoryItem *item_kinds; //项目分类信息
	CategoryUsers *user_kinds; //用户聚类信息

	int *num_item_train;  //项目评分用户数
	double *item_train_average; //项目平均评分

	int *num_user_train;  //用户历史评分项目数
	double *user_train_average; //用户平均评分
	int **user_item_history;  //用户历史评分项目
	double **user_item_history_coef;  //用户历史项目喜好

	double mark_average; //全局平均分
	double *item_train_biases;
	double *user_train_biases;

	double num_user_average; //平均用户评分项目数
	double **data_trains;  //映射后的训练集数据
	double **data_tests;  //映射后的测试集数据
};

#endif