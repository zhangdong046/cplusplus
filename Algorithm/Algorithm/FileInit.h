#ifndef INIT_H_
#define INIT_H_

#include "LogInit.h"

//�ļ����ݴ���

class FileInit{
public:
	FileInit(int num_u, int num_i, const char *path_tr, const char *path_te);
	~FileInit();

private:
	const char *path_train;  //ѵ�����ļ�
	const char *path_test;  //���Լ��ļ�
	LogInit *path_log;
	char *path_file_log;  //���ݴ�����־

protected:
	int num_user; //�û���
	int num_item; //��Ʒ��
	double *data_train; //ѵ��������
	double *data_test; //���Լ�����

};

#endif