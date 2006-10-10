// CheckForUpdateDlg.cpp - implementation file
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2005 Bernhard Seifert
// Copyright (C) 2004-2006 Oliver Schneider (assarbad.net)
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
// Author(s): - bseifert -> bseifert@users.sourceforge.net, bseifert@daccord.net
//            - assarbad -> http://assarbad.net/en/contact
//
// $Id$

#include "stdafx.h"
#include "windirstat.h"
#include "CheckForUpdateDlg.h"

/////////////////////////////////////////////////////////////////////////////

void StartUpdateDialog()
{
	AfxBeginThread(RUNTIME_CLASS(CUpdateThread), NULL);
}


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CUpdateThread, CWinThread);

BOOL CUpdateThread::InitInstance()
{
	CWinThread::InitInstance();

	CCheckForUpdateDlg dlg;
	dlg.DoModal();
	return false;
}

// CCheckForUpdateDlg dialog

IMPLEMENT_DYNAMIC(CCheckForUpdateDlg, CDialog)
CCheckForUpdateDlg::CCheckForUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckForUpdateDlg::IDD, pParent)
{
}

CCheckForUpdateDlg::~CCheckForUpdateDlg()
{
}

void CCheckForUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT, DescriptiveText);
	DDX_Control(pDX, IDOK, CloseButton);
	DDX_Control(pDX, IDC_STATIC_URL, UpdateUrl);
}


BEGIN_MESSAGE_MAP(CCheckForUpdateDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CCheckForUpdateDlg message handlers

void CCheckForUpdateDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CCheckForUpdateDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	UpdateFromUrl(_T(IDSS_CHECKUPDATESRV), _T(IDSS_CHECKUPDATEURI), IDXS_CHECKUPDATEPORT);
}

void CCheckForUpdateDlg::UpdateFromUrl(CString server, CString uri, INTERNET_PORT port)
{
	// This code should also work on Windows 9x according to MS
	try
	{
		DWORD dwStatusCode = HTTP_STATUS_OK;
		CString WDSagent;
		const UINT CHUNK_SIZE = 0x800;
		// We pass the current version via the agent string!
		// It contains all information (Ansi/Unicode/Debug or not ... etc)
		WDSagent.Format(TEXT("WinDirStat/%d.%d.%d [Build %d] (%s%4.4X)%s"), VN_MAJOR, VN_MINOR, VN_REVISION, VN_BUILD, _T(UASPEC), GetApp()->GetLangid(), _T(DRSPEC));
		// Create a session
		DescriptiveText.SetWindowText(TEXT("Trying to connect ..."));
		CInternetSession Session(WDSagent, 1, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
		// Connect to our server
		DescriptiveText.SetWindowText(TEXT("Establishing link to server ..."));
		CHttpConnection* pHttpConn = Session.GetHttpConnection(server, INTERNET_FLAG_NO_AUTO_REDIRECT, port, NULL, NULL);
		// Prepare fetching the update info ...
		DescriptiveText.SetWindowText(TEXT("Fetching file ..."));
		CHttpFile* pHttpFile = pHttpConn->OpenRequest(TEXT("GET"), uri, NULL, 1, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD);
		// Tell what kind of data we expect
		pHttpFile->AddRequestHeaders(TEXT("Content-Type: text/plain; charset=utf-8"));
		pHttpFile->SendRequest();
		// Query the status
		pHttpFile->QueryInfoStatusCode(dwStatusCode);
		if(HTTP_STATUS_OK == dwStatusCode)
		{
			CString Response;
			CHAR szBuf[CHUNK_SIZE] = {0};
			UINT nBytesRead;

			do
			{
				nBytesRead = pHttpFile->Read((void*) szBuf, CHUNK_SIZE);
				Response += CString(szBuf);
				if(nBytesRead < CHUNK_SIZE)
				{
					break;
				}
			} while(CHUNK_SIZE == nBytesRead);

			DescriptiveText.SetWindowText(Response);
		}
		else
		{
			// Unknown update status!
		}

		if(pHttpFile)
		{
			pHttpFile->Close();
			delete pHttpFile;
			pHttpFile = NULL;
		}

		if(pHttpConn)
		{
			pHttpConn->Close();
			delete pHttpConn;
			pHttpConn = NULL;
		}
 	}
	catch(CInternetException* e)
	{
		TCHAR ErrorMsg[MAX_PATH+1];
		e->GetErrorMessage(ErrorMsg, MAX_PATH);
		DescriptiveText.SetWindowText(ErrorMsg);
	}
}

// $Log$
// Revision 1.5  2006/10/10 01:41:50  assarbad
// - Added credits for Gerben Wieringa (Dutch translation)
// - Replaced Header tag by Id for the CVS tags in the source files ...
// - Started re-ordering of the files inside the project(s)/solution(s)
//
// Revision 1.4  2006/07/04 23:37:39  assarbad
// - Added my email address in the header, adjusted "Author" -> "Author(s)"
// - Added CVS Log keyword to those files not having it
// - Added the files which I forgot during last commit
//
