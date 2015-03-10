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
	CategoryItem *item_kinds; //��Ŀ������Ϣ
	CategoryUsers *user_kinds; //�û�������Ϣ

	int *num_item_train;  //��Ŀ�����û���
	double *item_train_average; //��Ŀƽ������

	int *num_user_train;  //�û���ʷ������Ŀ��
	double *user_train_average; //�û�ƽ������
	int **user_item_history;  //�û���ʷ������Ŀ
	double **user_item_history_coef;  //�û���ʷ��Ŀϲ��

	double mark_average; //ȫ��ƽ����
	double *item_train_biases;
	double *user_train_biases;

	double num_user_average; //ƽ���û�������Ŀ��
	double **data_trains;  //ӳ����ѵ��������
	double **data_tests;  //ӳ���Ĳ��Լ�����
};

#endif