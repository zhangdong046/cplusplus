#ifndef ITEMCLUSTER_H_
#define ITEMCLUSTER_H_
#include "ItemSimilarity.h"

#define NUM_ITEM_SUM 1682

typedef struct category
{
	int atom[NUM_ITEM_SUM];  //类别项目id
	int num;  //类别项目个数
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
	int num_item; //项目数量
	const ItemSimilarity *item_item; 
	double **distance;  //距离矩阵

public:
	Category *item_category;  //类组数组
};


#endif