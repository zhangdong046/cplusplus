#include "FileInit.h"

FileInit::FileInit(int num_u, int num_i, const char *path_tr, const char *path_te) :num_user(num_u), num_item(num_i), path_train(path_tr), path_test(path_te)
{
	FILE *file_train;
	FILE *file_test;
	FILE *file_log;

	int user_id, item_id, value;

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "file.log");
	if (path_log->flag)
		path_file_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	if (path_train == NULL || path_test == NULL)
	{
		delete path_log;
		throw MyException(0, "null path of log!");
	}

	if ((fopen_s(&file_log, path_file_log, "a")) == 0)
	{
		fprintf(file_log, "训练集文件：\n");

		if ((fopen_s(&file_train, path_train, "r")) != 0)
		{
			num_user = 0;
			num_item = 0;
		}
		else
		{
			//初始化训练集数组
			data_train = new double[num_user*num_item];
			for (int i = 0; i < num_user*num_item; i++)
				data_train[i] = 0.0;

			//读取文件
			while (!feof(file_train))
			{

				fscanf_s(file_train,"%d%d%d%*s",&user_id,&item_id,&value);
				
				int id  = (user_id - 1) * num_item + item_id - 1;
				data_train[id] = (double)value;
				fprintf(file_log, "%d  %d  %lf\n", user_id,item_id,data_train[id]);
			}
		}

		fclose(file_train);
		fprintf(file_log, "测试集文件：\n");

		if ((fopen_s(&file_test, path_test, "r")) != 0)
		{
			num_user = 0;
			num_item = 0;
		}
		else
		{
			//初始化测试集数组
			data_test= new double[num_user*num_item];
			for (int i = 0; i < num_user*num_item; i++)
				data_test[i] = 0.0;

			//读取文件
			while (!feof(file_test))
			{

				fscanf_s(file_test, "%d%d%d%*s", &user_id, &item_id, &value);

				int id = (user_id - 1) * num_item + item_id - 1;
				data_test[id] = (double)value;
				fprintf(file_log, "%d  %d  %lf\n", user_id, item_id, data_test[id]);
			}
		}
		fclose(file_test);
	}
	else
	{
		printf("log error!\n");
	}
	fclose(file_log);
}
FileInit::~FileInit()
{
	delete []data_train;
	delete []data_test;
	delete path_log;
}