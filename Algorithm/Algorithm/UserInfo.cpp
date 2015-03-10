#include "UserInfo.h"

UserInfo::UserInfo(int num_u, const char *path_u) :num_user(num_u),path_user_info(path_u)
{
	//初始化日志文件
	path_log = new LogInit("..\\log\\", "usercluster.log");
	if (path_log->flag)
		path_user_info_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	//初始化用户信息数组
	user_info = new int *[num_user];
	for (int i = 0; i < num_user; i++)
		user_info[i] = new int[3];
	for (int i = 0; i < num_user;i++)
	for (int j = 0; j < 3; j++)
		user_info[i][j] = 0;

	FILE *file_user_info;
	FILE *file_user_info_log;
	if ((fopen_s(&file_user_info_log, path_user_info_log, "a")) == 0)
	{
		fprintf(file_user_info_log, "读取用户信息：\n");

		if ((fopen_s(&file_user_info, path_user_info, "r")) != 0)
		{
			num_user = 0;
		}
		else
		{
			int id = 0;
			int age = 0;
			char gender = 'M';
			char profession[3];

			while (!feof(file_user_info))//注意文本末尾有空行也会进入循环
			{

				fscanf_s(file_user_info, "%d|%d|%c|%*s", &id, &age, &gender);

				id = id - 1;

				user_info[id][0] = age;

				switch (gender)
				{
				case 'M':
				{
							user_info[id][1] = 0;
				}
					break;
				case 'F':
				{
							user_info[id][1] = 1;
				}
					break;
				default:
					cout << "性别读取错误" << endl;
					return;
				}
			}

		    fseek(file_user_info, 0L, SEEK_SET);
			char str_line[1024];
			id = 0;

			while (!feof(file_user_info))
			{
				fgets(str_line, 1024, file_user_info);

				int i = 0;
				int j = 0;
				int count = 0;
				while (j < 3)
				{
					if (str_line[i] == '|')
					{
						i++;
						count++;
					}
					else if ((count == 3) && (j < 3))
					{
						profession[j] = str_line[i];
						i++;
						j++;
					}
					else
						i++;
				}

				if (profession[0] == 'a'&&profession[1] == 'd'&&profession[2] == 'm')//administrator
				{
					user_info[id][2] = 0;
				}
				else if (profession[0] == 'a'&&profession[1] == 'r'&&profession[2] == 't')//artist
				{
					user_info[id][2] = 1;
				}
				else if (profession[0] == 'd'&&profession[1] == 'o'&&profession[2] == 'c')//doctor
				{
					user_info[id][2] = 2;
				}
				else if (profession[0] == 'e'&&profession[1] == 'd'&&profession[2] == 'u')//educator
				{
					user_info[id][2] = 3;
				}
				else if (profession[0] == 'e'&&profession[1] == 'n'&&profession[2] == 'g')//engineer
				{
					user_info[id][2] = 4;
				}
				else if (profession[0] == 'e'&&profession[1] == 'n'&&profession[2] == 't')//entertainment
				{
					user_info[id][2] = 5;
				}
				else if (profession[0] == 'e'&&profession[1] == 'x'&&profession[2] == 'e')//executive
				{
					user_info[id][2] = 6;
				}
				else if (profession[0] == 'h'&&profession[1] == 'e'&&profession[2] == 'a')//healthcare
				{
					user_info[id][2] = 7;
				}
				else if (profession[0] == 'h'&&profession[1] == 'o'&&profession[2] == 'm')//homemaker
				{
					user_info[id][2] = 8;
				}
				else if (profession[0] == 'l'&&profession[1] == 'a'&&profession[2] == 'w')//lawyer
				{
					user_info[id][2] = 9;
				}
				else if (profession[0] == 'l'&&profession[1] == 'i'&&profession[2] == 'b')//librarian
				{
					user_info[id][2] = 10;
				}
				else if (profession[0] == 'm'&&profession[1] == 'a'&&profession[2] == 'r')//marketing
				{
					user_info[id][2] = 11;
				}
				else if (profession[0] == 'n'&&profession[1] == 'o'&&profession[2] == 'n')//none
				{
					user_info[id][2] = 12;
				}
				else if (profession[0] == 'o'&&profession[1] == 't'&&profession[2] == 'h')//other
				{
					user_info[id][2] = 13;
				}
				else if (profession[0] == 'p'&&profession[1] == 'r'&&profession[2] == 'o')//programmer
				{
					user_info[id][2] = 14;
				}
				else if (profession[0] == 'r'&&profession[1] == 'e'&&profession[2] == 't')//retired
				{
					user_info[id][2] = 15;
				}
				else if (profession[0] == 's'&&profession[1] == 'a'&&profession[2] == 'l')//salesman
				{
					user_info[id][2] = 16;
				}
				else if (profession[0] == 's'&&profession[1] == 'c'&&profession[2] == 'i')//scientist
				{
					user_info[id][2] = 17;
				}
				else if (profession[0] == 's'&&profession[1] == 't'&&profession[2] == 'u')//student
				{
					user_info[id][2] = 18;
				}
				else if (profession[0] == 't'&&profession[1] == 'e'&&profession[2] == 'c')//technician
				{
					user_info[id][2] = 19;
				}
				else if (profession[0] == 'w'&&profession[1] == 'r'&&profession[2] == 'i')//writer
				{
					user_info[id][2] = 20;
				}
				else
				{
					cout << "职业读取错误" << endl;
					return;
				}

				fprintf(file_user_info_log, "%d|%d|%d|%d\n", id, user_info[id][0], user_info[id][1], user_info[id][2]);
				id = id + 1;
			}        
		}
		fclose(file_user_info);
	}
	fclose(file_user_info_log);
}

UserInfo::~UserInfo()
{
	delete path_log;

	for (int i = 0; i < num_user; i++)
		delete []user_info[i];
	delete []user_info;
}