#ifndef BIASSVD_H_
#define BIASSVD_H_
#include "Lfm.h"

//BiasSVD:����ƫ������LFM
/*
1��ʹ�û����Ĺ淶�����ӡ�
2������򵥵�ƫ����:ȫ��ƽ�������û�ƫ�����Ŀƫ���
3��P��Q������sqrt(���־�ֵ����̬�ֲ�)/sqrt(�ֽ�ά��)��ʼ����
4������ĿΪ��׼ӳ��Ԥ�����֡�
*/

class BiasSvd :public Lfm{
public:
	BiasSvd(int num_u, int num_i, const char *path_tr, const char *path_te);
	virtual ~BiasSvd();

	void InitBiases(); 
	virtual void Matrix(double *P, double *Q, int dimension, int max_steps, double alpha, double beta);
	virtual void InitBiasSvd(int dimension, int max_steps, double alpha, double beta);

private:
	LogInit* path_log;
	char *path_bsvd_log; //���ݴ�����־

protected:
	double *item_train_biases;  //��Ŀƫ����
	int *num_user_train;  //�û�������������
	double *user_train_biases;  //�û�ƫ����
	double mark_average;  //ѵ�������м�¼�����ֵ�ȫ��ƽ����
};
#endif