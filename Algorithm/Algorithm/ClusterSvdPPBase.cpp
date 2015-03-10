#include "ClusterSvdPPBase.h"

ClusterSvdPPBase::ClusterSvdPPBase(int num_u, int num_i, const char *path_tr, const char *path_te) :FileInit(num_u, num_i, path_tr, path_te)
{

	path_log = new LogInit("..\\log\\", "clustersvdbase.log");
	if (path_log->flag)
		path_clustersvdbase_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//项目分类信息
	item_kinds = new CategoryItem[19];
	item_kinds[0] = { 0, 1, 2 };
	item_kinds[1] = { 2, 150, 149 };
	item_kinds[2] = { 151, 179, 29 };
	item_kinds[3] = { 180, 194, 15 };
	item_kinds[4] = { 195, 252, 58 };
	item_kinds[5] = { 253, 649, 397 };
	item_kinds[6] = { 650, 692, 43 };
	item_kinds[7] = { 693, 740, 48 };
	item_kinds[8] = { 741, 1350, 610 };
	item_kinds[9] = { 1351, 1351, 1};
	item_kinds[10] = { 1352, 1356, 5 };
	item_kinds[11] = { 1357, 1428, 72 };
	item_kinds[12] = { 1429, 1436, 8 };
	item_kinds[13] = { 1437, 1447, 11 };
	item_kinds[14] = { 1448, 1517, 70 };
	item_kinds[15] = { 1518, 1534, 17 };
	item_kinds[16] = { 1535, 1659, 125 };
	item_kinds[17] = { 1660, 1668, 9};
	item_kinds[18] = { 1669, 1681, 13 };

	//项目评分用户数
	num_item_train = new int[num_item];
	for (int i = 0; i < num_item; i++)
		num_item_train[i] = 0;

	//项目平均评分
	item_train_average = new double[num_item];
	for (int i = 0; i < num_item; i++)
		item_train_average[i] = 0.0;

	//用户历史评分项目数
	num_user_train = new int[num_user];
	for (int i = 0; i < num_user; i++)
		num_user_train[i] = 0;

	//用户平均评分
	user_train_average = new double[num_user];
	for (int i = 0; i < num_user; i++)
		user_train_average[i] = 0.0;
	
	//偏置项
	user_train_biases = new double[num_user];
	for (int i = 0; i < num_user; i++)
		user_train_biases[i] = 0.0;
	item_train_biases = new double[num_item];
	for (int i = 0; i < num_item; i++)
		item_train_biases[i] = 0.0;
	
	//训练集、测试集
	data_trains = new double *[num_user];
	for (int i = 0; i < num_user; i++)
		data_trains[i] = new double[num_item];
	data_tests = new double *[num_user];
	for (int i = 0; i < num_user; i++)
		data_tests[i] = new double[num_item];

	//初始化
	for (int i = 0; i < num_user;i++)
	for (int j = 0; j < num_item; j++)
		data_trains[i][j] = data_tests[i][j] = 0.0;
	mark_average = 0.0;
	num_user_average = 0.0;

	FILE *file_clustersvdbase_log;
	if ((fopen_s(&file_clustersvdbase_log, path_clustersvdbase_log, "a")) == 0)
	{
		CreateItemKind *tmp = new CreateItemKind(num_user, num_item, 400, path_tr, path_te, "..\\movielens\\ml-100k\\u.item");
		tmp->Cluster();
       
		//映射项目分类数据
		for (int s = 0; s < 19; s++)
		{
			int seq_item = 0;
			for (int t = 0; t < num_item; t++)
			{
				if (tmp->item_category[t].seq == s)
				{
					for (int i = 0; i < num_user; i++)
					{
						data_trains[i][seq_item + item_kinds[s].start] = data_train[i*num_item + t];
						data_tests[i][seq_item + item_kinds[s].start] = data_test[i*num_item + t];
					}
					seq_item++;
				}
			}
			if (seq_item != item_kinds[s].num)
				throw MyException(6, "item kinds error!");
		}

		fprintf(file_clustersvdbase_log, "分类后的训练集数据：\n");
		for (int i = 0; i < num_user;i++)
		for (int j = 0; j < num_item;j++)
		if (data_trains[i][j]>1e-10)
			fprintf(file_clustersvdbase_log, "%d,%d：%lf\n",i,j,data_trains[i][j]);
		fprintf(file_clustersvdbase_log, "分类后的测试集数据：\n");
		for (int i = 0; i < num_user; i++)
		for (int j = 0; j < num_item; j++)
		if (data_tests[i][j]>1e-10)
			fprintf(file_clustersvdbase_log, "%d,%d：%lf\n", i, j, data_tests[i][j]);

		delete tmp;

	    int num_mark = 0;
	    //获取项目、全局评分均值
	    for (int i = 0; i < num_user; i++)
	    {
		    for (int j = 0; j < num_item; j++)
		    {
			    if (data_trains[i][j] > 1e-10)
			    {
			 	    num_user_train[i] = num_user_train[i] + 1;
					num_item_train[j] = num_item_train[j] + 1;
					user_train_average[i] += data_trains[i][j];
					item_train_average[j] += data_trains[i][j];
				    num_mark = num_mark + 1;
				    mark_average = mark_average + data_trains[i][j];
			    }
		    }
	    }
	    mark_average = mark_average / num_mark;

		fprintf(file_clustersvdbase_log, "全局平均分：");
		fprintf(file_clustersvdbase_log, "%lf\n", mark_average);

		fprintf(file_clustersvdbase_log, "项目评分用户数：\n");
		for (int i = 0; i < num_item; i++)
			fprintf(file_clustersvdbase_log, "%d：%d\n", i, num_item_train[i]);

		//获取用户平均评分
		for (int i = 0; i < num_user; i++)
		{
			if (num_user_train[i] > 0)
				user_train_average[i] = user_train_average[i] / double(num_user_train[i]);
		}
		//获取项目平均评分
		fprintf(file_clustersvdbase_log, "项目评分均值：\n");
		for (int j = 0; j < num_item; j++)
		{
			if (num_item_train[j] > 0)
				item_train_average[j] = item_train_average[j] / double(num_item_train[j]);
			fprintf(file_clustersvdbase_log, "%d, %lf\n", j, item_train_average[j]);
		}

		fprintf(file_clustersvdbase_log, "用户历史评分项目数：\n");
		for (int i = 0; i < num_user;i++)
			fprintf(file_clustersvdbase_log, "%d：%d\n",i, num_user_train[i]);

		//获取用户历史评分项目
		user_item_history = new int*[num_user];
		user_item_history_coef = new double*[num_user];
		for (int i = 0; i < num_user; i++)
		{
			user_item_history[i] = new int[num_user_train[i]];
			user_item_history_coef[i] = new double[num_user_train[i]];
			
			for (int j = 0; j < num_user_train[i]; j++)
			{
				user_item_history[i][j] = 0;
				user_item_history_coef[i][j] = 0.0;
			}
		}

		fprintf(file_clustersvdbase_log, "用户历史评分项目：\n");
		for (int i = 0; i < num_user; i++)
		{
			fprintf(file_clustersvdbase_log, "\n用户%d：\n", i);
			int user_item_count = 0;
			for (int j = 0; j < num_item; j++)
			{
				if (data_trains[i][j] > 1e-10)
				{
					user_item_history[i][user_item_count] = j;
					fprintf(file_clustersvdbase_log, "%d", user_item_history[i][user_item_count]);

					user_item_history_coef[i][user_item_count] = data_trains[i][j] - user_train_average[i];
					fprintf(file_clustersvdbase_log, "(%lf), ", user_item_history_coef[i][user_item_count]);

					user_item_count++;
				}
			}
			if (user_item_count != num_user_train[i])
		        throw MyException(6, "user's history item error!");
		}

		num_user_average = (double)num_mark / num_user;
		fprintf(file_clustersvdbase_log, "\n平均用户评分项目数：%lf\n", num_user_average);

		//用户聚类信息
		user_kinds = new CategoryUsers[num_user];
		for (int i = 0; i < num_user; i++)
			user_kinds[i] = { i, 0, { 0 }, { 0.0 } };

		UserSimilarity *tmp1 = new UserSimilarity(num_user, num_item, path_tr, path_te);
		tmp1->GetSimiMatrix();
		UserCluster *tmp2 = new UserCluster(num_user,tmp1);
		int num_user_kinds = tmp2->Cluster(num_user,300);

		//用户聚类信息预处理
		for (int i = 0; i < num_user_kinds; i++)
		{
			for (int j = 0; j < tmp2->user_category[i].num; j++)
			{
				int id = tmp2->user_category[i].atom[j];
				
				user_kinds[id].num = tmp2->user_category[i].num;
				for (int s = 0; s < tmp2->user_category[i].num; s++)
				{
					user_kinds[id].atom[s] = tmp2->user_category[i].atom[s];
					user_kinds[id].weight[s] = tmp2->user_category[i].weight[s];
				}
			}
		}

		fprintf(file_clustersvdbase_log, "预处理后的分类用户数据：\n");
		for (int i = 0; i < num_user; i++)
		{
			fprintf(file_clustersvdbase_log, "\n用户：%d\n",i);
			for (int j = 0; j < user_kinds[i].num; j++)
			{
				fprintf(file_clustersvdbase_log, " id:%d|%lf ", user_kinds[i].atom[j], user_kinds[i].weight[j]);
			}
		}

		delete tmp2;
		delete tmp1;
    }
    fclose(file_clustersvdbase_log);
}

ClusterSvdPPBase::~ClusterSvdPPBase()
{
	delete []user_kinds;

	for (int i = 0; i < num_user; i++)
		delete []user_item_history[i];
	delete []user_item_history;

	for (int i = 0; i < num_user; i++)
		delete[]user_item_history_coef[i];
	delete[]user_item_history_coef;

	for (int i = 0; i < num_user; i++)
		delete []data_tests[i];
	delete[]data_tests;

	for (int i = 0; i < num_user; i++)
		delete[]data_trains[i];
	delete[]data_trains;

	delete []item_train_biases;
	delete []user_train_biases;
	
	delete []num_user_train;
	delete []num_item_train;
	delete []item_train_average;
	delete []user_train_average;

	delete []item_kinds;
	delete path_log;
}