#include "ItemCluster.h"

ItemCluster::ItemCluster(int num_i, const ItemSimilarity *item_i) :num_item(num_i), item_item(item_i)
{

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "itemcluster.log");
	if (path_log->flag)
		path_itemcluster_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化成员变量
	distance = new double *[num_item];
	for (int j = 0; j < num_item; j++)
	{
		distance[j] = new double[num_item];
	}

	item_category = new Category[num_item];
	for (int j = 0; j < num_item; j++)
		item_category[j] = { { j }, 1 };

	FILE *file_itemcluster_log;
	if ((fopen_s(&file_itemcluster_log, path_itemcluster_log, "a")) == 0)
	{
		//初始化距离矩阵
		fprintf(file_itemcluster_log, "初始距离矩阵：\n");

		for (int i = 0; i < num_item; i++)
		{
			for (int j = 0; j < num_item; j++)
			{
			    distance[i][j] = item_item->item_item_simi[i][j];
				fprintf(file_itemcluster_log, "%d,%d： %lf\n", i, j, distance[i][j]);
			}
		}
	}
	fclose(file_itemcluster_log);

}

ItemCluster::~ItemCluster()
{
	for (int j = 0; j < num_item; j++)
		delete[]distance[j];
	delete[]distance;

	delete[]item_category;

	delete path_log;
}

int ItemCluster::Merge(int step)
{
	double min_distance = 0.0;
	int a = 0;
	int b = 0;

	FILE *file_itemcluster_log;
	if ((fopen_s(&file_itemcluster_log, path_itemcluster_log, "a")) == 0)
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
						throw MyException(2, "data error in distance matrix!");
					}
				}
			}
		}

		fprintf(file_itemcluster_log, "%d：\n", step);
		fprintf(file_itemcluster_log, "a=%d,b=%d： %lf\n", a, b, min_distance);

		if (min_distance == 0.0)
		{
			fclose(file_itemcluster_log);
			return 1;
		}

		//合并最小距离类组
		int num_a = item_category[a].num;
		int num_b = item_category[b].num;
		for (int i = 0; i < num_b; i++)
		{
			item_category[a].atom[num_a] = item_category[b].atom[i];
			num_a++;
		}
		item_category[a].num = num_a;
		
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
			for (int j = b; j < step - 1; j++)
			{
				distance[i][j] = distance[i][j + 1];
			}
		}

		for (int j = 0; j < step - 1; j++)
		{
			for (int i = b; i < step - 1; i++)
			{
				distance[i][j] = distance[i + 1][j];
			}
		}

		//类组对象调整
		for (int i = b; i < step - 1; i++)
			item_category[i] = item_category[i + 1];
	}
	fclose(file_itemcluster_log);

	return 0;
}

int ItemCluster::Cluster(int step, int num_category)
{
	while (step > num_category)
	{
		if(Merge(step) == 0)
			step--;
		else
			break;
	}

	FILE *file_itemcluster_log;
	if ((fopen_s(&file_itemcluster_log, path_itemcluster_log, "a")) == 0)
	{
		fprintf(file_itemcluster_log, "距离矩阵：\n");
		for (int i = 0; i < step; i++)
		{
			for (int j = 0; j < step; j++)
			{
				fprintf(file_itemcluster_log, "%d,%d： %lf\n", i, j, distance[i][j]);
			}
		}

		fprintf(file_itemcluster_log, "项目聚类：\n");
		for (int i = 0; i < step; i++)
		{
			fprintf(file_itemcluster_log, "%d： ", i);
			int num_tmp = item_category[i].num;

			for (int j = 0; j < num_tmp; j++)
				fprintf(file_itemcluster_log, "%d  ", item_category[i].atom[j]);
			fprintf(file_itemcluster_log, "\n");
		}

	}
	fclose(file_itemcluster_log);

	return step;
}