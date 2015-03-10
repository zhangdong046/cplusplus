#ifndef MYEXCEPTION_H_
#define MYEXCEPTION_H_
#include "LogInit.h"

class MyException
{
public:
	MyException(int errorId, const char *info);
	MyException(MyException& myExp);
	MyException &operator =(const MyException &other);
	~MyException();

	int getErrorId();

private:
	int m_errorId;
	char *m_error_info;
};

#endif