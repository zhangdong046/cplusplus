#include "ParameterAdjust.h"

ParameterAdjust::ParameterAdjust(int user, int item, const char *path_tr, const char *path_te) : num_user(user), num_item(item), path_train(path_tr), path_test(path_te)
{
	//初始化参数
	alpha_min = beta_min = gamma_min = delta_min = epsilon_min = 0.0;
	alpha_step = beta_step = gamma_step = delta_step = epsilon_step =1.0;
	alpha_max = beta_max = gamma_max = delta_max = epsilon_max =0.0;
	dimension_min = dimension_step = dimension_max = 1;
    
	cluster_coef = new double[19];
	for (int i = 0; i < 19; i++)
		cluster_coef[i] = 1.0;

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "parameter.log");
	if (path_log->flag)
		path_conf_log = path_log->path_logs;
	else
		throw MyException(1, "create log error!");
}

ParameterAdjust::~ParameterAdjust()
{
	delete path_log;
	delete[]cluster_coef;
}

void ParameterAdjust::SetAlpha(double min, double max, double step)
{
	alpha_min = min;
	alpha_step = step;
	alpha_max = max;
}

void ParameterAdjust::SetBeta(double min, double max, double step)
{
	beta_min = min;
	beta_step = step;
	beta_max = max;
}

void ParameterAdjust::SetGamma(double min, double max, double step)
{
	gamma_min = min;
	gamma_step = step;
	gamma_max = max;
}

void ParameterAdjust::SetDelta(double min, double max, double step)
{
	delta_min = min;
	delta_step = step;
	delta_max = max;
}

void ParameterAdjust::SetEpsilon(double min, double max, double step)
{
	epsilon_min = min;
	epsilon_step = step;
	epsilon_max = max;
}

void ParameterAdjust::SetDimension(int min, int max, int step)
{
	dimension_min = min;
	dimension_step = step;
	dimension_max = max;
}

void ParameterAdjust::SetKindsCluster(int seq, double value)
{
	if (seq<0 || seq>18)
	{
		throw MyException(7,"parameter error!");
	}

	cluster_coef[seq] = value;
}

void ParameterAdjust::Adjust(int seq_alg, int count)
{
	double 	rmse_alpha = 0.0;
	double rmse_beta = 0.0;
	double rmse_gamma = 0.0;
	double rmse_delta = 0.0;
	double rmse_epsilon = 0.0;
	int rmse_dimension = 0;

	double 	mse_alpha = 0.0;
	double mse_beta = 0.0;
	double mse_gamma = 0.0;
	double mse_delta = 0.0;
	double mse_epsilon = 0.0;
	int mse_dimension = 0;

	double 	precision_alpha = 0.0;
	double precision_beta = 0.0;
	double precision_gamma = 0.0;
	double precision_delta = 0.0;
	double precision_epsilon = 0.0;
	int precision_dimension = 0;

	double right_rmse = 10.0;
	double right_mse = 10.0;
	double right_precision = 0.0;

	ClusterSvdPPBase *base = NULL;
	if (seq_alg == 4)
	{
		base = new ClusterSvdPPBase(num_user, num_item, path_train, path_test);
	}

	FILE *file_conf_log;
	if ((fopen_s(&file_conf_log, path_conf_log, "a")) == 0)
	{
		int count_step = 1;

		for (int i = dimension_min; i <= dimension_max; i += dimension_step)
		{
			for (double j = alpha_min; j < alpha_max + 1e-6; j += alpha_step)
			{
				for (double k = beta_min; k < beta_max + 1e-6; k += beta_step)
				{
					for (double s = gamma_min; s < gamma_max + 1e-6; s += gamma_step)
					{
						for (double t = delta_min; t < delta_max + 1e-6; t += delta_step)
						{
							for (double m = epsilon_min; m < epsilon_max + 1e-6; m += epsilon_step)
							{
								double tmp_rmse = 0.0;
								double tmp_mse = 0.0;
								double tmp_precision = 0.0;

								for (int z = 0; z < count; z++)
								{
									switch (seq_alg)
									{
										//Funk-SVD
									case 1:
									{
											  Lfm *tmp = new Lfm(num_user, num_item, path_train, path_test);
											  tmp->InitLfm(i, 200, j, k);
											  tmp_rmse += tmp->GetRmse();
											  tmp_mse += tmp->GetMse();
											  tmp_precision += tmp->GetPrecision();
											  delete tmp;
									}
										break;
										//biasSVD
									case 2:
									{
											  BiasSvd *tmp = new BiasSvd(num_user, num_item, path_train, path_test);
											  //tmp->InitBiases();
											  tmp->InitBiasSvd(i, 200, j, k);
											  tmp_rmse += tmp->GetRmse();
											  tmp_mse += tmp->GetMse();
											  tmp_precision += tmp->GetPrecision();
											  delete tmp;
									}
										break;
										//SVD++
									case 3:
									{
											  SvdPlusPlus *tmp = new SvdPlusPlus(num_user, num_item, path_train, path_test);
											  //tmp->InitBiases();
											  tmp->InitSvdPlusPlus(i, 200, j, k);
											  tmp_rmse += tmp->GetRmse();
											  tmp_mse += tmp->GetMse();
											  tmp_precision += tmp->GetPrecision();
											  delete tmp;
									}
										break;
										//ClusterSVD++
									case 4:
									{
											  ClusterSvdPlusPlus *tmp = new ClusterSvdPlusPlus(num_user, num_item, base);
											  tmp->InitClusterSvdPlusPlus(i, 200, j, k, s, t, m, cluster_coef);
											  tmp_rmse += tmp->GetRmse();
											  tmp_mse += tmp->GetMse();
											  tmp_precision += tmp->GetPrecision();
											  delete tmp;
									}
										break;
									default:
										std::cout << "算法序号错误" << endl;
										return;
									}
								}

								tmp_rmse = tmp_rmse / (double)count;
								tmp_mse = tmp_mse / (double)count;
								tmp_precision = tmp_precision / (double)count;

								std::cout << "第" << count_step << "次试参：" << endl;
								std::cout << "Rmse: " << tmp_rmse << "Mse: " << tmp_mse << "Precision: " << tmp_precision << endl;

								if (right_rmse - tmp_rmse > 1e-10)
								{
									right_rmse = tmp_rmse;
									rmse_alpha = j;
									rmse_beta = k;
									rmse_gamma = s;
									rmse_delta = t;
									rmse_epsilon = m;
									rmse_dimension = i;
								}

								if (right_mse - tmp_mse > 1e-10)
								{
									right_mse = tmp_mse;
									mse_alpha = j;
									mse_beta = k;
									mse_gamma = s;
									mse_delta = t;
									mse_epsilon = m;
									mse_dimension = i;
								}

								if (tmp_precision - right_precision > 1e-10)
								{
									right_precision = tmp_precision;
									precision_alpha = j;
									precision_beta = k;
									precision_gamma = s;
									precision_delta = t;
									precision_epsilon = m;
									precision_dimension = i;
								}

								count_step++;
							}
						}
					}
				}
			}
		}

		std::fprintf(file_conf_log, "本次调参算法:%d\n", seq_alg);
		std::fprintf(file_conf_log, "本次调参最优RMSE:%lf\n", right_rmse);
		std::fprintf(file_conf_log, "相关参数:%d,%lf,%lf,(%lf,%lf,%lf)\n", rmse_dimension, rmse_alpha, rmse_beta, rmse_gamma, rmse_delta, rmse_epsilon);
		std::fprintf(file_conf_log, "本次调参最优MSE:%lf\n", right_mse);
		std::fprintf(file_conf_log, "相关参数:%d,%lf,%lf,(%lf,%lf,%lf)\n", mse_dimension, mse_alpha, mse_beta, mse_gamma, mse_delta, mse_epsilon);
		std::fprintf(file_conf_log, "本次调参最优Precision:%lf\n", right_precision);
		std::fprintf(file_conf_log, "相关参数:%d,%lf,%lf,(%lf,%lf,%lf)\n", precision_dimension, precision_alpha, precision_beta, precision_gamma, precision_delta, precision_epsilon);
	}
    fclose(file_conf_log);

	if (seq_alg == 4)
	{
		delete base;
	}
}