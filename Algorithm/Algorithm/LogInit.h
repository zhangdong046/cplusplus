#ifndef LOGINIT_H_
#define LOGINIT_H_

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include <time.h>
#include "MyException.h"
using namespace std;

//文件数据处理

class LogInit{
public:
	LogInit(const char *path_l = "..\\log\\", const char *name_l = "file.log");
	~LogInit();

private:
	const char *path_log;  //日志文件目录
	const char *name_log;  //日志文件名

public:
	char *path_logs; //日志文件全路径
	bool flag;
};

#endif