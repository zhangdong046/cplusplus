#ifndef ITEMINFO_H_
#define ITEMINFO_H_

#include "LogInit.h"

typedef struct item_class
{
	int atom[19];  
	int seq;  
}ItemClass;

class ItemInfo{
public:
	ItemInfo(int num_i, const char *path_i);
	~ItemInfo();

private:
	const char *path_item_info;  
	LogInit *path_log;
	char *path_item_info_log; 

public:
	int num_item;
	ItemClass *item_category;
};

#endif