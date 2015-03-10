#include "MyException.h"

MyException::MyException(int errorId, const char *info)
{
	m_errorId = errorId;

	if (info == NULL)
	{
		m_error_info = new char[1];
		*m_error_info = '\0';
	}
	else
	{
		int length = strlen(info);
		m_error_info = new char[length + 1];
		strcpy_s(m_error_info, 100, info);
	}
}

MyException::~MyException()
{
	delete[]m_error_info;
}

MyException::MyException(MyException& other)
{
	m_errorId = other.m_errorId;
		
	int length = strlen(other.m_error_info);
	m_error_info = new char[length + 1];
	strcpy_s(m_error_info, 100, other.m_error_info);
}


MyException &MyException::operator =(const MyException &other)
{
	if (this == &other)
		return *this;

	m_errorId = other.m_errorId;

	delete[]m_error_info;
	int length = strlen(other.m_error_info);
	m_error_info = new char[length + 1];
	strcpy_s(m_error_info, 100, other.m_error_info);

	return *this;
}

int MyException::getErrorId()
{
	printf("%s\n", m_error_info);
	return m_errorId;
}