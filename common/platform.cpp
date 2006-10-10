// platform.cpp - Implementation of PlatformIsWindows9x()
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

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#include <afxwin.h> // MFC  core and standard components
#include "platform.h"

bool PlatformIsWindows9x()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);

	if(!GetVersionEx(&osvi))
	{
		TRACE("GetVersionEx() failed.\r\n");
		return false;
	}

	return (VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId);
}


// $Log$
// Revision 1.8  2006/10/10 01:41:49  assarbad
// - Added credits for Gerben Wieringa (Dutch translation)
// - Replaced Header tag by Id for the CVS tags in the source files ...
// - Started re-ordering of the files inside the project(s)/solution(s)
//
// Revision 1.7  2006/07/04 23:37:39  assarbad
// - Added my email address in the header, adjusted "Author" -> "Author(s)"
// - Added CVS Log keyword to those files not having it
// - Added the files which I forgot during last commit
//
// Revision 1.6  2006/07/04 22:49:19  assarbad
// - Replaced CVS keyword "Date" by "Header" in the file headers
//
// Revision 1.5  2006/07/04 20:45:16  assarbad
// - See changelog for the changes of todays previous check-ins as well as this one!
//
// Revision 1.4  2004/11/13 17:25:17  bseifert
// Test-commit of platform.cpp
//
// Revision 1.3  2004/11/05 16:53:05  assarbad
// Added Date and History tag where appropriate.
//
