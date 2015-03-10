#include "UserSimilarity.h"

UserSimilarity::UserSimilarity(int num_u, int num_i, const char *path_tr, const char *path_te) :FileInit(num_u, num_i, path_tr, path_te)
{
	//初始化日志文件
	path_log = new LogInit("..\\log\\", "usersimi.log");
	if (path_log->flag)
		path_usersimi_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化成员变量
	num_item_user_list = new int[num_item];
	for (int j = 0; j < num_item; j++)
		num_item_user_list[j] = 0;

	user_user_simi = new double *[num_user];
	for (int i = 0; i < num_user; i++)
		user_user_simi[i] = new double[num_user];
	for (int i = 0; i < num_user;i++)
	for (int j = 0; j < num_user; j++)
		user_user_simi[i][j] = 0.0;

	//初始化辅助数组
	double *user_train_average = new double[num_user];
	for (int i = 0; i < num_user; i++)
		user_train_average[i] = 0.0;

	FILE *file_usersimi_log;
	if ((fopen_s(&file_usersimi_log, path_usersimi_log, "a")) == 0)
	{
		//求取用户平均评分
		int num_user_rating;
		fprintf(file_usersimi_log, "用户平均评分：\n");
	    for (int i = 0; i < num_user; i++)
	    {
		    num_user_rating = 0;
		    for (int j = 0; j < num_item; j++)
		    {
			    if (data_train[i*num_item + j] > 1e-10)
			    {
					num_user_rating++;
					user_train_average[i] += data_train[i*num_item + j];
			    }
		    }

			if (num_user_rating != 0)
				user_train_average[i] = user_train_average[i] / (double)num_user_rating;
			else
				user_train_average[i] = 0.0;
			fprintf(file_usersimi_log, "%d: %lf\n", i, user_train_average[i]);
	    }

		//初始化项目-用户倒排表
		for (int j = 0; j < num_item; j++)
		{
			for (int i = 0; i < num_user; i++)
			{
				if (user_train_average[i] > 1e-10)
				{
					if (data_train[i*num_item + j] - user_train_average[i] > 1e-10)
					{
						num_item_user_list[j]++;
					}
				}
			}
		}

		item_user_list = new int *[num_item];
		for (int j = 0; j < num_item; j++)
		if (num_item_user_list[j] != 0)
			item_user_list[j] = new int[num_item_user_list[j]];
		else
			item_user_list[j] = new int[1];

		for (int i = 0; i < num_item;i++)
		for (int j = 0; j < num_item_user_list[i]; j++)
			item_user_list[i][j] = 0;

		//项目-用户倒排表
		fprintf(file_usersimi_log, "项目-用户倒排表：\n");
		int user_count;
		for (int j = 0; j < num_item; j++)
		{
			user_count = 0;
			fprintf(file_usersimi_log, "\n项目%d:", j);
			for (int i = 0; i < num_user; i++)
			{
				if (user_train_average[i] > 1e-10)
				{
					if (data_train[i*num_item + j] - user_train_average[i] > 1e-10)
					{
						item_user_list[j][user_count] = i;
						fprintf(file_usersimi_log, "%d,", item_user_list[j][user_count]);
						user_count++;
					}
				}
			}
		}

    }
	fclose(file_usersimi_log);

	delete[]user_train_average;
}

UserSimilarity::~UserSimilarity()
{
	delete[]num_item_user_list;

	for (int i = 0; i < num_user; i++)
	{
		delete[]user_user_simi[i];
	}
	delete[]user_user_simi;

	for (int i = 0; i < num_item; i++)
	{
		delete[]item_user_list[i];
	}
	delete[]item_user_list;

	delete path_log;
}

void UserSimilarity::GetSimiMatrix()
{
	//初始化用户正反馈物品数数组
	int *N = new int[num_user];
	for (int i = 0; i < num_user; i++)
		N[i] = 0;

	double **C = new double*[num_user];
	for (int i = 0; i < num_user; i++)
		C[i] = new double[num_user];
	for (int i = 0; i < num_user;i++)
	for (int j = 0; j < num_user;j++)
		C[i][j] = 0.0;
	
	//计算用户相似度矩阵
	for (int j = 0; j < num_item; j++)
	{
		for (int s = 0; s < num_item_user_list[j]; s++)
		{
			N[item_user_list[j][s]] += 1;
			for (int t = s + 1; t < num_item_user_list[j]; t++)
			{
				C[item_user_list[j][s]][item_user_list[j][t]] += 1 / log(1 + num_item_user_list[j]);
			}
		}
	}

	FILE *file_usersimi_log;
	if ((fopen_s(&file_usersimi_log, path_usersimi_log, "a")) == 0)
	{
		fprintf(file_usersimi_log, "\n用户相似度矩阵：\n");
		for (int i = 0; i < num_user; i++)
		{
			for (int j = i + 1; j < num_user; j++)
			{
				if ((N[i] != 0) && (N[j]!= 0))
				{
					user_user_simi[i][j] = C[i][j] / sqrt(N[i] * N[j]);
				}
				else
				{
					user_user_simi[i][j] = 0.0;
				}
				fprintf(file_usersimi_log, "%d,%d：%lf\n",i,j,user_user_simi[i][j]);
			}
		}
	}
	fclose(file_usersimi_log);

	delete[]N;
	for (int i = 0; i < num_user; i++)
	{
		delete[]C[i];
	}
	delete[]C;
}