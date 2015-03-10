#ifndef BIASSVD_H_
#define BIASSVD_H_
#include "Lfm.h"

//BiasSVD:加入偏置项后的LFM
/*
1、使用基本的规范化因子。
2、加入简单的偏置项:全局平均数、用户偏置项、项目偏置项。
3、P、Q矩阵以sqrt(评分均值的正态分布)/sqrt(分解维数)初始化。
4、以项目为基准映射预测评分。
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
	char *path_bsvd_log; //数据处理日志

protected:
	double *item_train_biases;  //项目偏置项
	int *num_user_train;  //用户已有评分数量
	double *user_train_biases;  //用户偏置项
	double mark_average;  //训练集所有记录的评分的全局平均数
};
#endif