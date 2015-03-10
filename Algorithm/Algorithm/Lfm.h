#ifndef LFM_H_
#define LFM_H_
#include "FileInit.h"

//Latent Factor Model:隐语义模型(Funk-SVD)
/*
1、使用基本的规范化因子。
2、未加入任何偏置项。
3、P、Q矩阵以sqrt(评分均值的正态分布)/sqrt(分解维数)初始化。
4、以项目为基准映射预测评分。
*/

class Lfm:public FileInit{
public:
	Lfm(int num_u, int num_i, const char *path_tr, const char *path_te);
	virtual ~Lfm();

	virtual void Matrix(double *P, double *Q, int dimension, int max_steps, double alpha, double beta);
	virtual void InitLfm(int dimension, int max_steps, double alpha, double beta);
	
	double Grand(int expect = 3,int variance = 1);//模拟正态分布数据

	double GetMse();
	double GetRmse();
	double GetPrecision();

private:
	LogInit* path_log;
	char *path_lfm_log; //数据处理日志

protected:
	double mse; //平均绝对误差
	double rmse; //均方根误差
	double precision; //正确率

	double *item_train_average; //项目已有评分均值
	int *num_item_train; //项目已有评分数量
	double *item_prediction_max; //项目预测评分最大值
	double *item_prediction_min; //项目预测评分最小值

	double v1, v2, s;
	int phase;
};

#endif
//静态成员变量在此声明，在类外初始化且不能再带有static关键字
/*
类内声明静态成员变量：
static int phase;
类外定义并初始化静态数据成员：
int Lfm::phase = 0;
*/
//静态成员函数初始化时不带static.
/*
类内声明：
static double gaussrand(int expect = 3,int variance = 1);
类外定义：
double Lfm::gaussrand(int expect, int variance){}
*/
//常量和引用，必须通过参数列表进行初始化