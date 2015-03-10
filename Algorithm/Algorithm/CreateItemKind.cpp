#include "CreateItemKind.h"

CreateItemKind::CreateItemKind(int num_u, int num_i, int num_c, const char* path_tr, const char* path_te, const char* item_info) :ItemInfo(num_i, item_info), num_user(num_u), num_cluster(num_c), path_train(path_tr), path_test(path_te)
{
	if (path_tr == NULL || path_te == NULL || item_info == NULL)
		throw MyException(4, "null path!");

	path_log = new LogInit("..\\log\\", "createitemkind.log");
	if (path_log->flag)
		path_createitemkind_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

}

CreateItemKind::~CreateItemKind()
{
	delete path_log;
}

int CreateItemKind::Cluster()
{
	ItemSimilarity *tmp = new ItemSimilarity(num_user, num_item, path_train, path_test);
	tmp->GetSimiMatrix();

	ItemCluster *temp = new ItemCluster(num_item, tmp);
	int num_kind = temp->Cluster(num_item, num_cluster);
	
	for (int i = 0; i < num_kind; i++)
	{
		int max_kind;
		int kinds[19] = { 0 };

		//�þ�����ֻ��һ����Ŀ
		if (temp->item_category[i].num == 1)
		{
			int s = temp->item_category[i].atom[0];
			int right_one,tmp_sum = 0;

			for (int t = 0; t < 19; t++)
			//CreateItemKind::item_category[s]
			if (item_category[s].atom[t] == 1)
			{
				tmp_sum += 1;
				right_one = t;
			}
			//��һ�ַ��ำֵ
			//�������Ŀֻ��һ������
			if (tmp_sum == 1)
				item_category[s].seq = right_one;

			continue;
		}

		//�þ����а��������Ŀ
		for (int j = 0; j < temp->item_category[i].num; j++)
		{
			int s = temp->item_category[i].atom[j];
			for (int t = 0; t < 19;t++)
			if (item_category[s].atom[t] == 1)
			{
				kinds[t] += 1;
			}
		}

		//��ȡ�þ����г��ִ�����������
		int max = 0;
		for (int t = 0; t < 19; t++)
		{
			if (kinds[t] > max)
			{
				max = kinds[t];
				max_kind = t;
			}
		}

		//��һ�ַ��ำֵ
		for (int j = 0; j < temp->item_category[i].num; j++)
		{
			int s = temp->item_category[i].atom[j];
			int right_one,tmp_sum = 0;

			for (int t = 0; t < 19; t++)
			if (item_category[s].atom[t] == 1)
			{
				tmp_sum += 1;
				right_one = t;
			}
			//����Ŀֻ��һ������
			if (tmp_sum == 1)
			{
				item_category[s].seq = right_one;
			}
			else if (item_category[s].atom[max_kind] == 1)
			{
				item_category[s].seq = max_kind;
			}
		}
	}

	FILE *file_createitemkind_log;
	if ((fopen_s(&file_createitemkind_log, path_createitemkind_log, "a")) == 0)
	{
		fprintf(file_createitemkind_log, "��һ�ַ��ำֵ��\n");
		for (int i = 0; i < num_item; i++)
			fprintf(file_createitemkind_log, "%d��%d\n", i, item_category[i].seq);

		//�ڶ��ַ��ำֵ
		for (int i = 0; i < num_item; i++)
		{
			int insert_sort[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
			
			//����Ŀ��δ����
			if (item_category[i].seq == 20)
			{
				//��ȡ���ƶ���ߵ�9����Ŀ
				for (int j = 0; j < num_item; j++)
				{
					double temp_simi = tmp->item_item_simi[i][j];

					if (temp_simi >1e-10)
					{
						for (int k = 0; k < 9; k++)
						{
							if (insert_sort[k] == -1)
							{
								insert_sort[k] = j;
								break;
							}
							else
							{
								//��������
								if (temp_simi > tmp->item_item_simi[i][insert_sort[k]])
								{
									for (int l = 8; l > k; l--)
									{
										insert_sort[l] = insert_sort[l - 1];
									}
									insert_sort[k] = j;
									break;
								}
							}
						}
					}
				}

				int kinds[19] = { 0 };
				for (int j = 0; j < 9; j++)
				{
					if (insert_sort[j] != -1)
					{
						for (int k = 0; k < 19; k++)
						{
							if (item_category[insert_sort[j]].atom[k] == 1)
							{
								kinds[k] += 1;
							}
						}
					}
					else
						break;
				}

				//�Ľ���KNN�㷨
				int max_kinds =0, right_one = 20;
				for (int k = 0; k < 19; k++)
				{
				    if (item_category[i].atom[k] == 1)
					{
						if (kinds[k] >max_kinds)
						{
							max_kinds = kinds[k];
							right_one = k;
						}
					}
				}

				item_category[i].seq = right_one;
			}
		}

		fprintf(file_createitemkind_log, "�ڶ��ַ��ำֵ��\n");
		for (int i = 0; i < num_item; i++)
			fprintf(file_createitemkind_log, "%d��%d\n", i, item_category[i].seq);

		fprintf(file_createitemkind_log, "�����ַ��ำֵ��\n");
		int priority[19] = {3,4,11,16,17,6,14,15,10,7,12,5,18,8,9,14,2,1,0};
		for (int i = 0; i < num_item; i++)
		{
			//��δ����������Ŀ
			if(item_category[i].seq == 20)
			{
				for (int j = 0; j < 19; j++)
				{
					if (item_category[i].atom[priority[j]] == 1)
					{
						item_category[i].seq = priority[j];
						break;
					}
				}
			}
		}

		for (int i = 0; i < num_item; i++)
			fprintf(file_createitemkind_log, "%d��%d\n", i, item_category[i].seq);
	}
	fclose(file_createitemkind_log);

	delete tmp;
	delete temp;
	return 0;
}