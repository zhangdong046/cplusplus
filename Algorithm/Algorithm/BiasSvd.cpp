#include "BiasSvd.h"

BiasSvd::BiasSvd(int num_u, int num_i, const char *path_tr, const char *path_te) : Lfm(num_u, num_i, path_tr, path_te)
{

	mark_average = 0.0;
	//ѵ����������������
	int num_mark = 0;

	//��ʼ����־�ļ�
	path_log = new LogInit("..\\log\\", "biassvd.log");
	if (path_log->flag)
		path_bsvd_log = path_log->path_logs;
	else
	{
		delete path_log;
		throw MyException(1, "create log error!");
	}

	FILE *file_bsvd_log;
	if ((fopen_s(&file_bsvd_log, path_bsvd_log, "a")) == 0)
	{
		//��ʼ����������
		user_train_biases = new double[num_user];
		for (int i = 0; i < num_user; i++)
			user_train_biases[i] = 0.0;
		item_train_biases = new double[num_item];
		for (int i = 0; i < num_item; i++)
			item_train_biases[i] = 0.0;

		num_user_train = new int[num_user];
		for (int i = 0; i < num_user; i++)
			num_user_train[i] = 0;
		
		//��ȡȫ�����־�ֵ
		for (int j = 0; j < num_item; j++)
		{
			for (int i = 0; i < num_user; i++)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					//�û�
					num_user_train[i] = num_user_train[i] + 1;
					//ȫ��
					num_mark = num_mark + 1;
					mark_average = mark_average + data_train[i*num_item + j];
				}
			}
		}

		mark_average = mark_average / num_mark;
		fprintf(file_bsvd_log, "ȫ��ƽ����:  %lf\n", mark_average);
		fprintf(file_bsvd_log, "�û���ʷ������Ŀ����\n");
		for (int i = 0; i < num_user; i++)
			fprintf(file_bsvd_log, "%d��%d\n", i, num_user_train[i]);
	}
	fclose(file_bsvd_log);
}

BiasSvd::~BiasSvd()
{
	delete[]num_user_train;
	delete[]user_train_biases;
	delete[]item_train_biases;
	delete path_log;
}

void BiasSvd::InitBiases()
{
	FILE *file_bsvd_log;
	if ((fopen_s(&file_bsvd_log, path_bsvd_log, "a")) == 0)
	{
		//ƫ����һ��ȫ����ʼ��Ϊ0.0 
		fprintf(file_bsvd_log, "��Ŀƫ�����ʼ����\n");
		for (int j = 0; j < num_item; j++)
		{
			if (item_train_average[j] > 1e-10)
				item_train_biases[j] = item_train_average[j] - mark_average;
			else
				item_train_biases[j] = 0.0;
			fprintf(file_bsvd_log, "%lf\n", item_train_biases[j]);
		}

		fprintf(file_bsvd_log, "�û�ƫ�����ʼ����\n");
		int count;
		for (int i = 0; i < num_user; i++)
		{
			if (num_user_train[i] > 0)
			{
				count = 0;
				for (int j = 0; j < num_item; j++)
				{
					if (data_train[i*num_item + j] > 1e-10)
					{
						user_train_biases[i] = user_train_biases[i] + data_train[i*num_item + j] - item_train_average[j];
						count = count + 1;
					}
				}
				user_train_biases[i] = user_train_biases[i] / (double)count;
			}
			else
				user_train_biases[i] = 0.0;
			fprintf(file_bsvd_log, "%lf\n", user_train_biases[i]);
		}
	}
	fclose(file_bsvd_log);
}

void BiasSvd::Matrix(double *P, double *Q, int dimension, int max_steps, double alpha, double beta)
{
	double loss;
	double sum_user_biases;
	double sum_item_biases;

	for (int step = 0; step < max_steps; ++step)
	{
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				if (data_train[i*num_item + j] > 1e-10)
				{
					double error = data_train[i*num_item + j];
					error += 3.0;
					for (int k = 0; k < dimension; ++k)
						error -= P[i*dimension + k] * Q[k*num_item + j];
					error -= mark_average + user_train_biases[i] + item_train_biases[j];

					//���¹�ʽ��P[i*dimension + k]��ƫ����Ϊ-2*atom * Q[k*num_item + j] + beta * P[i*dimension + k]
					for (int k = 0; k < dimension; ++k)
					{
						P[i*dimension + k] += alpha * (2 * error * Q[k*num_item + j] - beta * P[i*dimension + k]);
						Q[k*num_item + j] += alpha * (2 * error * P[i*dimension + k] - beta * Q[k*num_item + j]);
					}
					
					user_train_biases[i] += alpha*(2 * error - beta * user_train_biases[i]);
					item_train_biases[j] += alpha*(2 * error - beta * item_train_biases[j]);
				}
			}
		}

		loss = 0.0;
		sum_user_biases = 0.0;
		sum_item_biases = 0.0;
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
					double atoms = 0.0;
					for (int k = 0; k < dimension; ++k)
						atoms += P[i*dimension + k] * Q[k*num_item + j];
					loss += pow((data_train[i*num_item + j] + 3.0 - atoms - mark_average -user_train_biases[i]-item_train_biases[j]), 2);
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
}

void BiasSvd::InitBiasSvd(int dimension, int max_steps, double alpha, double beta)
{
	//�������P��Q����
	double *P = new double[num_user*dimension];
	double *Q = new double[dimension*num_item];
	double *Result = new double[num_user*num_item];
	for (int i = 0; i < num_user*num_item; i++)
		Result[i] = 0.0;

	//��ʼ��P,Q����
	FILE *file_bsvd_log;
	if ((fopen_s(&file_bsvd_log, path_bsvd_log, "a")) == 0)
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
		cout << "����ֽ� ��ʼ" << endl;
		Matrix(P, Q, dimension, max_steps, alpha, beta);
		cout << "����ֽ� ����" << endl;

		//����ԭʼԤ����
		fprintf(file_bsvd_log, "ԭʼԤ������\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j < num_item; ++j)
			{
				double atom = 0.0;
				for (int k = 0; k < dimension; ++k)
					atom += P[i*dimension + k] * Q[k*num_item + j];
				Result[i*num_item + j] = atom - 3.0 + mark_average + user_train_biases[i] + item_train_biases[j];
				fprintf(file_bsvd_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);
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
		fprintf(file_bsvd_log, "��ӳ����Ԥ������\n");
		for (int i = 0; i < num_user; ++i)
		{
			for (int j = 0; j<num_item; ++j)
			{
				if (data_test[i*num_item + j]>0)
				{
					sum_number++;
					loss1 += pow(data_test[i*num_item + j] - Result[i*num_item + j], 2);
					loss2 += abs(data_test[i*num_item + j] - Result[i*num_item + j]);
					fprintf(file_bsvd_log, "%d %d %lf\n", i + 1, j + 1, Result[i*num_item + j]);
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
	fprintf(file_bsvd_log, "MSE:%lf RMSE%lf Precision:%lf\n", mse, rmse, precision);
	fclose(file_bsvd_log);
}