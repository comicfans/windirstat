#pragma once

#include <map>
#include <set>
#include "atlstr.h" 

struct ListItem {

	//key is basename only
	std::map<CString, ListItem*> children;

	static ListItem* ReadFromDirectory(CString path);

	bool isFile;

	CString baseName;
	CString fullPath;

	~ListItem();
};
