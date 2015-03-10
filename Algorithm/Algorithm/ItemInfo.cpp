#include "ItemInfo.h"

ItemInfo::ItemInfo(int num_i, const char *path_i) :num_item(num_i), path_item_info(path_i)
{
	//初始化日志文件
	path_log = new LogInit("..\\log\\", "iteminfo.log");
	if (path_log->flag)
		path_item_info_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化项目类别数组
	item_category = new ItemClass[num_item];
	for (int i = 0; i < num_item; i++)
		item_category[i] = { { 0 }, 20 };

	FILE *file_item_info;
	FILE *file_item_info_log;
	if ((fopen_s(&file_item_info_log, path_item_info_log, "a")) == 0)
	{
		fprintf(file_item_info_log, "读取项目信息：\n");

		if ((fopen_s(&file_item_info, path_item_info, "r")) != 0)
		{
			num_item = 0;
		}
		else
		{
			int id = 0;
			char str_line[2048];

			while (!feof(file_item_info))
			{
				fgets(str_line, 2048, file_item_info);
	
				int count = 0;
				const char *p = str_line;

				while (count < 5)
				{
					if (*p == '|')
						count++;
					p++;
				}

				//读取项目类别数据
				int tmp_num = 0;
				while (*p != '\0')
				{
					if (*p != '|')
					{
						item_category[id].atom[tmp_num] = *p - '0';
						tmp_num++;
					}
					p++;
				}
				/*fgets()方法会在‘\0’前多出值为-38
				的未知字符导致seq元素被重新赋值*/
				item_category[id].seq = 20;

				//输出项目类别日志
				fprintf(file_item_info_log, "%d：", id);
				for (int i = 0; i < 19; i++)
				{
					if (item_category[id].atom[i] == 1)
					{
						fprintf(file_item_info_log, "%d ",i);
					}
				}
				fprintf(file_item_info_log, "\n");

				id++;
			}
		}
		fclose(file_item_info);
	}
	fclose(file_item_info_log);

}

ItemInfo::~ItemInfo()
{
	delete path_log;

	delete[]item_category;

}