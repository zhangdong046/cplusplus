#ifndef USERCLUSTER_H_
#define USERCLUSTER_H_
#include "UserSimilarity.h"

#define NUM_USER_SUM 943

typedef struct category_user
{
	int atom[NUM_USER_SUM];  //����û�id
	int num;  //����û�����
	double weight[NUM_USER_SUM];  //���û����϶�

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
	int num_user; //�û�����
	const UserSimilarity *user_user;  //���ƶȾ���
	double **distance;  //�������

public:
	CategoryUser *user_category;  //��������
};


#endif