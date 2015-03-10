#ifndef ITEMCLUSTER_H_
#define ITEMCLUSTER_H_
#include "ItemSimilarity.h"

#define NUM_ITEM_SUM 1682

typedef struct category
{
	int atom[NUM_ITEM_SUM];  //�����Ŀid
	int num;  //�����Ŀ����
}Category;

class ItemCluster
{
public:
	ItemCluster(int num_i, const ItemSimilarity *item_i);
	~ItemCluster();

	int Merge(int step);
	int Cluster(int step, int num_category);

private:
	LogInit* path_log;
	char *path_itemcluster_log;

protected:
	int num_item; //��Ŀ����
	const ItemSimilarity *item_item; 
	double **distance;  //�������

public:
	Category *item_category;  //��������
};


#endif