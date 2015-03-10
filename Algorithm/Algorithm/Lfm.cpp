#include "Lfm.h"

Lfm::Lfm(int num_u, int num_i, const char *path_tr, const char *path_te) : FileInit(num_u, num_i, path_tr, path_te)
{
	mse = 0.0;
	rmse = 0.0;
	precision = 0.0;
	
	v1 = 0.0;
	v2 = 0.0;
	s = 0.0;
	phase = 0;

	//初始化日志文件
	path_log = new LogInit("..\\log\\", "lfm.log");
	if (path_log->flag)
		path_lfm_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	FILE *file_lfm_log;
	if ((fopen_s(&file_lfm_log, path_lfm_log, "a")) == 0)
	{
		//初始化辅助数组
		num_item_train = new int[num_item];
		for (int i = 0; i < num_item; i++)
			num_item_train[i] = 0;
		item_train_average = new double[num_item];
		for (int i = 0; i < num_item;i++)
			item_train_average[i] = 0.0;

		item_prediction_max = new double[num_item];
		for (int i = 0; i < num_item; i++)
			item_prediction_max[i] = 5.0;
		item_prediction_min = new double[num_item];
		for (int i = 0; i < num_item; i++)
			item_prediction_min[i] = 1.0;

		//获取每个项目已有评分均值
		fprintf(file_lfm_log, "计算项目评分均值：\n");
		for (int j = 0; j < num_item; j++)
		{
			for (int i = 0; i < num_user; i++)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					//项目
					num_item_train[j] = num_item_train[j] + 1;
					item_train_average[j] += data_train[i*num_item + j];
				}
			}
			fprintf(file_lfm_log, "%d:  %lf\n", num_item_train[j], item_train_average[j]);
		}
		for (int j = 0; j < num_item; j++)
		{
			if (num_item_train[j] > 0)
				item_train_average[j] = item_train_average[j] / double(num_item_train[j]);
			fprintf(file_lfm_log, "%lf\n", item_train_average[j]);
		}
	}
	fclose(file_lfm_log);
}

Lfm::~Lfm()
{
	delete []num_item_train;
	delete []item_train_average;
	delete []item_prediction_max;
	delete []item_prediction_min;
	delete path_log;
}

void Lfm::Matrix(double *P, double *Q, int dimension, int max_steps, double alpha, double beta)
{
	double loss;
	for (int step = 0; step < max_steps; ++step)
	{
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					double error = data_train[i*num_item + j];
					for (int k = 0; k < dimension; ++k)
						error -= P[i*dimension + k] * Q[k*num_item + j];
					//更新公式，P[i*dimension + k]的偏导数为-2*atom * Q[k*num_item + j] + beta * P[i*dimension + k]
					for (int k = 0; k < dimension; ++k)
					{
						P[i*dimension + k] += alpha * (2 * error * Q[k*num_item + j] - beta * P[i*dimension + k]);
						Q[k*num_item + j] += alpha * (2 * error * P[i*dimension + k] - beta * Q[k*num_item + j]);
					}
				}
			}
		}
		loss = 0.0;
		//计算每一次迭代后，损失函数的大小
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_train[i*num_item + j]>0)
				{
					double atoms = 0.0;
					for (int k = 0; k < dimension; ++k)
						atoms += P[i*dimension + k] * Q[k*num_item + j];
					loss += pow(data_train[i*num_item + j] - atoms, 2);
				}
			}
		}

		for (int i = 0; i < num_user; ++i)
		{
			for (int k = 0; k < dimension; ++k)
				loss += (beta / 2) * pow(P[i*dimension + k], 2);
		}

		for (int j = 0; j < num_item; ++j)
		{
			for (int k = 0; k < dimension; ++k)
				loss += (beta / 2) * pow(Q[k*num_item + j], 2);
		}

		if (loss<0.001)
			break;
		if (step != 0)
		{
			if (step % 2 == 0)
			{
				cout << "loss:" << loss << endl;
			    alpha *= (0.9 + 0.1*rand()/RAND_MAX);
			}
		}
	}
}

double Lfm::Grand(int expect, int variance)
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

void Lfm::InitLfm(int dimension, int max_steps, double alpha, double beta)
{
	//结果矩阵，P、Q矩阵
	double *P = new double[num_user*dimension];
	double *Q = new double[dimension*num_item];
	double *Result = new double[num_user*num_item];
	for (int i = 0; i < num_user*num_item; i++)
		Result[i] = 0.0;

	//初始化P,Q矩阵
	FILE *file_lfm_log;
	if ((fopen_s(&file_lfm_log, path_lfm_log, "a")) == 0)
	{
		srand((unsigned)time(NULL));
		for (int i = 0; i < num_user; ++i)
		for (int j = 0; j < dimension; ++j)
		{
			P[i*dimension + j] = sqrt(abs(Grand(3, 1))) / pow(dimension, 0.5);
		}
		for (int i = 0; i < dimension; ++i)
		for (int j = 0; j < num_item; ++j)
		{
			Q[i*num_item + j] = sqrt(abs(Grand(3, 1))) / pow(dimension, 0.5);
		}
		cout << "矩阵分解 开始" << endl;
		Matrix(P, Q, dimension, max_steps, alpha, beta);
		cout << "矩阵分解 结束" << endl;

		//计算原始预测结果
		fprintf(file_lfm_log, "原始预测结果：\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				double atom = 0.0;
				for (int k = 0; k < dimension; ++k)
					atom += P[i*dimension + k] * Q[k*num_item + j];
				Result[i*num_item + j] = atom;
				fprintf(file_lfm_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);
			}
		}

		//更新项目预测评分的上下限
		for (int i = 0; i<num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (Result[i*num_item + j] - item_prediction_max[j] > 1e-10)
					item_prediction_max[j] = Result[i*num_item + j];
				if (item_prediction_min[j] - Result[i*num_item + j] > 1e-10)
					item_prediction_min[j] = Result[i*num_item + j];
			}
		}

		//以项目为基准映射预测评分
		for (int i = 0; i<num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_test[i*num_item + j]>0)
				{
					//训练集中项目存在用户评分
					if ((item_train_average[j] != 0) || (num_item_train[j] != 0))
					{
						//排除极端情况
						if ((abs(item_prediction_max[j] - item_train_average[j])>0.01) && (abs(item_train_average[j] - item_prediction_min[j])>0.01))
						{
							if (Result[i*num_item + j] - item_train_average[j] > 1e-10)
							{
								Result[i*num_item + j] = item_train_average[j] + (Result[i*num_item + j] - item_train_average[j])*((5.0 - item_train_average[j]) / (item_prediction_max[j] - item_train_average[j]));
							}
							else if (item_train_average[j] - Result[i*num_item + j] > 1e-10)
							{
								Result[i*num_item + j] = item_train_average[j] - (item_train_average[j] - Result[i*num_item + j])*((item_train_average[j] - 1.0) / (item_train_average[j] - item_prediction_min[j]));
							}
						}
					}
					else
					{
						Result[i*num_item + j] = 2.0;
					}
				}
			}
		}

		double loss1 = 0.0;
		double loss2 = 0.0;
		int sum_number = 0;
		int right_number = 0;
		//求得平均绝对误差、均方根误差和正确率。
		fprintf(file_lfm_log, "经映射后的预测结果：\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_test[i*num_item + j]>0)
				{
					sum_number++;
					loss1 += pow(data_test[i*num_item + j] - Result[i*num_item + j], 2);
					loss2 += abs(data_test[i*num_item + j] - Result[i*num_item + j]);
					fprintf(file_lfm_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);
					Result[i*num_item + j] = floor(Result[i*num_item + j] + 0.5);
					if (abs(Result[i*num_item + j] - data_test[i*num_item + j]) < 1e-10)
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

		delete[]P;
		delete[]Q;
		delete[]Result;
	}
	fprintf(file_lfm_log, "MSE:%lf RMSE%lf Precision:%lf\n", mse, rmse, precision);
	fclose(file_lfm_log);
}

double Lfm::GetMse()
{
	return mse;
}
double Lfm::GetRmse()
{
	return rmse;
}
double Lfm::GetPrecision()
{
	return precision;
}