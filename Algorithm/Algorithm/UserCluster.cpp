#include "UserCluster.h"

UserCluster::UserCluster(int num_u, const UserSimilarity *user_u) :num_user(num_u), user_user(user_u)
{

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "usercluster.log");
	if (path_log->flag)
		path_usercluster_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化成员变量
	distance = new double *[num_user];
	for (int i = 0; i < num_user; i++)
	{
		distance[i] = new double[num_user];
	}

	user_category = new CategoryUser[num_user];
	for (int i = 0; i < num_user; i++)
		user_category[i] = { { i }, 1, {0.0} };

	FILE *file_usercluster_log;
	if ((fopen_s(&file_usercluster_log, path_usercluster_log, "a")) == 0)
	{
		//初始化距离矩阵
		fprintf(file_usercluster_log, "初始距离矩阵：\n");

		for (int i = 0; i < num_user; i++)
		{
			for (int j = 0; j < num_user; j++)
			{
				if (i < j)
				{
					distance[i][j] = distance[j][i] = user_user->user_user_simi[j][i] = user_user->user_user_simi[i][j];
				}
				else if (i == j)
				{
					distance[i][j] = 0.0;
				}

				fprintf(file_usercluster_log, "%d,%d： %lf\n",i,j,distance[i][j]);
			}
		}
	}
	fclose(file_usercluster_log);

}

UserCluster::~UserCluster()
{
	for (int i = 0; i < num_user; i++)
		delete[]distance[i];
	delete[]distance;

	delete[]user_category;

	delete path_log;
}

int UserCluster::Merge(int step)
{
	double min_distance = 0.0;
	int a = 0;
	int b = 0;

	FILE *file_usercluster_log;
	if ((fopen_s(&file_usercluster_log, path_usercluster_log, "a")) == 0)
	{
		//查找最大相似类组
		for (int i = 0; i < step; i++)
		{
			for (int j = 0; j < step; j++)
			{
				if (distance[i][j] > min_distance)
				{
					min_distance = distance[i][j];

					(i < j) ? (a = i, b = j) : (a = j, b = i);

					if (i == j)
					{
						char tmp[100] = { "data error in min_distance!" };
						throw MyException(2, tmp);
					}
				}
			}
		}

		fprintf(file_usercluster_log, "%d：\n", step);
		fprintf(file_usercluster_log, "a=%d,b=%d： %lf\n", a, b, min_distance);

		if (min_distance == 0.0)
		{
			fclose(file_usercluster_log);
			return 1;
		}

        //合并最小距离类组
		int num_a = user_category[a].num;
		int num_b = user_category[b].num;
		for (int i = 0; i < num_b; i++)
		{
			user_category[a].atom[num_a] = user_category[b].atom[i];
			num_a++;
		}
		user_category[a].num = num_a;

		//重建距离矩阵
		num_a = num_a - num_b;
		for (int i = 0; i < step; i++)
		{
			if (i != a&&i != b)
			{
				distance[a][i] = distance[i][a] = (distance[a][i] * num_a + distance[b][i] * num_b) / (double)(num_a + num_b);
			}
		}

		//距离矩阵调整
		for (int i = 0; i < step; i++)
		{
			for (int j = b; j < step-1; j++)
			{
				distance[i][j] = distance[i][j+1];
			}
		}

		for (int j = 0; j < step-1; j++)
		{
			for (int i = b; i < step - 1; i++)
			{
				distance[i][j] = distance[i+1][j];
			}
		}

		//类组对象调整
		for (int i = b; i < step - 1; i++)
			user_category[i] = user_category[i + 1];
	}
	fclose(file_usercluster_log);

	return 0;
}

int UserCluster::Cluster(int step, int num_category)
{
	while (step > num_category)
	{
		if (Merge(step) == 0)
			step--;
		else
			break;
	}

	FILE *file_usercluster_log;
	if ((fopen_s(&file_usercluster_log, path_usercluster_log, "a")) == 0)
	{
		fprintf(file_usercluster_log, "距离矩阵：\n");
		for (int i = 0; i < step; i++)
		{
			for (int j = 0; j < step; j++)
			{
				fprintf(file_usercluster_log, "%d,%d： %lf\n", i, j, distance[i][j]);
			}
		}

		fprintf(file_usercluster_log, "用户聚类：\n");
		for (int i = 0; i < step; i++)
		{
			fprintf(file_usercluster_log, "%d： ",i);
			int num_tmp = user_category[i].num;

			for (int j = 0; j < num_tmp;j++)
				fprintf(file_usercluster_log, "%d  ", user_category[i].atom[j]);
			fprintf(file_usercluster_log, "\n");
		}

		//计算类用户契合度
		for (int i = 0; i < step; i++)
		{
			int num_tmp = user_category[i].num;
			for (int j = 0; j < num_tmp; j++)
			{
				for (int k = 0; k < num_tmp; k++)
				{
					user_category[i].weight[j] += user_user->user_user_simi[user_category[i].atom[j]][user_category[i].atom[k]];
				}
			}

			double sum_weight = 0.0;
			for (int j = 0; j < num_tmp; j++)
			{
				sum_weight += user_category[i].weight[j];
			}

			fprintf(file_usercluster_log, "\n聚类%d的用户契合度：\n", i);
			for (int j = 0; j < num_tmp; j++)
			{
				if (sum_weight != 0.0)
				{
					user_category[i].weight[j] = user_category[i].weight[j] / sum_weight;
				}
				else
				{
					user_category[i].weight[j] = 1.0;
				}
				
				fprintf(file_usercluster_log, " id:%d|%lf ", user_category[i].atom[j], user_category[i].weight[j]);
			}
		}

	}
	fclose(file_usercluster_log);

	return step;
}