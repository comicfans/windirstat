// SelectDrivesDlg.h - Declaration of CDriveItem, CDrivesList and CSelectDrivesDlg
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
// Author(s): - bseifert -> http://windirstat.info/contact/bernhard/
//            - oliver   -> http://windirstat.info/contact/oliver/
//

#ifndef __WDS_SELECTDRIVESDLG_H__
#define __WDS_SELECTDRIVESDLG_H__
#pragma once

#include "ownerdrawnlistcontrol.h"
#include "layout.h"

//
// The dialog has these three radio buttons.
//
enum RADIO
{
    RADIO_ALLLOCALDRIVES,
    RADIO_SOMEDRIVES,
    RADIO_AFOLDER
};


class CDrivesList;

//
// CDriveItem. An item in the CDrivesList Control.
// All methods are called by the gui thread.
//
class CDriveItem: public COwnerDrawnListItem
{
public:
    CDriveItem(CDrivesList *list, LPCTSTR pszPath);
    void StartQuery(HWND dialog, UINT serial);

    void SetDriveInformation(bool success, LPCTSTR name, ULONGLONG total, ULONGLONG free);

    virtual int Compare(const CSortingListItem *other, int subitem) const;

    CString GetPath() const;
    CString GetDrive() const;
    bool IsRemote() const;
    bool IsSUBSTed() const;
    virtual bool DrawSubitem(int subitem, CDC *pdc, CRect rc, UINT state, int *width, int *focusLeft) const;
    virtual CString GetText(int subitem) const;
    int GetImage() const;

private:
    CDrivesList *m_list;    // Backpointer
    CString m_path;         // e.g. "C:\"
    bool m_isRemote;        // Whether the drive type is DRIVE_REMOTE (network drive)

    bool m_querying;        // Information thread is running.
    bool m_success;         // Drive is accessible. false while m_querying is true.

    CString m_name;         // e.g. "BOOT (C:)"
    ULONGLONG m_totalBytes; // Capacity
    ULONGLONG m_freeBytes;  // Free space

    double m_used;          // used space / total space
};

//
// CDriveInformationThread. Does the GetVolumeInformation() call, which
// may hang for ca. 30 sec, it a network drive is not accessible.
//
class CDriveInformationThread: public CWinThread
{
    // Set of all running CDriveInformationThreads.
    // Used by InvalidateDialogHandle().
    static CSet<CDriveInformationThread *, CDriveInformationThread *> _runningThreads;
    static CCriticalSection _csRunningThreads;

    // The objects register and unregister themselves in _runningThreads
    void AddRunningThread();
    void RemoveRunningThread();

public:
    static void InvalidateDialogHandle();
    static void OnAppExit();

    CDriveInformationThread(LPCTSTR path, LPARAM driveItem, HWND dialog, UINT serial);
    virtual BOOL InitInstance();

    LPARAM GetDriveInformation(bool& success, CString& name, ULONGLONG& total, ULONGLONG& free);

private:
    const CString m_path;       // Path like "C:\"
    const LPARAM m_driveItem;   // The list item, we belong to

    CCriticalSection m_cs;  // for m_dialog
    HWND m_dialog;          // synchronized by m_cs
    const UINT m_serial;    // serial number of m_dialog

    // "[out]"-parameters
    CString m_name;         // Result: name like "BOOT (C:)", valid if m_success
    ULONGLONG m_totalBytes; // Result: capacity of the drive, valid if m_success
    ULONGLONG m_freeBytes;  // Result: free space on the drive, valid if m_success
    bool m_success;         // Result: false, iff drive is unaccessible.
};

//
// CDrivesList.
//
class CDrivesList: public COwnerDrawnListControl
{
    DECLARE_DYNAMIC(CDrivesList)
public:
    CDrivesList();
    CDriveItem *GetItem(int i);
    void SelectItem(CDriveItem *item);
    bool IsItemSelected(int i);

    virtual bool HasImages();

    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


//
// CSelectDrivesDlg. The initial dialog, where the user can select
// one or more drives or a folder for scanning.
//
class CSelectDrivesDlg : public CDialog
{
    DECLARE_DYNAMIC(CSelectDrivesDlg)
    enum { IDD = IDD_SELECTDRIVES };
    static CString getFullPathName_(LPCTSTR relativePath);

public:
    CSelectDrivesDlg(CWnd* pParent = NULL);
    virtual ~CSelectDrivesDlg();

    // Dialog Data
    int m_radio;            // out.
    CString m_folderName;   // out. Valid if m_radio = RADIO_AFOLDER
    CStringArray m_drives;  // out. Valid if m_radio != RADIO_AFOLDER

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    void UpdateButtons();

    static UINT _serial;    // Each Instance of this dialog gets a serial number
    CDrivesList m_list;
    CButton m_okButton;
    CStringArray m_selectedDrives;
    CLayout m_layout;
    // Callback function for the dialog shown by SHBrowseForFolder()
    // MUST be static!
    static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedBrowsefolder();
    afx_msg void OnLbnSelchangeDrives();
    afx_msg void OnBnClickedAlllocaldrives();
    afx_msg void OnBnClickedAfolder();
    afx_msg void OnBnClickedSomedrives();
    afx_msg void OnEnChangeFoldername();
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnLvnItemchangedDrives(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnDestroy();
    afx_msg LRESULT OnWmuOk(WPARAM, LPARAM);
    afx_msg LRESULT OnWmuThreadFinished(WPARAM, LPARAM lparam);
    afx_msg void OnSysColorChange();
};

#endif // __WDS_SELECTDRIVESDLG_H__
