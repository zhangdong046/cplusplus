#ifndef LOGINIT_H_
#define LOGINIT_H_

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include <time.h>
#include "MyException.h"
using namespace std;

//�ļ����ݴ���

class LogInit{
public:
	LogInit(const char *path_l = "..\\log\\", const char *name_l = "file.log");
	~LogInit();

private:
	const char *path_log;  //��־�ļ�Ŀ¼
	const char *name_log;  //��־�ļ���

public:
	char *path_logs; //��־�ļ�ȫ·��
	bool flag;
};

#endif