#include "ClusterSvdPlusPlus.h"

ClusterSvdPlusPlus::ClusterSvdPlusPlus(int num_u, int num_i, const ClusterSvdPPBase *b) :num_user(num_u), num_item(num_i), base(b)
{
	path_log = new LogInit("..\\log\\", "clustersvdplusplus.log");
	if (path_log->flag)
		path_clustersvdplusplus_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	mse = rmse = precision = v1 = v2 = s = 0.0;
	phase = 0;

	item_prediction_max = new double[num_item];
	for (int i = 0; i < num_item; i++)
		item_prediction_max[i] = 5.0;
	item_prediction_min = new double[num_item];
	for (int i = 0; i < num_item; i++)
		item_prediction_min[i] = 1.0;

}

ClusterSvdPlusPlus::~ClusterSvdPlusPlus()
{
	delete[]item_prediction_min;
	delete[]item_prediction_max;
	delete path_log;
}

double ClusterSvdPlusPlus::GetMse()
{
	return mse;
}
double ClusterSvdPlusPlus::GetRmse()
{
	return rmse;
}
double ClusterSvdPlusPlus::GetPrecision()
{
	return precision;
}

double ClusterSvdPlusPlus::Grand(int expect, int variance)
{
	double x;
	if (phase == 0) {
		do {
			double u1 = (double)rand() / RAND_MAX;
			double u2 = (double)rand() / RAND_MAX;

			v1 = 2 * u1 - 1;
			v2 = 2 * u2 - 1;
			s = v1 * v1 + v2 * v2;
		} while ((s >= 1.0) || (s < 0.1));

		x = v1 * sqrt(-2 * log(s) / s);
	}
	else
		x = v2 * sqrt(-2 * log(s) / s);

	phase = 1 - phase;
	x = x*variance + expect;
	return x;
}

double ClusterSvdPlusPlus::GetPrediction(int user_id, int item_id, int dimension, int coef, double *sum_w, double epsilon_history, double **P, double **Q, double **W)
{
	double atom1 = 0.0;

	for (int k = 0; k < dimension; k++)
		sum_w[k] = 0.0;

	for (int s = 0; s < base->num_user_train[user_id]; s++)
	{
		for (int k = 0; k < dimension; k++)
		{
			sum_w[k] += (1.0 + epsilon_history * base->user_item_history_coef[user_id][s]) * W[k][base->user_item_history[user_id][s]];
		}
	}

	for (int k = 0; k < dimension; k++)
	{
		sum_w[k] = sum_w[k] * coef;
		atom1 += (P[user_id][k] + sum_w[k]) * Q[k][item_id];
	}

	//矩阵预测评分
	atom1 += base->user_train_biases[user_id] + base->item_train_biases[item_id] + base->mark_average;
	return atom1;
}

double ClusterSvdPlusPlus::GetUserCoef(int user_id, int dimension_id, int dimension, double **P)
{
	double atom2 = 0.0;
	double values = 1.0;
	atom2 += P[user_id][dimension_id];

	for (int t = 0; t < base->user_kinds[user_id].num; t++)
	{
		atom2 -= base->user_kinds[user_id].weight[t] * P[base->user_kinds[user_id].atom[t]][dimension_id];
		
		if (base->user_kinds[user_id].atom[t] == user_id)
			values -= base->user_kinds[user_id].weight[t];
	}

	atom2 = 2.0 * atom2 * values;
	return atom2;
}

double ClusterSvdPlusPlus::GetItemCoef(int item_id, int dimension_id, int kind_id, int dimension, double **Q)
{
	double atom3 = 0.0;
	atom3 += Q[dimension_id][item_id];

	double sum1 = 0.0;
	for (int t = base->item_kinds[kind_id].start; t <= base->item_kinds[kind_id].end; t++)
	{
		sum1 += pow(Q[dimension_id][t], 2);
	}
	sum1 = pow(sum1, 0.5);

	if (sum1 == 0.0)
	{
		atom3 = 0.0;
	}
	else
	{
		atom3 = atom3 / sum1;
	}

	return atom3;
}

void ClusterSvdPlusPlus::Matrix(double **P, double **Q, double **W, int dimension, int max_steps, double alpha_learn, double beta_norm, double gamma_user, double delta_item, double epsilon_history, double cluster_coef[])
{
	double *sum_w = new double[dimension];

	for (int step = 0; step < max_steps; ++step)
	{
		//更新矩阵
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				if (base->data_trains[i][j] > 1e-10)
				{

					//获取项目类型编号
					int kind_id = 0;
					for (int s = 0; s < 19; s++)
					{
						if (j <= base->item_kinds[s].end && j >= base->item_kinds[s].start)
							kind_id = s;
					}

					double coef = (double)base->num_user_train[i];
					if (coef > 1e-10)
						coef = 1 / sqrt(coef);
					else
						coef = 1.0;

					double error = base->data_trains[i][j] + 3.0 - GetPrediction(i, j, dimension, coef, sum_w, epsilon_history, P, Q, W);

					for (int k = 0; k < dimension; k++)
					{
						//更新U、V矩阵
						P[i][k] += alpha_learn * (2 * cluster_coef[kind_id] * error * Q[k][j] - beta_norm * P[i][k]);
						Q[k][j] += alpha_learn * (2 * cluster_coef[kind_id] * error * (P[i][k] + sum_w[k]) - beta_norm * Q[k][j]);
						
					    //同类用户
						P[i][k] += alpha_learn * (0.5) * gamma_user * (-1.0) * GetUserCoef(i, k, dimension, P);
						
						//同类项目
						Q[k][j] += alpha_learn * (0.5) * delta_item * cluster_coef[kind_id] * (-1.0) * GetItemCoef(j, k, kind_id, dimension, Q);
					}

					//更新W矩阵
					for (int s = 0; s < base->num_user_train[i]; s++)
					{
						for (int k = 0; k < dimension; k++)
						{
							W[k][base->user_item_history[i][s]] += alpha_learn * (2 * cluster_coef[kind_id] * (1.0 - epsilon_history * base->user_item_history_coef[i][s]) * coef * error * Q[k][j] - beta_norm * W[k][base->user_item_history[i][s]]);;
						}
					}

					//更新偏置项
					base->user_train_biases[i] += alpha_learn *(2 * cluster_coef[kind_id] * error - beta_norm * base->user_train_biases[i]);
					base->item_train_biases[j] += alpha_learn *(2 * cluster_coef[kind_id] * error - beta_norm * base->item_train_biases[j]);
				}
			}
		}

		//计算损失函数
		double loss = 0.0;
		double sum_user_biases = 0.0;
		double sum_item_biases = 0.0;

		//计算偏置项的平方和
		for (int j = 0; j < num_user; j++)
			sum_user_biases = sum_user_biases + pow(base->user_train_biases[j], 2);
		for (int i = 0; i < num_item; i++)
			sum_item_biases = sum_item_biases + pow(base->item_train_biases[i], 2);

		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (base->data_trains[i][j]>0)
				{
					int kind_id = 0;
					for (int s = 0; s < 19; s++)
					{
						if (j <= base->item_kinds[s].end && j >= base->item_kinds[s].start)
							kind_id = s;
					}

					double coef = (double)base->num_user_train[i];
					if (coef > 1e-10)
						coef = 1 / sqrt(coef);
					else
						coef = 1.0;

					loss += cluster_coef[kind_id] * pow((base->data_trains[i][j] + 3.0 - GetPrediction(i, j, dimension, coef, sum_w, epsilon_history, P, Q, W)), 2);
				}
			}
		}

		for (int i = 0; i < num_user; ++i)
		{
			for (int k = 0; k < dimension; ++k)
				loss += (beta_norm / 2) * pow(P[i][k], 2);
		}

		for (int j = 0; j < num_item; ++j)
		{
			for (int k = 0; k < dimension; ++k)
				loss += (beta_norm / 2) * (pow(Q[k][j], 2) + pow(W[k][j], 2));
		}

		loss += (beta_norm / 2) * (sum_item_biases + sum_user_biases);

		//l1,2范式规范因子
		for (int i = 0; i < 19; i++)
		{
			for (int j = 0; j < dimension; j++)
			{
				double sum_item_kinds = 0.0;
				for (int s = base->item_kinds[i].start; s <= base->item_kinds[i].end; s++)
				{
					sum_item_kinds += pow(Q[j][s], 2);
				}
				sum_item_kinds = pow(sum_item_kinds, 0.5);
				loss += cluster_coef[i] * (0.5) * delta_item * sum_item_kinds;
			}

		}

		//同类用户规范
		for (int i = 0; i < num_user; i++)
		{
			for (int j = 0; j < dimension; j++)
			{
				double sum_user_kinds = 0.0;
				sum_user_kinds += P[i][j];

				for (int s = 0; s < base->user_kinds[i].num; s++)
				{
					sum_user_kinds -= base->user_kinds[i].weight[s] * P[base->user_kinds[i].atom[s]][j];
				}
				sum_user_kinds = pow(sum_user_kinds, 2);

				loss += (0.5) * gamma_user * sum_user_kinds;
			}
		}

		if (loss<0.001)
			break;
		if (step != 0)
		{
			if (step % 2 == 0)
			{
				cout << "loss:" << loss << endl;
				alpha_learn *= (0.9 + 0.1*rand() / RAND_MAX);
			}
		}
	}

	delete []sum_w;
}

void ClusterSvdPlusPlus::InitClusterSvdPlusPlus(int dimension, int max_steps, double alpha_learn, double beta_norm, double gamma_user, double delta_item, double epsilon_history, double cluster_coef[])
{
	double **P = new double *[num_user];
	for (int i = 0; i < num_user; i++)
		P[i] = new double[dimension];

	double **Q = new double *[dimension];
	for (int i = 0; i < dimension; i++)
		Q[i] = new double[num_item];
	double **W = new double *[dimension];
	for (int i = 0; i < dimension; i++)
		W[i] = new double[num_item];

	double **Result = new double *[num_user];
	for (int i = 0; i < num_user; i++)
		Result[i] = new double[num_item];

	for (int i = 0; i < num_user; i++)
	for (int j = 0; j < num_item; j++)
		Result[i][j] = 0.0;

	FILE *file_clustersvdplusplus_log;
	if ((fopen_s(&file_clustersvdplusplus_log, path_clustersvdplusplus_log, "a")) == 0)
	{
		srand((unsigned)time(NULL));

		for (int i = 0; i < num_user; ++i)
		for (int j = 0; j < dimension; ++j)
			P[i][j] = sqrt(abs(Grand(3, 1))) / ((2.0) * pow(dimension, 0.5));

		for (int i = 0; i < dimension; ++i)
		for (int j = 0; j < num_item; ++j)
			Q[i][j] = sqrt(abs(Grand(3, 1))) / pow(dimension, 0.5);
	
		for (int i = 0; i < dimension; ++i)
		for (int j = 0; j < num_item; ++j)
			W[i][j] = sqrt(abs(Grand(3, 1))) / ((2.0) * pow(base->num_user_average, 0.5) * pow(dimension, 0.5));

		cout << "矩阵分解 开始" << endl;
		Matrix(P, Q, W, dimension, max_steps, alpha_learn, beta_norm, gamma_user, delta_item, epsilon_history, cluster_coef);
		cout << "矩阵分解 结束" << endl;

		fprintf(file_clustersvdplusplus_log, "原始预测结果：\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				double atom = 0.0;
				double *tmp_sum = new double[dimension];

				double coef = (double)base->num_user_train[i];
				if (coef > 1e-10)
					coef = 1 / sqrt(coef);
				else
					coef = 1.0;

				Result[i][j] = GetPrediction(i, j, dimension, coef, tmp_sum, epsilon_history, P, Q, W) - 3.0;
				fprintf(file_clustersvdplusplus_log, "%d %d %lf\n", i + 1, j + 1, Result[i][j]);

				delete[]tmp_sum;
			}
		}

		for (int i = 0; i<num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (Result[i][j] - item_prediction_max[j] > 1e-10)
					item_prediction_max[j] = Result[i][j];
				if (item_prediction_min[j] - Result[i][j] > 1e-10)
					item_prediction_min[j] = Result[i][j];
			}
		}

		//以项目为基准映射预测评分
		for (int i = 0; i<num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (base->data_tests[i][j]>0)
				{
					if (base->num_item_train[j] != 0)
					{
						if ((abs(item_prediction_max[j] - base->item_train_average[j])>0.01) && (abs(base->item_train_average[j] - item_prediction_min[j])>0.01))
						{
							if (Result[i][j] - base->item_train_average[j] > 1e-10)
							{
								Result[i][j] = base->item_train_average[j] + (Result[i][j] - base->item_train_average[j])*((5.0 - base->item_train_average[j]) / (item_prediction_max[j] - base->item_train_average[j]));
							}
							else if (base->item_train_average[j] - Result[i][j] > 1e-10)
							{
								Result[i][j] = base->item_train_average[j] - (base->item_train_average[j] - Result[i][j])*((base->item_train_average[j] - 1.0) / (base->item_train_average[j] - item_prediction_min[j]));
							}
						}
					}
					else
					{
						Result[i][j] = 2.0;
					}
				}
			}
		}

		double loss1 = 0.0;
		double loss2 = 0.0;
		int sum_number = 0;
		int right_number = 0;

		//平均绝对误差、均方根误差和正确率
		fprintf(file_clustersvdplusplus_log, "经映射后的预测结果：\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (base->data_tests[i][j]>0)
				{
					sum_number++;
					loss1 += pow(base->data_tests[i][j] - Result[i][j], 2);
					loss2 += abs(base->data_tests[i][j] - Result[i][j]);

					fprintf(file_clustersvdplusplus_log, "%d %d %lf\n", i + 1, j + 1, Result[i][j]);
					Result[i][j] = floor(Result[i][j] + 0.5);

					if (abs(Result[i][j] - base->data_tests[i][j]) < 1e-10)
					{
						right_number++;
					}
				}
			}
		}

		mse = loss2 / sum_number;
		loss1 = loss1 / sum_number;
		rmse = pow(loss1, 0.5);
		precision = double(right_number) / double(sum_number);

	}
	fprintf(file_clustersvdplusplus_log, "MSE:%lf RMSE%lf Precision:%lf\n", mse, rmse, precision);
	fclose(file_clustersvdplusplus_log);

	for (int i = 0; i < num_user; i++)
		delete[]Result[i];
	delete []Result;

	for (int i = 0; i < dimension; i++)
		delete[]W[i];
	delete[]W;
	for (int i = 0; i < dimension; i++)
		delete[]Q[i];
	delete[]Q;

	for (int i = 0; i < num_user; i++)
		delete[]P[i];
	delete[]P;

}