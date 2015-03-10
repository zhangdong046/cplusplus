#ifndef CLUSTERSVDPLUSPLUS_H_
#define CLUSTERSVDPLUSPLUS_H_
#include "ClusterSvdPPBase.h"

class ClusterSvdPlusPlus
{
public:
	ClusterSvdPlusPlus(int num_u, int num_i, const ClusterSvdPPBase *base);
	~ClusterSvdPlusPlus();

	double Grand(int expect = 3, int variance = 1);
	double GetMse();
	double GetRmse();
	double GetPrecision();

	double GetPrediction(int user_id, int item_id, int dimension, int coef, double *sum_w, double epsilon_history, double **P, double **Q, double **W);
	double GetUserCoef(int user_id, int dimension_id, int dimension, double **P);
	double GetItemCoef(int item_id, int dimension_id, int kind_id,  int dimension, double **Q);

	void Matrix(double **P, double **Q, double **W, int dimension, int max_steps, double alpha_learn, double beta_norm, double gamma_user, double delta_item, double epsilon_history, double cluster_coef[]);
	void InitClusterSvdPlusPlus(int dimension, int max_steps, double alpha_learn, double beta_norm, double gamma_user, double delta_item, double epsilon_history, double cluster_coef[]);

private:
	LogInit* path_log;
	char *path_clustersvdplusplus_log; 

	const ClusterSvdPPBase *base;

	double mse; 
	double rmse; 
	double precision; 

	double *item_prediction_max; 
	double *item_prediction_min; 

	double v1, v2, s;
	int phase;

public:
	int num_user;
	int num_item;
};

#endif