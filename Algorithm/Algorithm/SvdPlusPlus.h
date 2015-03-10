#ifndef SVDPLUSPLUS_H_
#define SVDPLUSPLUS_H_
#include "BiasSvd.h"

//SVD++:考虑邻域影响的LFM
/*
1、使用基本的规范化因子。
2、加入简单的偏置项:全局平均数、用户偏置项、项目偏置项。
3、以项目为基准映射预测评分。
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
	char *path_svdpp_log; //数据处理日志

protected:
	int **user_item_train_history;  //用户历史评分项目
	double num_user_average;  //用户平均评分项目数
};
#endif