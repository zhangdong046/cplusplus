#ifndef LFM_H_
#define LFM_H_
#include "FileInit.h"

//Latent Factor Model:������ģ��(Funk-SVD)
/*
1��ʹ�û����Ĺ淶�����ӡ�
2��δ�����κ�ƫ���
3��P��Q������sqrt(���־�ֵ����̬�ֲ�)/sqrt(�ֽ�ά��)��ʼ����
4������ĿΪ��׼ӳ��Ԥ�����֡�
*/

class Lfm:public FileInit{
public:
	Lfm(int num_u, int num_i, const char *path_tr, const char *path_te);
	virtual ~Lfm();

	virtual void Matrix(double *P, double *Q, int dimension, int max_steps, double alpha, double beta);
	virtual void InitLfm(int dimension, int max_steps, double alpha, double beta);
	
	double Grand(int expect = 3,int variance = 1);//ģ����̬�ֲ�����

	double GetMse();
	double GetRmse();
	double GetPrecision();

private:
	LogInit* path_log;
	char *path_lfm_log; //���ݴ�����־

protected:
	double mse; //ƽ���������
	double rmse; //���������
	double precision; //��ȷ��

	double *item_train_average; //��Ŀ�������־�ֵ
	int *num_item_train; //��Ŀ������������
	double *item_prediction_max; //��ĿԤ���������ֵ
	double *item_prediction_min; //��ĿԤ��������Сֵ

	double v1, v2, s;
	int phase;
};

#endif
//��̬��Ա�����ڴ��������������ʼ���Ҳ����ٴ���static�ؼ���
/*
����������̬��Ա������
static int phase;
���ⶨ�岢��ʼ����̬���ݳ�Ա��
int Lfm::phase = 0;
*/
//��̬��Ա������ʼ��ʱ����static.
/*
����������
static double gaussrand(int expect = 3,int variance = 1);
���ⶨ�壺
double Lfm::gaussrand(int expect, int variance){}
*/
//���������ã�����ͨ�������б���г�ʼ��