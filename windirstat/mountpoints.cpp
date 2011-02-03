// mountpoints.cpp - Implementation of CMountPoints
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2005 Bernhard Seifert
// Copyright (C) 2004-2006, 2008 Oliver Schneider (assarbad.net)
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
//            - assarbad -> http://windirstat.info/contact/oliver/
//

#include "stdafx.h"
#include "osspecific.h"

#include "mountpoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CReparsePoints::~CReparsePoints()
{
    Clear();
}

void CReparsePoints::Clear()
{
    m_drive.RemoveAll();

    POSITION pos = m_volume.GetStartPosition();
    while(pos != NULL)
    {
        CString volume;
        PointVolumeArray *pva = NULL;
        m_volume.GetNextAssoc(pos, volume, pva);
        ASSERT_VALID(pva);
        delete pva;
    }
    m_volume.RemoveAll();
}

void CReparsePoints::Initialize()
{
    Clear();

    GetDriveVolumes();
    GetAllMountPoints();
}

void CReparsePoints::GetDriveVolumes()
{
    m_drive.SetSize(32);

    DWORD drives = GetLogicalDrives();
    int i;
    DWORD mask = 0x00000001;
    for(i = 0; i < 32; i++, mask <<= 1)
    {
        CString volume;

        if((drives & mask) != 0)
        {
            CString s;
            s.Format(_T("%c:\\"), i + wds::chrCapA);

            BOOL b = ::GetVolumeNameForVolumeMountPoint(s, volume.GetBuffer(_MAX_PATH), _MAX_PATH);
            volume.ReleaseBuffer();

            if(!b)
            {
                TRACE(_T("GetVolumeNameForVolumeMountPoint(%s) failed.\n"), s);
                volume.Empty();
            }
        }

        m_drive[i]= volume;
    }
}

void CReparsePoints::GetAllMountPoints()
{
    TCHAR volume[_MAX_PATH];
    HANDLE hvol = ::FindFirstVolume(volume, countof(volume));
    if(hvol == INVALID_HANDLE_VALUE)
    {
        TRACE(_T("No volumes found.\r\n"));
        return;
    }

    for(BOOL bContinue = true; bContinue; bContinue = ::FindNextVolume(hvol, volume, countof(volume)))
    {
        PointVolumeArray *pva = new PointVolumeArray;
        ASSERT_VALID(pva);

        DWORD sysflags;
        CString fsname;
        BOOL b = GetVolumeInformation(volume, NULL, 0, NULL, NULL, &sysflags, fsname.GetBuffer(_MAX_PATH), _MAX_PATH);
        fsname.ReleaseBuffer();

        if(!b)
        {
            TRACE(_T("This file system (%s) is not ready.\r\n"), volume);
            m_volume.SetAt(volume, pva);
            continue;
        }

        if((sysflags & FILE_SUPPORTS_REPARSE_POINTS) == 0)
        {
            // No support for reparse points, and therefore for volume
            // mount points, which are implemented using reparse points.
            TRACE(_T("This file system (%s) does not support volume mount points.\r\n"), volume);
            m_volume.SetAt(volume, pva);
            continue;
        }

        TCHAR point[_MAX_PATH];
        HANDLE h = ::FindFirstVolumeMountPoint(volume, point, countof(point));
        if(h == INVALID_HANDLE_VALUE)
        {
            TRACE(_T("No volume mount points found on %s.\r\n"), volume);
            m_volume.SetAt(volume, pva);
            continue;
        }

        for(BOOL bCont = true; bCont; bCont = ::FindNextVolumeMountPoint(h, point, countof(point)))
        {
            CString uniquePath = volume;
            uniquePath += point;
            CString mountedVolume;

            BOOL b = ::GetVolumeNameForVolumeMountPoint(uniquePath, mountedVolume.GetBuffer(_MAX_PATH), _MAX_PATH);
            mountedVolume.ReleaseBuffer();

            if(!b)
            {
                TRACE(_T("GetVolumeNameForVolumeMountPoint(%s) failed.\r\n"), uniquePath);
                continue;
            }

            SPointVolume pv;
            pv.point = point;
            pv.volume = mountedVolume;

            pv.point.MakeLower();

            pva->Add(pv);
        }
        ::FindVolumeMountPointClose(h);

        m_volume.SetAt(volume, pva);
    }

    (void)::FindVolumeClose(hvol);

#ifdef _DEBUG
    POSITION pos = m_volume.GetStartPosition();
    while(pos != NULL)
    {
        CString volume;
        PointVolumeArray *pva = NULL;
        m_volume.GetNextAssoc(pos, volume, pva);
        pva->AssertValid();
    }
#endif
}


bool CReparsePoints::IsVolumeMountPoint(CString path)
{
    if(path.GetLength() < 3 || path[1] != wds::chrColon || path[2] != wds::chrBackslash)
    {
        // Don't know how to make out mount points on UNC paths ###
        return false;
    }

    ASSERT(path.GetLength() >= 3);
    ASSERT(path[1] == wds::chrColon);
    ASSERT(path[2] == wds::chrBackslash);

    if(path.Right(1) != wds::chrBackslash)
    {
        path += _T("\\");
    }

    path.MakeLower();

    CString volume = m_drive[path[0] - wds::chrSmallA];
    path = path.Mid(3);

    return IsVolumeMountPoint(volume, path);
}

// Check whether the current item is a junction point but no volume mount point
// as the latter ones are treated differently (see above).
bool CReparsePoints::IsFolderJunction(CString path)
{
    if(IsVolumeMountPoint(path))
    {
        return false;
    }

    DWORD attr = GetFileAttributes(path);
    if(attr == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    return ((attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
}

bool CReparsePoints::IsVolumeMountPoint(CString volume, CString path)
{
    while(true)
    {
        int i = 0;
        PointVolumeArray *pva;
        if(!m_volume.Lookup(volume, pva))
        {
            TRACE(_T("CMountPoints: Volume(%s) unknown!\r\n"), volume);
            return false;
        }

        CString point;
        for(i  =  0; i < pva->GetSize(); i++)
        {
            point = (*pva)[i].point;
            if(path.Left(point.GetLength()) == point)
            {
                break;
            }
        }
        if(i >= pva->GetSize())
        {
            return false;
        }

        if(path.GetLength() == point.GetLength())
        {
            return true;
        }

        volume = (*pva)[i].volume;
        path = path.Mid(point.GetLength());
    }
}
