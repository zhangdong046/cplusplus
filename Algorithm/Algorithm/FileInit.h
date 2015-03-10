#ifndef INIT_H_
#define INIT_H_

#include "LogInit.h"

//文件数据处理

class FileInit{
public:
	FileInit(int num_u, int num_i, const char *path_tr, const char *path_te);
	~FileInit();

private:
	const char *path_train;  //训练集文件
	const char *path_test;  //测试集文件
	LogInit *path_log;
	char *path_file_log;  //数据处理日志

protected:
	int num_user; //用户数
	int num_item; //物品数
	double *data_train; //训练集数据
	double *data_test; //测试集数据

};

#endif