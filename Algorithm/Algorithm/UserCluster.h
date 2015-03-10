#ifndef USERCLUSTER_H_
#define USERCLUSTER_H_
#include "UserSimilarity.h"

#define NUM_USER_SUM 943

typedef struct category_user
{
	int atom[NUM_USER_SUM];  //类别用户id
	int num;  //类别用户个数
	double weight[NUM_USER_SUM];  //类用户契合度

}CategoryUser;

class UserCluster
{
public:
	UserCluster(int num_u, const UserSimilarity *user_u);
	~UserCluster();

	int Merge(int step);
	int Cluster(int step, int num_category);

private:
	LogInit* path_log;
	char *path_usercluster_log; 

protected:
	int num_user; //用户数量
	const UserSimilarity *user_user;  //相似度矩阵
	double **distance;  //距离矩阵

public:
	CategoryUser *user_category;  //类组数组
};


#endif