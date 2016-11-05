// FileFindWDS.cpp - Implementation of CFileFindWDS
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2005 Bernhard Seifert
// Copyright (C) 2004-2016 WinDirStat team (windirstat.info)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "StdAfx.h"
#include "FileFindWDS.h"
#include "windirstat.h"
#include "ListItem.h"

// Function to access the file attributes from outside
DWORD CFileFindWDS::GetAttributes() const
{
	ListItem *used= GetCurrentListItem();
	if (used) {
		return GetFileAttributes(used->fullPath);
	}


    ASSERT(m_hContext != NULL);
    ASSERT_VALID(this);

    if(m_pFoundInfo != NULL)
    {
        return ((LPWIN32_FIND_DATA)m_pFoundInfo)->dwFileAttributes;
    }
    else
    {
        return INVALID_FILE_ATTRIBUTES;
    }
}

// Wrapper for file size retrieval
// This function tries to return compressed file size whenever possible.
// If the file is not compressed the uncompressed size is being returned.
ULONGLONG CFileFindWDS::GetCompressedLength() const
{
#if 0 // TODO: make this an option (the compressed size instead of "normal" size
    ULARGE_INTEGER ret;
    ret.LowPart = ::GetCompressedFileSize(GetFilePath(), &ret.HighPart);

    // Check for error
    if((::GetLastError() != ERROR_SUCCESS) && (ret.LowPart == INVALID_FILE_SIZE))
    {
        // In case of an error return size from CFileFind object
        return GetLength();
    }
    else
    {
        return ret.QuadPart;
    }
#endif // 0
    // Use the file size already found by the finder object
	if (!m_listItem) {
		return GetLength();
	}

	LARGE_INTEGER ret;

	HANDLE file=CreateFile((const LPCTSTR)GetFilePath(), 
		GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	GetFileSizeEx(file, &ret);

	CloseHandle(file);

	return ret.QuadPart;
}

CFileFindWDS::CFileFindWDS(ListItem *listItem):
	m_listItem(listItem),
	m_currentChild(NULL)
{

}


	
BOOL CFileFindWDS::FindFile(LPCTSTR pstrName , DWORD dwUnused ) {
	if (!m_listItem) {
		return CFileFind::FindFile(pstrName, dwUnused);
	}
	return !m_listItem->isFile && !m_listItem->children.empty();
}

BOOL CFileFindWDS::FindNextFile() {
	if (!m_listItem) {
		return CFileFind::FindNextFile();
	}

	if (m_listItem->isFile || m_listItem->children.empty()) {
		return FALSE;
	}

	if (!m_currentChild) {
		m_currentChild = m_listItem->children.begin()->second;
		return m_listItem->children.size()!=1;
	} 

	CString currentName = GetFileName();
	std::map<CString, ListItem*>::iterator next = ++m_listItem->children.find(currentName);
	m_currentChild = next->second;

	return (++next)!=m_listItem->children.end();
}



	
BOOL CFileFindWDS::IsDots() const {
	if (!m_listItem) {
		return CFileFind::IsDots();
	}

	CString fileName = GetFileName();
	return fileName == _T(".") || fileName == _T("..");

}

	
BOOL CFileFindWDS::MatchesMask(DWORD dwMask) const {
	if (!m_listItem) {
		return CFileFind::MatchesMask(dwMask);
	}

	return GetAttributes() & dwMask;
}

	
CString CFileFindWDS::GetFileName() const {

	ListItem *used = GetCurrentListItem();

	if (!used) {
		return CFileFind::GetFileName();
	}

	return used->baseName;
}

CString CFileFindWDS::GetFilePath() const {

	ListItem *used = GetCurrentListItem();

	if (!used) {
		return CFileFind::GetFilePath();
	}

	return used->fullPath;
}


BOOL CFileFindWDS::GetLastWriteTime(FILETIME* pTimeStamp) const {
	if (!m_listItem) {
		return CFileFind::GetLastWriteTime(pTimeStamp);
	}

	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

	if (!GetCurrentListItem()->isFile) {
		dwFlagsAndAttributes |= FILE_FLAG_BACKUP_SEMANTICS;
	}


	HANDLE file=CreateFile((const LPCTSTR)GetFilePath(), 
		GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,dwFlagsAndAttributes,NULL);

	if (file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}


	if (!GetFileTime(file, NULL, NULL, pTimeStamp)) {

		CloseHandle(file);
		return FALSE;
	}

	return TRUE;
}
