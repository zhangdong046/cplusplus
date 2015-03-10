#include "SvdPlusPlus.h"

SvdPlusPlus::SvdPlusPlus(int num_u, int num_i, const char *path_tr, const char *path_te) : BiasSvd(num_u, num_i, path_tr, path_te)
{
	num_user_average = 0.0;
	
	//��ʼ����־�ļ�
	path_log = new LogInit("..\\log\\", "svdpp.log");
	if (path_log->flag)
		path_svdpp_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	FILE *file_svdpp_log;
	if ((fopen_s(&file_svdpp_log, path_svdpp_log, "a")) == 0)
	{
		//��ʼ����������
		user_item_train_history = new int*[num_user];
		for (int i = 0; i < num_user; i++)
		{
			user_item_train_history[i] = new int[num_user_train[i]];
			for (int j = 0; j < num_user_train[i]; j++)
				user_item_train_history[i][j] = 0;
		}

		//��ȡ�û���ʷ������Ŀ
		fprintf(file_svdpp_log, "�û���ʷ������Ŀ��\n");
		for (int i = 0; i < num_user; i++)
		{
			fprintf(file_svdpp_log, "\n�û�%d��\n", i);
			int user_item_count = 0;
			for (int j = 0; j < num_item; j++)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					user_item_train_history[i][user_item_count] = j;
					user_item_count++;
					fprintf(file_svdpp_log, "%d, ", j);
				}
			}
			num_user_average += double(num_user_train[i]);
		}
		num_user_average = num_user_average / num_user;
	}
	fclose(file_svdpp_log);
}

SvdPlusPlus::~SvdPlusPlus()
{
	for (int i = 0; i < num_user; i++)
	{
		delete[]user_item_train_history[i];
	}
	delete[]user_item_train_history;
}

double SvdPlusPlus::GetPrediction(int user_id, int item_id, int dimension, int coef, double *tmp, double *P, double *Q, double *W)
{
	double atom = 0.0;

	//��ʼ���������飬���Դ洢w������������
	for (int k = 0; k < dimension; k++)
		tmp[k] = 0.0;

	for (int s = 0; s < num_user_train[user_id]; s++)
	{
		//���û���ʷ��Ŀ������W�������������
		for (int k = 0; k < dimension; k++)
		{
			tmp[k] += W[k * num_item + user_item_train_history[user_id][s]];
		}
	}

	for (int k = 0; k < dimension; k++)
	{
		tmp[k] = tmp[k] * coef;

		//��P�����û���������+sum(W����������)/sqrt(user_num_history)��*��Q������Ŀj����������
		atom += (P[user_id*dimension + k] + tmp[k])*Q[k*num_item + item_id];
	}

	//����Ԥ������
	atom += user_train_biases[user_id] + item_train_biases[item_id] + mark_average;
	return atom;
}

void SvdPlusPlus::Matrix(double *P, double *Q, double *W, int dimension, int max_steps, double alpha, double beta)
{
	//�����������Դ洢w������������
	double *tmp = new double[dimension];

	for (int step = 0; step < max_steps; ++step)
	{
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{

					double coef = (double)num_user_train[i];
					if (coef > 1e-10)
						coef = 1 / sqrt(coef);
					else
						coef = 1.0;

					//����Ԥ�����������ʵ�����ֲ�
					double error = data_train[i*num_item + j] + 3.0 - GetPrediction(i, j, dimension, coef, tmp, P, Q, W);

					//����U��V����
					for (int k = 0; k < dimension; k++)
					{
						P[i*dimension + k] += alpha * (2 * error * Q[k*num_item + j] - beta * P[i*dimension + k]);
						Q[k*num_item + j] += alpha * (2 * error * (P[i*dimension + k] + tmp[k]) - beta * Q[k*num_item + j]);
					}

					//����W����
					for (int s = 0; s < num_user_train[i]; s++)
					{
						for (int k = 0; k < dimension; k++)
						{
							W[k * num_item + user_item_train_history[i][s]] += alpha * (2 * coef * error * Q[k*num_item + j] - beta * W[k * num_item + user_item_train_history[i][s]]);;
						}
					}

					//����ƫ�����ȫ��ƽ��ֵ
					user_train_biases[i] += alpha*(2 * error - beta * user_train_biases[i]);
					item_train_biases[j] += alpha*(2 * error - beta * item_train_biases[j]);
					mark_average += 2 * alpha * error;
				}
			}
		}
		
		double loss = 0.0;
		double sum_user_biases = 0.0;
		double sum_item_biases = 0.0;
		//����ƫ�����ƽ����
		for (int j = 0; j < num_user; j++)
		{
			sum_user_biases = sum_user_biases + pow(user_train_biases[j], 2);
		}
		for (int i = 0; i < num_item; i++)
		{
			sum_item_biases = sum_item_biases + pow(item_train_biases[i], 2);
		}

		//����ÿһ�ε�������ʧ�����Ĵ�С
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_train[i*num_item + j]>0)
				{

					double coef = (double)num_user_train[i];
					if (coef > 1e-10)
						coef = 1 / sqrt(coef);
					else
						coef = 1.0;

					loss += pow((data_train[i*num_item + j] + 3.0 - GetPrediction(i, j, dimension, coef, tmp, P, Q, W)), 2);
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
				loss += (beta / 2) * (pow(Q[k*num_item + j],2) + pow(W[k*num_item + j], 2));
		}

		loss += (beta / 2) * (sum_item_biases + sum_user_biases);

		if (loss<0.001)
			break;
		
		if (step != 0)
		{
			if (step % 2 == 0)
			{
				cout << "loss:" << loss << endl;
				alpha *= (0.9 + 0.1*rand() / RAND_MAX);
			}
		}
	}
	delete []tmp;
}

void SvdPlusPlus::InitSvdPlusPlus(int dimension, int max_steps, double alpha, double beta)
{
	//�������P��Q��W����
	double *P = new double[num_user*dimension];
	double *Q = new double[dimension*num_item];
	double *W = new double[dimension*num_item];
	double *Result = new double[num_user*num_item];
	for (int i = 0; i < num_user*num_item; i++)
		Result[i] = 0.0;

	//��ʼ��P,Q,W����
	FILE *file_svdpp_log;
	if ((fopen_s(&file_svdpp_log, path_svdpp_log, "a")) == 0)
	{
		srand((unsigned)time(NULL));

		for (int i = 0; i < num_user; ++i)
		for (int j = 0; j < dimension; ++j)
		{
			P[i*dimension + j] = sqrt(abs(Grand(3, 1))) / ((2.0)* pow(dimension, 0.5));
		}
		for (int i = 0; i < dimension; ++i)
		for (int j = 0; j < num_item; ++j)
		{
			Q[i*num_item + j] = sqrt(abs(Grand(3, 1))) / pow(dimension, 0.5);
		}
		for (int i = 0; i < dimension; ++i)
		for (int j = 0; j < num_item; ++j)
		{
			W[i*num_item + j] = sqrt(abs(Grand(3, 1))) / ( (2.0) * pow(num_user_average, 0.5) * pow(dimension, 0.5));
		}

		cout << "����ֽ� ��ʼ" << endl;
		Matrix(P, Q, W, dimension, max_steps, alpha, beta);
		cout << "����ֽ� ����" << endl;

		//����ԭʼԤ����
		fprintf(file_svdpp_log, "ԭʼԤ������\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				double atom = 0.0;
				double *tmp = new double[dimension];
				for (int t = 0; t < dimension; t++)
					tmp[t] = 0.0;

				double coef = (double)num_user_train[i];
				if (coef > 1e-10)
					coef = 1 / sqrt(coef);
				else
					coef = 1.0;

				Result[i*num_item + j] = GetPrediction(i, j, dimension, coef, tmp, P, Q, W) - 3.0;
				fprintf(file_svdpp_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);

				delete []tmp;
			}
		}

		//������ĿԤ�����ֵ�������
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

		//����ĿΪ��׼ӳ��Ԥ������
		for (int i = 0; i<num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_test[i*num_item + j]>0)
				{
					//ѵ��������Ŀ�����û�����
					if ((item_train_average[j] != 0) || (num_item_train[j] != 0))
					{
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

		//���ƽ��������������������ȷ�ʡ�
		fprintf(file_svdpp_log, "��ӳ����Ԥ������\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_test[i*num_item + j]>0)
				{
					sum_number++;
					loss1 += pow(data_test[i*num_item + j] - Result[i*num_item + j], 2);
					loss2 += abs(data_test[i*num_item + j] - Result[i*num_item + j]);

					fprintf(file_svdpp_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);
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
		delete[]W;
		delete[]Result;
	}
	fprintf(file_svdpp_log, "MSE:%lf RMSE%lf Precision:%lf\n", mse, rmse, precision);
	fclose(file_svdpp_log);
}