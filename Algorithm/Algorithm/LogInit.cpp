#include "LogInit.h"

LogInit::LogInit(const char *path_l, const char *name_l):path_log(path_l), name_log(name_l)
{
	flag = false;

	if (path_log == NULL || name_log == NULL)
	{
		throw MyException(0, "null path of log!");
	}

	path_logs = new char[100];
	const char *p;
	int count = 0;
	
	p = path_log;
	while (*p != '\0')
	{
		path_logs[count] = *p;
		p++;
		count++;
	}

	p = name_log;
	while (*p != '\0')
	{
		path_logs[count] = *p;
		p++;
		count++;
	}
	path_logs[count] = '\0';

	FILE *log;
	if ((fopen_s(&log, path_logs, "w")) == 0)
		flag = true;
	fclose(log);
}

LogInit::~LogInit()
{
	delete path_logs;
}