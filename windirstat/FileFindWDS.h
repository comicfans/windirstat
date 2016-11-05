// FileFindWDS.h - Declaration of CFileFindWDS
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

#ifndef __WDS_FILEFINDWDS_H__
#define __WDS_FILEFINDWDS_H__
#pragma once
#include <afx.h> // Declaration of prototype for CFileFind

struct ListItem;
class CFileFindWDS : public CFileFind
{
public:
	CFileFindWDS(ListItem *listItem=NULL);
    DWORD GetAttributes() const;
    ULONGLONG GetCompressedLength() const;


	virtual CString GetFileName() const;
	virtual CString GetFilePath() const;

	virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;

	virtual BOOL FindFile(LPCTSTR pstrName = NULL, DWORD dwUnused = 0);
	virtual BOOL FindNextFile();



	virtual BOOL CFileFindWDS::MatchesMask(DWORD dwMask) const;
	virtual BOOL CFileFindWDS::IsDots() const;

	ListItem* GetCurrentListItem() const{ return m_currentChild ? m_currentChild : m_listItem; }
private:
	ListItem *m_listItem;
	ListItem *m_currentChild;
};

#endif // __WDS_FILEFINDWDS_H__
