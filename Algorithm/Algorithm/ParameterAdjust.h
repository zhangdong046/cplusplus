#ifndef PARAMETER_H_
#define PARAMETER_H_
#include "BiasSvd.h"
#include "SvdPlusPlus.h"
#include "ClusterSvdPlusPlus.h"

class ParameterAdjust{
public:
	ParameterAdjust(int user, int item, const char *path_tr, const char *path_te);
	~ParameterAdjust();

	void SetAlpha(double min,double max,double step = 1.0);
	void SetBeta(double min, double max, double step = 1.0);
	void SetGamma(double min = 0.0, double max = 0.0, double step = 1.0);
	void SetDelta(double min = 0.0, double max = 0.0, double step = 1.0);
	void SetEpsilon(double min = 0.0, double max = 0.0, double step = 1.0);
	void SetDimension(int min, int max, int step = 100);
	void SetKindsCluster(int seq, double value = 1.0);

	void Adjust(int seq_alg, int count);

private:
	double alpha_min, alpha_step, alpha_max;
	double beta_min, beta_step, beta_max;
	double gamma_min, gamma_step, gamma_max;
	double delta_min, delta_step, delta_max;
	double epsilon_min, epsilon_step, epsilon_max;
	double *cluster_coef;

	int dimension_min;
	int dimension_step;
	int dimension_max;

	LogInit* path_log;
	char *path_conf_log; //数据处理日志

	int num_user;
	int num_item;
	const char *path_train;
	const char *path_test;
};
#endif