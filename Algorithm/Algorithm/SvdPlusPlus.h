#ifndef SVDPLUSPLUS_H_
#define SVDPLUSPLUS_H_
#include "BiasSvd.h"

//SVD++:��������Ӱ���LFM
/*
1��ʹ�û����Ĺ淶�����ӡ�
2������򵥵�ƫ����:ȫ��ƽ�������û�ƫ�����Ŀƫ���
3������ĿΪ��׼ӳ��Ԥ�����֡�
*/

class SvdPlusPlus :public BiasSvd{
public:
	SvdPlusPlus(int num_u, int num_i, const char *path_tr, const char *path_te);
	virtual ~SvdPlusPlus();

	virtual void Matrix(double *P, double *Q, double *W, int dimension, int max_steps, double alpha, double beta);
	virtual void InitSvdPlusPlus(int dimension, int max_steps, double alpha, double beta);
	
	double GetPrediction(int user_id, int item_id, int dimension,int coef, double *tmp, double *P, double *Q, double *W);

private:
	LogInit* path_log;
	char *path_svdpp_log; //���ݴ�����־

protected:
	int **user_item_train_history;  //�û���ʷ������Ŀ
	double num_user_average;  //�û�ƽ��������Ŀ��
};
#endif