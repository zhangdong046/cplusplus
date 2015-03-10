#include "ItemSimilarity.h"

ItemSimilarity::ItemSimilarity(int num_u, int num_i, const char *path_tr, const char *path_te) :FileInit(num_u, num_i, path_tr, path_te)
{
	//初始化成员变量
	num_user_item_list = new int[num_user];
	for (int i = 0; i < num_user; i++)
		num_user_item_list[i] = 0;

	item_item_simi = new double *[num_item];
	for (int j = 0; j < num_item; j++)
		item_item_simi[j] = new double[num_item];
	for (int i = 0; i < num_item; i++)
	for (int j = 0; j < num_item; j++)
		item_item_simi[i][j] = 0.0;

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "itemsimi.log");
	if (path_log->flag)
		path_itemsimi_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化辅助数组
	double *item_train_average = new double[num_item];
	for (int j = 0; j < num_item; j++)
		item_train_average[j] = 0.0;

	FILE *file_itemsimi_log;
	if ((fopen_s(&file_itemsimi_log, path_itemsimi_log, "a")) == 0)
	{
		//求取项目平均评分
		int num_item_rating;
		fprintf(file_itemsimi_log, "项目平均评分：\n");
		for (int j = 0; j < num_item; j++)
		{
			num_item_rating = 0;
			for (int i = 0; i < num_user; i++)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					num_item_rating++;
					item_train_average[j] += data_train[i*num_item + j];
				}
			}

			if (num_item_rating != 0)
				item_train_average[j] = item_train_average[j] / (double)num_item_rating;
			else
				item_train_average[j] = 0.0;
			fprintf(file_itemsimi_log, "%d: %lf\n", j, item_train_average[j]);
		}

		//初始化用户-项目倒排表
		for (int i = 0; i < num_user; i++)
		{
			for (int j = 0; j < num_item; j++)
			{
				if (item_train_average[j] > 1e-10)
				{
					if (data_train[i*num_item + j] - item_train_average[j] > 1e-10)
					{
						num_user_item_list[i]++;
					}
				}
			}
		}

		user_item_list = new int *[num_user];
		for (int i = 0; i < num_user; i++)
		if (num_user_item_list[i] != 0)
			user_item_list[i] = new int[num_user_item_list[i]];
		else
			user_item_list[i] = new int[1];

		for (int i = 0; i < num_user; i++)
		for (int j = 0; j < num_user_item_list[i]; j++)
			user_item_list[i][j] = 0;

		//用户-项目倒排表
		fprintf(file_itemsimi_log, "用户-项目倒排表：\n");
		int item_count;
		for (int i = 0; i < num_user; i++)
		{
			item_count = 0;
			fprintf(file_itemsimi_log, "\n用户%d:", i);
			for (int j = 0; j < num_item; j++)
			{
				if (item_train_average[j] > 1e-10)
				{
					if (data_train[i*num_item + j] - item_train_average[j] > 1e-10)
					{
						user_item_list[i][item_count] = j;
						fprintf(file_itemsimi_log, "%d,", user_item_list[i][item_count]);
						item_count++;
					}
				}
			}
		}

	}
	fclose(file_itemsimi_log);

	delete[]item_train_average;
}

ItemSimilarity::~ItemSimilarity()
{
	delete[]num_user_item_list;

	for (int i = 0; i < num_item; i++)
	{
		delete[]item_item_simi[i];
	}
	delete[]item_item_simi;

	for (int i = 0; i < num_user; i++)
	{
		delete[]user_item_list[i];
	}
	delete[]user_item_list;

	delete path_log;
}

void ItemSimilarity::GetSimiMatrix()
{
	int *N = new int[num_item];
	for (int j = 0; j < num_item; j++)
		N[j] = 0;

	double **C = new double*[num_item];
	for (int j = 0; j < num_item; j++)
		C[j] = new double[num_item];
	for (int i = 0; i < num_item; i++)
	for (int j = 0; j < num_item; j++)
		C[i][j] = 0.0;

	//计算项目相似度矩阵
	for (int i = 0; i < num_user; i++)
	{
		for (int s = 0; s < num_user_item_list[i]; s++)
		{
			N[user_item_list[i][s]] += 1;
			for (int t = 0; t < num_user_item_list[i]; t++)
			{
				if (t != s)
				{
					C[user_item_list[i][s]][user_item_list[i][t]] += 1 / log(1 + num_user_item_list[i]);
				}
			}
		}
	}

	FILE *file_itemsimi_log;
	if ((fopen_s(&file_itemsimi_log, path_itemsimi_log, "a")) == 0)
	{
		fprintf(file_itemsimi_log, "\n项目相似度矩阵：\n");
		for (int i = 0; i < num_item; i++)
		{
			for (int j = 0; j < num_item; j++)
			{
				if (i == j)
				{
					item_item_simi[i][j] = 0.0;
				}
				else if ((N[i] != 0) && (N[j] != 0))
				{
					item_item_simi[i][j] = C[i][j] / sqrt(N[i] * N[j]);
				}
				else
				{
					item_item_simi[i][j] = 0.0;
				}
				fprintf(file_itemsimi_log, "%d,%d：%lf\n", i, j, item_item_simi[i][j]);
			}
		}
	}
	fclose(file_itemsimi_log);

	delete[]N;
	for (int j = 0; j < num_item; j++)
	{
		delete[]C[j];
	}
	delete[]C;
}