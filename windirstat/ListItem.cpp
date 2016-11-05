
#include "stdafx.h"
#include "ListItem.h"

#include <fstream>
#include <string>
#include <algorithm>


ListItem* ListItem::ReadFromDirectory(CString path) {

	std::wfstream fileList(path+_T("\\file.list"));

	ListItem *ret = new ListItem();

	std::wstring line;

	bool atLeastOne = false;
	while (std::getline(fileList,line)){

		atLeastOne = true;

		std::replace(line.begin(), line.end(), _T('/'), _T('\\'));

		CString lineStr(line.c_str());

		lineStr.Trim();

		if (lineStr.IsEmpty()) {
			continue;
		}

		ListItem* parentItem = ret;

		int pos=0;

		CString currentToken;
		while(true){
		
			currentToken = lineStr.Tokenize(_T("\\"), pos);

			if (pos == lineStr.GetLength()+1) {
				break;
			}

			ListItem* inserted = parentItem->children[currentToken]; 
			if(!inserted){
				inserted = new ListItem();
				parentItem->children[currentToken] = inserted;
			}

			inserted->isFile = false;
			inserted->fullPath = path; 

			std::wstring pathToCurrentRoot = line.substr(0, pos-1);
			inserted->baseName = currentToken;
			inserted->fullPath.Append(_T("\\"));
			inserted->fullPath.Append(pathToCurrentRoot.c_str());

			parentItem = inserted;
		} 

		ListItem *leaf = new ListItem();
		leaf->isFile = true;
		leaf->fullPath = path; 
		leaf->fullPath.Append(_T("\\"));
		leaf->fullPath.Append(line.c_str());
		leaf->baseName = currentToken;
		parentItem->children[currentToken] = leaf;
	}

	if (atLeastOne) {
		return ret;
	}
	delete ret;

	return NULL;
}

ListItem::~ListItem() {

	for (std::map<CString, ListItem*>::iterator it = children.begin(); it != children.end(); ++it) {
		delete it->second;
	}
}
